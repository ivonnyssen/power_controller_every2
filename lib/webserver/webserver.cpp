#include "webserver.h"
#include "indexPage.h"

#if defined(ARDUINO)
#else
#include <MockSD.h>
#include <cstring>
#include <cctype>
#endif

void WebServer::handleHttpRequest() {
    EthernetClient client = server.available();
    if (client.available()) {
        Request request = parseRequest(&client);
        switch (request.type) {
            case Request::GET:
                sendResponse(&client, request.url, Request::GET);
                break;
            case Request::POST:
                switch(request.command){
                    case OFF:
                        relay->togglePort(request.powerPort, false);
                        break;
                    case ON:
                        relay->togglePort(request.powerPort, true);
                        break;
                    case CYCLE:
                        relay->togglePort(request.powerPort, false);
                        delay(1000);
                        relay->togglePort(request.powerPort, true);
                        break;
                    default:
                        break;
                }
                sendResponse(&client, "/", Request::POST);
            default:
                break;
        }
    }
    delay(10);
    client.stop();
}

Request WebServer::parseRequest(Stream *client) {
    Request result;

    char s[1024] {0};
    client->readBytesUntil('\n', &s[0], sizeof(s));
#if DEBUG_WEB_SERVER
    if(Serial) Serial.println(s);
#endif
    if(strncmp(s,"GET", 3) == 0){
        result.type = Request::GET;
        strncpy(result.url, &s[4], strrchr(s, ' ') - s - 4);
        readAndLogRequestLines(client);
    } else if(strncmp(&s[0],"POST", 4) == 0){
        result.type = Request::POST;
        readAndLogRequestLines(client);
        if(client->available()){
            memset(&s[0], 0, sizeof(s));
            client->readBytesUntil('\n', &s[0], sizeof(s));
            if(strncmp(s, "power",5) == 0){
                if(isdigit(s[5])){
                    result.powerPort = s[5] - '0';
                }
                if(isdigit(s[7])) {
                    result.command = s[7] - '0';
                }
            }
        }
    } else {
        result.type = Request::UNSUPPORTED;
    }
#if DEBUG_WEB_SERVER
    if(Serial) Serial.println(result.url);
#endif
    return result;
}

bool WebServer::strEndsWith(const char *source, const char *end) {
    size_t sourceLength = strlen(source);
    size_t endLength = strlen(end);
    return (sourceLength >= endLength && strcmp(source + sourceLength - endLength, end) == 0);
}

void WebServer::sendResponse(Stream *client, const char* url, int type) {
    //print header
    if (type == Request::POST) {
        client->println("HTTP/1.1 303 See Other");
        char buffer[64] = {0};
        sprintf(buffer, "Location: http://%d.%d.%d.%d%s",EthernetClass::localIP()[0],EthernetClass::localIP()[1],
                EthernetClass::localIP()[2],EthernetClass::localIP()[3],url);
        client->println(buffer);
    } else {
        client->println("HTTP/1.1 200 OK");
        if(strcmp(url, "/") == 0){
            client->println("Content-Type: text/html");
            char buffer[64] = {0};
            sprintf(buffer, "Refresh: 450; url=http://%d.%d.%d.%d%s",EthernetClass::localIP()[0],
                    EthernetClass::localIP()[1],EthernetClass::localIP()[2],EthernetClass::localIP()[3],url);
            client->println(buffer);
        }
    }
    if(strEndsWith(url, ".json")) { client->println("Content-Type: application/json"); }
    client->println("Connection: close");
    client->println();

    if(strcmp(url, "/") == 0) {
        printIndexPage(client);
    } else if(strcmp(url, "/sensors.json") == 0){
        sensorsPrintJson(client);
    } else if(strcmp(url, "/battery.json") == 0){
        bms->printCellVoltages(client);
        bms->printFaults(client);
        bms->printStates(client);
    } else if(strcmp(url, "/switches.json") == 0) {
        relay->printJSON(client);
    }
}

void WebServer::printIndexPage(Stream *client) {
    for (char* line : page){
        client->write(line);
    }
}

void WebServer::readAndLogRequestLines(Stream *client) {
    char s[265]{0};

    while (client->available()) {
        memset(&s[0], 0, sizeof(s));
        client->readBytesUntil('\n', &s[0], sizeof(s));
#if DEBUG_WEB_SERVER
        if(Serial) Serial.println(s);
#endif
        if(s[0] == '\r'){
            break;
        }
    }
}

WebServer::WebServer(Relay *aRelayPtr, Bms *aBmsPtr) {
    this->relay = aRelayPtr;
    this->bms = aBmsPtr;
}

void WebServer::begin() {
    EthernetClass::begin(mac);
    if(!EthernetClass::begin(mac)){
        if(Serial) Serial.println("Ethernet/DHCP failed");
    }
    server.begin();
#if DEBUG_WEB_SERVER
    if(Serial) Serial.println(EthernetClass::localIP());
#endif
}

