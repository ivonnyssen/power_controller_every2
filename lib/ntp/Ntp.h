
#ifndef POWER_CONTROLLER_BLE_NTP_H
#define POWER_CONTROLLER_BLE_NTP_H
#if defined(ARDUINO)
#include <Ethernet.h>
#include <TimeLib.h>

#define NTP_OFFSET   3155673600
#define UNIX_OFFSET   946684800
#define LOCAL_NTP_PORT 8888
#define NTP_PACKET_SIZE 48
#define NTP_SERVER "time.nist.gov"

void sendPacket(const char * address);
void ntpBegin(int timeZone);
time_t getTime();

#endif //Arduino

#endif //POWER_CONTROLLER_BLE_NTP_H
