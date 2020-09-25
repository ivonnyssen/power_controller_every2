//
// adapted from OverkillSolar Bms here: https://github.com/FurTrader/OverkillSolarBMS
//

/* Copyright 2020 Neil Jansen (njansen1@gmail.com)
 * Copyright 2020 Igor von Nyssen (igor@vonnyssen.com)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef POWER_CONTROLLER_EVERY_BMS_H
#define POWER_CONTROLLER_EVERY_BMS_H

#include <TimeLib.h>
#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <MockArduino.h>
#endif

#include "ProtectionStatus.h"

#define BMS_OPTION_DEBUG true

#define NUM_TEMP_SENSORS 2
#define NUM_CELLS 8
#define RX_BUFFER_SIZE 64

// Constants
#define START_BYTE 0xDD
#define STOP_BYTE  0x77
#define READ       0xA5
#define WRITE      0x5A

// Commands
#define CMD_BASIC_SYSTEM_INFO 0x03
#define CMD_CELL_VOLTAGES     0x04
#define CMD_NAME              0x05
#define CMD_CTL_MOSFET        0xE1


class SoftwareVersion {
public:
    uint8_t major;
    uint8_t minor;

    SoftwareVersion() {
        major = 0;
        minor = 0;
    }

    explicit SoftwareVersion(const uint8_t version) {
        major = version / 0x0Fu & 0b1111u;
        minor = version & 0b1111u;
    }

};

class ProductionDate {
public:
    uint16_t year;
    uint8_t month;
    uint8_t day;

    ProductionDate(){
        day = 01;
        month = 01;
        year = 2000;
    }

    explicit ProductionDate(const uint16_t date) {
        // Production date is stored internally as a uint16_t, bit-packed as follows:
        //         1111110000000000
        // Field   5432109876543210  # bits  offset
        // ======  ================  ======  ======
        // Day:               xxxxx  5       0
        // Month:         xxxx       4       5
        // Year:   xxxxxxx           7       9
        day = date & 0x1fu;
        month = (uint8_t)(date >> 0x05u) & 0x0fu;
        year = 2000u + (date >> 9u);
    }
};

class Bms {
public:
    explicit Bms(Stream *port);

    void begin(uint16_t timeout = 1000); // serial port stream and timeout
    void poll(); // Call this every time you want to poll the Bms
    bool hasComError() const;  // Returns true if there was a timeout or checksum error on the last call

    float totalVoltage;
    float current;
    float balanceCapacity;
    float rateCapacity;
    uint16_t cycleCount;
    ProductionDate productionDate;
    ProtectionStatus protectionStatus;
    SoftwareVersion softwareVersion;
    uint8_t stateOfCharge;  // state of charge, in percent (0-100)
    bool isDischargeFetEnabled;
    bool isChargeFetEnabled;
    uint8_t numCells;
    uint8_t numTemperatureSensors;
    float temperatures[NUM_TEMP_SENSORS]{};
    float cellVoltages[NUM_CELLS]{};
    char name[32]{0};
    float minVoltage24;
    float maxVoltage24;
    float maxCharge24;
    float maxDischarge24;
    time_t lastPollTime;

    void clear24Values();
    void clearFaultCounts();
    bool isBalancing(uint8_t cellNumber) const;
    void setMosfetControl(bool charge, bool discharge);

    void printFaults(Stream *client) const;
    void printCellVoltages(Stream *client);
    void printStates(Stream *client);

    uint8_t basicSystemInfoCommand[7] = {START_BYTE, READ, CMD_BASIC_SYSTEM_INFO, 0x00, 0xFF, 0xFD, STOP_BYTE};
    uint8_t  cellVoltagesCommand[7] = {START_BYTE, READ, CMD_CELL_VOLTAGES, 0x00, 0xFF, 0xFC, STOP_BYTE};
    uint8_t  nameCommand[7] = {START_BYTE, READ, CMD_NAME, 0x00, 0xFF, 0xFB, STOP_BYTE};

    void queryBasicInfo();
    void queryCellVoltages();
    void queryBmsName();

#ifndef UNIT_TEST
private:
#endif
    static uint16_t calculateChecksum(const uint8_t* buffer, int len);
    static void calculateMosfetCommandString(uint8_t *commandString, bool charge, bool discharge);

    static bool validateResponse(uint8_t *buffer, uint8_t command, int bytesReceived);
    void parseBasicInfoResponse(const uint8_t *buffer);
    void parseVoltagesResponse(const uint8_t *buffer);
    void parseNameResponse(const uint8_t *buffer, uint8_t length);

#if BMS_OPTION_DEBUG
    void debug();  // Calling this method will print out the received data to the main serial port
#endif
#ifdef UNIT_TEST
private:
#endif
    Stream* serial;
    bool comError;
    uint32_t balanceStatus;  // The cell balance statuses, stored as a bitfield
};

#endif //POWER_CONTROLLER_EVERY_BMS_H
