
#ifndef POWER_CONTROLLER_BLE_RELAY_H
#define POWER_CONTROLLER_BLE_RELAY_H

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <MockArduino.h>
#endif

#define NUM_PORTS 4
#define PORT0_PIN 2
#define PORT1_PIN 3
#define PORT2_PIN 4
#define PORT3_PIN 5

#define OFF 0
#define ON 1
#define CYCLE 2

class Relay{
    bool ports[NUM_PORTS] {false,false,false,false};
public:
    void begin();
    static uint8_t getPinForPort(uint8_t port);
    void printJSON(Stream *client) const;
    void togglePort(uint8_t portNumber, bool on);
};

#endif //POWER_CONTROLLER_BLE_RELAY_H
