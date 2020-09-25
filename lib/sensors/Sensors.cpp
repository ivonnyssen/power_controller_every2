#include "Sensors.h"
#if defined(ARDUINO)
#else
#include <MockSD.h>
#endif
void Sensors::measureAndLog() {
    for(int i = 0; i < NUM_SENSOR_RECORDS - 1; i++){
        sensorData[i] = sensorData[i + 1];
    }
    bme.takeForcedMeasurement();
    lastLogTime = now();
    sensorData[NUM_SENSOR_RECORDS - 1] = {lastLogTime, bme.readPressure() / 100.0F, bme.readTemperature(), bme.readHumidity()};

    char buffer[64] = {0};
    tmElements_t logTime;
    breakTime(sensorData[NUM_SENSOR_RECORDS - 1].readoutTime, logTime);
    sprintf(buffer, "%02d-%02d %02d:%02d, %s, %s, %s", logTime.Month, logTime.Day, logTime.Hour, logTime.Minute,
            String(sensorData[NUM_SENSOR_RECORDS - 1].pressure).c_str(), String(sensorData[NUM_SENSOR_RECORDS - 1].temperature).c_str(), String(sensorData[NUM_SENSOR_RECORDS - 1].humidity).c_str());
#if DEBUG_SENSORS
    if(Serial) Serial.println(buffer);
#endif
}

void Sensors::printJson(Stream *client) {
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

bool Sensors::begin() {
    if(!bme.begin()){
        if(Serial) Serial.println("BME280 failed.");
        return false;
    } else {
        bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_OFF   );
    }
    return true;
}

Sensors::Sensors() {
    lastLogTime = 0;
}
