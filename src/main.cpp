/*
 * Power Controller on a web page
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Relay module is attached to digital pins 2, 3, 4, 5
 * Ethernet CS pin is 10
 * Battery Management System is attached to RX and TX (Serial 1)
 */

#include <TimeLib.h>
#include <Ntp.h>
#include <webserver.h>
#include <Relay.h>
#include <Sensors.h>
#include <Bms.h>

#define ETHERNET_CS_PIN 10

Relay* relay;

#define SENSOR_UPDATE_INTERVAL 900

Bms* bms;
#define BMS_UPDATE_INTERVAL 60

WebServer* webServer;

#define PST -8

#ifndef UNIT_TEST
void setup() {
    Serial.begin(9600);

    relay = new Relay;
    bms = new Bms(&Serial1);
    webServer = new WebServer(relay, bms);

    //get all CS pins to high before we do anything to the bus
    pinMode(ETHERNET_CS_PIN,OUTPUT);
    digitalWrite(ETHERNET_CS_PIN, HIGH);

    webServer->begin();
    ntpBegin(PST);
    relay->begin();
    sensorsBegin();
    bms->begin();
}

void  loop() {

    // listen for incoming requests and handle them
    webServer->handleHttpRequest();

    time_t seconds = now();
    if(seconds % SENSOR_UPDATE_INTERVAL == 0 && seconds != sensorsLastLogTime) {
        sensorsMeasureAndLog();
    }

    if(seconds % BMS_UPDATE_INTERVAL == 0 && seconds != bms->lastPollTime){
        bms->poll();
    }

    if(seconds % SECS_PER_DAY == 0){
        bms->clear24Values();
        bms->clearFaultCounts();
    }
}

#endif




