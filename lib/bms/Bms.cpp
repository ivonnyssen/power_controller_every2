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

#ifdef ARDUINO

#include <Bms.h>

Bms::Bms(Stream *port){
    serial = port;
    totalVoltage = 0;
    current = 0;
    balanceCapacity = 0;
    rateCapacity = 0;
    cycleCount = 0;
    productionDate = ProductionDate(0);
    protectionStatus = ProtectionStatus(0);
    softwareVersion = SoftwareVersion(0);
    stateOfCharge = 0;  // state of charge, in percent (0-100)
    isDischargeFetEnabled = false;
    isChargeFetEnabled = false;
    numCells = 0;
    numTemperatureSensors = 0;
    comError = false;
    balanceStatus = 0;
    lastPollTime = 0;
    minVoltage24 = 0;
    maxVoltage24 = 0;
    maxCharge24 = 0;
    maxDischarge24 = 0;
}

void Bms::begin(uint16_t timeout) {
#if BMS_OPTION_DEBUG
    Serial.println("OverkillSolarBMS Begin!");
#endif
    serial->setTimeout(timeout);
}

void Bms::poll() {
    if (serial != nullptr) {
        lastPollTime = now();
        queryBasicInfo();
        minVoltage24 = totalVoltage < minVoltage24 ? totalVoltage : minVoltage24;
        maxVoltage24 = totalVoltage > maxVoltage24 ? totalVoltage : maxVoltage24;
        maxCharge24 = current > maxCharge24 ? current : maxCharge24;
        maxDischarge24 = current < -maxDischarge24 ? -current : maxDischarge24;
        queryCellVoltages();
        if(name[0] == 0){
            queryBmsName();
        }
    }
}

bool Bms::hasComError() const {
    return comError;
}

bool Bms::isBalancing(uint8_t cellNumber) const {
    if (cellNumber <= numCells) {
        return (balanceStatus >> cellNumber) & 1u;
    }
    else {
        return false;
    }
}

void Bms::clearFaultCounts() {
    protectionStatus.clearFaultCounts();
}

void Bms::setMosfetControl(bool charge, bool discharge) {
#if BMS_OPTION_DEBUG
    Serial.println("Query 0xE1 MOSFET Control");
#endif
    uint8_t data[] = {START_BYTE, WRITE, CMD_CTL_MOSFET, 0x02, 0x00, 0x00, 0x00, 0x00, STOP_BYTE};
    calculateMosfetCommandString(data, charge, discharge);

    serial->write(data, sizeof(data));

    uint8_t buffer[RX_BUFFER_SIZE] {0};
    comError = validateResponse(buffer, CMD_CTL_MOSFET, 0);
}

void Bms::calculateMosfetCommandString(uint8_t * commandString, bool charge, bool discharge) {
    uint8_t xxByte = 0b11;
    xxByte &= charge ? 0b10u : 0b11u;
    xxByte &= discharge ? 0b01u : 0b11u;
    commandString[5] = xxByte;

    uint16_t checksum = calculateChecksum(&commandString[2], 4);
    commandString[6] = (uint8_t) checksum >> 8u;
    commandString[7] = (uint8_t) checksum;
}

void Bms::queryBasicInfo() {
#if BMS_OPTION_DEBUG
    Serial.println("Query 0x03 Basic Info");
#endif
    serial->write(basicSystemInfoCommand, sizeof(basicSystemInfoCommand));

    uint8_t buffer[RX_BUFFER_SIZE] {0};

    int bytesReceived = serial->readBytesUntil((char)STOP_BYTE, buffer, sizeof(buffer));
    comError = validateResponse(buffer, CMD_BASIC_SYSTEM_INFO, bytesReceived);
    if(comError){
        return;
    }
    parseBasicInfoResponse(buffer);
}

void Bms::parseBasicInfoResponse(const uint8_t *buffer) {
    totalVoltage = (uint16_t)(buffer[4] << 8u) | (uint16_t)(buffer[5]);
    current =  (uint16_t)(buffer[6] << 8u) | (uint16_t)(buffer[7]);
    balanceCapacity = (uint16_t)(buffer[8] << 8u) | (uint16_t)(buffer[9]);
    rateCapacity = (uint16_t)(buffer[10] << 8u) | (uint16_t)(buffer[11]);
    cycleCount = (uint16_t)(buffer[12] << 8u) | (uint16_t)(buffer[13]);
    productionDate = ProductionDate((uint16_t)(buffer[14] << 8u) | (uint16_t)(buffer[15]));
    balanceStatus = (uint32_t)(buffer[16] << 8u) | (uint32_t)(buffer[17]) | (uint32_t)(buffer[18] << 24u) | (uint32_t)(buffer[19] << 16u) ;
    protectionStatus.updateProtectionStatus((uint16_t)(buffer[20] << 8u) | (uint16_t)(buffer[21]));
    softwareVersion = SoftwareVersion(buffer[22]);
    stateOfCharge = buffer[23];
    isDischargeFetEnabled = buffer[24] & 0b00000010u;
    isChargeFetEnabled = buffer[24] & 0b00000001u;
    numCells = buffer[25];
    numTemperatureSensors = buffer[26];

    for (int i = 0; i < min(numTemperatureSensors, NUM_TEMP_SENSORS); i++) {
        temperatures[i] = ((uint16_t)(buffer[27 + (i * 2)] << 8u) | (uint16_t)(buffer[28 + (i * 2)]))-2731;
    }
}


