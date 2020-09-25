
#ifndef POWER_CONTROLLER_BLE_WEBSERVER_H
#define POWER_CONTROLLER_BLE_WEBSERVER_H
#include <Bms.h>
#include <Relay.h>
#include <Sensors.h>

#if defined(ARDUINO)
#include <Arduino.h>
#include <Ethernet.h>
#else
#include <MockArduino.h>
#include <MockEthernet.h>
#include <mocks/MockCamera.h>
#endif

#define LISTEN_PORT 80
#define DEBUG_WEB_SERVER true

class Request{
public:
    enum RequestType {
        GET,
        POST,
        UNSUPPORTED
    };

    RequestType type;
    char url[256]{0};
    uint8_t powerPort;
    uint8_t command;
};

class WebServer {
public:
    WebServer(Relay* aRelayPtr, Bms* aBmsPtr, Sensors* aSensorPtr);
    void begin();
    void handleHttpRequest();
#ifndef UNIT_TEST
private:
#endif
    uint8_t mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    EthernetServer server = EthernetServer(LISTEN_PORT);
    Relay* relay{};
    Bms* bms{};
    Sensors* sensors{};
    static Request parseRequest(Stream *client);
    static void readAndLogRequestLines(Stream *client);
    void sendResponse(Stream *client, const char* url, int type);
    static bool strEndsWith(const char *source, const char* end);
    static void printIndexPage(Stream *client);
};

#endif //POWER_CONTROLLER_BLE_WEBSERVER_H
