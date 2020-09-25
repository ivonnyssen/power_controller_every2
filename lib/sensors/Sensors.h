
#ifndef POWER_CONTROLLER_BLE_SENSORS_H
#define POWER_CONTROLLER_BLE_SENSORS_H

#include <TimeLib.h>
#if defined(ARDUINO)
#include <Arduino.h>
#include <Adafruit_BME280.h>
#else
#include <MockArduino.h>
#include <mocks/MockBME280.h>
#endif

#define DEBUG_SENSORS true
#define NUM_SENSOR_RECORDS 96 //24 * 4

typedef struct SensorData{
    time_t readoutTime;
    float pressure;
    float temperature;
    float humidity;
} SensorData;

class Sensors {
public:
    Sensors();
    bool begin();
    void measureAndLog();
    void printJson(Stream *client);
    time_t lastLogTime;
private:
    Adafruit_BME280 bme;
    SensorData sensorData[NUM_SENSOR_RECORDS]{0};
};

#endif //POWER_CONTROLLER_BLE_SENSORS_H
