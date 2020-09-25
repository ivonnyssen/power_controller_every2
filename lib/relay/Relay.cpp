#include "Relay.h"

uint8_t Relay::getPinForPort(uint8_t port){
    switch (port) {
        case 0:
            return PORT0_PIN;
        case 1:
            return PORT1_PIN;
        case 2:
            return PORT2_PIN;
        case 3:
            return PORT3_PIN;
        default:
            return PORT0_PIN;
    }
}

void Relay::printJSON(Stream *client) const {
    client->println(R"===({ "switches": [)===");
    char buffer[64] = {0};
    sprintf(buffer,R"===({"name": "Imaging Computer 1", "state": %s},)===", ports[0] ? "true" : "false");
    client->println(buffer);
    sprintf(buffer,R"===({"name": "Imaging Computer 2", "state": %s},)===", ports[1] ? "true" : "false");
    client->println(buffer);
    sprintf(buffer,R"===({"name": "Port 3", "state": %s},)===", ports[2] ? "true" : "false");
    client->println(buffer);
    sprintf(buffer,R"===({"name": "Port 4", "state": %s})===", ports[3] ? "true" : "false");
    client->println(buffer);
    client->println(R"===(]})===");
}

void Relay::begin() {
    for(int i=0; i < NUM_PORTS; i++){
        pinMode(getPinForPort(i), OUTPUT);
        ports[i] = digitalRead(getPinForPort(i)) != HIGH;
    }
}

void Relay::togglePort(uint8_t portNumber, bool on) {
    ports[portNumber] = on;
    digitalWrite(getPinForPort(portNumber), on ? LOW : HIGH);
}