void Bms::queryCellVoltages() {
#if BMS_OPTION_DEBUG
    Serial.println("Query 0x04 Cell Voltages");
#endif

    serial->write(cellVoltagesCommand, sizeof(cellVoltagesCommand));

    uint8_t buffer[RX_BUFFER_SIZE] {0};
    int bytesReceived = serial->readBytesUntil((char)STOP_BYTE, buffer, sizeof(buffer));
    comError = validateResponse(buffer, CMD_CELL_VOLTAGES, bytesReceived);
    if(comError){
        return;
    }

    parseVoltagesResponse(buffer);
}

void Bms::parseVoltagesResponse(const uint8_t *buffer) {
    for (int i = 0; i < min(numCells, NUM_CELLS); i++) {
        cellVoltages[i] = (uint16_t)(buffer[i * 2 + 4] << 8u) | (uint16_t)(buffer[i * 2 + 5]);
    }
}

void Bms::queryBmsName() {
#if BMS_OPTION_DEBUG
    Serial.println("Query 0x05 Bms Name");
#endif
    serial->write(nameCommand, sizeof(nameCommand));

    uint8_t buffer[RX_BUFFER_SIZE] {0};
    int bytesReceived = serial->readBytesUntil((char)STOP_BYTE, buffer, sizeof(buffer));
    comError = validateResponse(buffer, CMD_NAME, bytesReceived);
    if(comError){
        return;
    }

    parseNameResponse(buffer, buffer[3]);
}

void Bms::parseNameResponse(const uint8_t *buffer, uint8_t length) {
    memset(&name[0], 0, sizeof(name));
    strncpy(&name[0], reinterpret_cast<const char *>(&buffer[4]), min(sizeof(name), length));
}

uint16_t Bms::calculateChecksum(const uint8_t *buffer, int len) {
    uint16_t checksum =0;
    for(int i = 0; i < len; i++){
        checksum += buffer[i];
    }
    return 0xFFFF - checksum + 1;
}

bool Bms::validateResponse(uint8_t *buffer, uint8_t command, int bytesReceived) {
    if(bytesReceived <= 0) {
        return false;
    }

    if(!(buffer[0] == START_BYTE && buffer[1] == command && buffer[2] == 0x00)){
        return false;
    }

    uint16_t calculatedCheckSum = calculateChecksum(&buffer[02], bytesReceived-4);
    uint16_t transmittedChecksum = ((uint16_t)(buffer[bytesReceived-2]  << 8u) | (uint16_t)(buffer[bytesReceived-1]));
    if(calculatedCheckSum != transmittedChecksum) {
        return false;
    }
    return true;
}

void Bms::clear24Values() {
    minVoltage24 = totalVoltage;
    maxVoltage24 = totalVoltage;
    maxCharge24 = current > 0 ? current : 0;
    maxDischarge24 = current < -maxDischarge24 ? -current : 0;
}

void Bms::printFaults(Stream *client) const {
    protectionStatus.printFaultCounts(client);
}

void Bms::printCellVoltages(Stream *client) {
    client->println(R"===({ "cellVoltages":[)===");
    for(int i = 0; i < NUM_CELLS; i++){
        char buffer[64] = {0};
        sprintf(buffer, R"===({"cell":"%d", "cellVoltage":%s, "balancing": %s})===", i, String(cellVoltages[i]).c_str(), isBalancing(i) ? "true" : "false");
        client->print(buffer);
        if(i != NUM_CELLS - 1) {
            client->println(",");
        } else {
            client->println();
        }
    }
    client->println(R"===(],)===");
}

void Bms::printStates(Stream *client) {
    char buffer[64] = {0};
    sprintf(buffer, R"===("charge": "%sA",)===", current < 0 ? String(0).c_str() : String(current/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("discharge": "%sA",)===", current < 0 ? String(-current/1000).c_str() : String(0).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("totalVoltage": "%sV",)===", String(totalVoltage/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("remainingSOC": %d,)===", stateOfCharge);
    client->println(buffer);
    sprintf(buffer, R"===("minVoltage": "%sV",)===", String(minVoltage24/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("maxVoltage": "%sV",)===", String(maxVoltage24/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("maxCharge": "%sA",)===", String(maxCharge24/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("maxDischarge": "%sA",)===", String(maxDischarge24/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("maxPower": "%sW",)===", String(balanceCapacity/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("temp1": "%sC",)===", String(temperatures[0]/1000).c_str());
    client->println(buffer);
    sprintf(buffer, R"===("temp2": "%sC")===", String(temperatures[1]/1000).c_str());
    client->println(buffer);
    client->println("}");
}

#endif