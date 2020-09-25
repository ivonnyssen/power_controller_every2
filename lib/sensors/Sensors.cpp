#include "Sensors.h"
#if defined(ARDUINO)
#else
#include <MockSD.h>
#endif

#define TINY_BME280_I2C
#include <TinyBME280.h>
SensorData sensorData[NUM_SENSOR_RECORDS]{0};
tiny::BME280 bme;
time_t sensorsLastLogTime;

void sensorsMeasureAndLog() {
    for(int i = 0; i < NUM_SENSOR_RECORDS - 1; i++){
        sensorData[i] = sensorData[i + 1];
    }
    sensorsLastLogTime = now();
    sensorData[NUM_SENSOR_RECORDS - 1] = {sensorsLastLogTime, bme.readFixedPressure() / 100.0, bme.readFixedTempC() / 100.0, bme.readFixedHumidity() / 100.0};

#if DEBUG_SENSORS
    char buffer[64] = {0};
    tmElements_t logTime;
    breakTime(sensorData[NUM_SENSOR_RECORDS - 1].readoutTime, logTime);
    sprintf(buffer, "%02d-%02d %02d:%02d, %s, %s, %s", logTime.Month, logTime.Day, logTime.Hour, logTime.Minute,
            String(sensorData[NUM_SENSOR_RECORDS - 1].pressure).c_str(), String(sensorData[NUM_SENSOR_RECORDS - 1].temperature).c_str(), String(sensorData[NUM_SENSOR_RECORDS - 1].humidity).c_str());
    if(Serial) Serial.println(buffer);
#endif
}

void sensorsPrintJson(Stream *client) {
    client->println(R"===({ "values":[)===");
    for(int i = 0; i < NUM_SENSOR_RECORDS; i++){
        char buffer[128] = {0};
        tmElements_t logTime;
        breakTime(sensorData[i].readoutTime, logTime);
        sprintf(buffer, R"===({"time":"%02d-%02d %02d:%02d", "pressure":%s, "temp":%s, "humidity":%s})===",
                logTime.Month, logTime.Day, logTime.Hour, logTime.Minute, String(sensorData[i].pressure).c_str(),
                String(sensorData[i].temperature).c_str(), String(sensorData[i].humidity).c_str());
        client->print(buffer);
        if(i != NUM_SENSOR_RECORDS - 1) {
            client->println(",");
        } else {
            client->println();
        }
    }
    client->println("]}");
}

void sensorsBegin() {
    bme.begin();
}
