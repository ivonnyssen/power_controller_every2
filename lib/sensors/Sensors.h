
#ifndef POWER_CONTROLLER_BLE_SENSORS_H
#define POWER_CONTROLLER_BLE_SENSORS_H

#include <TimeLib.h>
#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <MockArduino.h>
#include <mocks/MockBME280.h>
#endif

#define DEBUG_SENSORS false
#define NUM_SENSOR_RECORDS 96 //24 * 4

typedef struct SensorData{
    time_t readoutTime;
    float pressure;
    float temperature;
    float humidity;
} SensorData;

void sensorsBegin();
void sensorsMeasureAndLog();
void sensorsPrintJson(Stream *client);
extern time_t sensorsLastLogTime;

#endif //POWER_CONTROLLER_BLE_SENSORS_H
