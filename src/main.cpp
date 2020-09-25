/*
 * Power Controller on a web page
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Relay module is attached to digital pins 2, 3, 5, 6
 * Ethernet CS pin is 10
 * SD card CS pin is 4
 * Arducam CS pin is 7
 * Battery Management System is attached to RX and TX (Serial 1)
 */

#define DEBUG false

#include <TimeLib.h>
#include <Ntp.h>
#include <webserver.h>
#include <Relay.h>
#include <Sensors.h>
#include <Bms.h>

#define ETHERNET_CS_PIN 10

Relay* relay;

Sensors* sensors;
#define SENSOR_UPDATE_INTERVAL 10

Bms* bms;
#define BMS_UPDATE_INTERVAL 61

WebServer* webServer;

#define PST -8

#ifndef UNIT_TEST
void setup() {
    Serial.begin(256000);

    relay = new Relay;
    sensors = new Sensors;
    bms = new Bms(&Serial1);
    webServer = new WebServer(relay, bms, sensors);

    //get all CS pins to high before we do anything to the bus
    pinMode(ETHERNET_CS_PIN,OUTPUT);
    digitalWrite(ETHERNET_CS_PIN, HIGH);

    webServer->begin();
    ntpBegin(PST);
    relay->begin();
    sensors->begin();
//    bms->begin();
}

void  loop() {

    // listen for incoming requests and handle them
    webServer->handleHttpRequest();

    time_t seconds = now();
    if(seconds % SENSOR_UPDATE_INTERVAL == 0 && seconds != sensors->lastLogTime) {
        sensors->measureAndLog();
    }

/*    if(seconds % BMS_UPDATE_INTERVAL == 0 && seconds != bms->lastPollTime){
        bms->poll();
    }

    if(seconds % SECS_PER_DAY == 0){
        bms->clear24Values();
        bms->clearFaultCounts();
    } */
}

#endif




