#ifndef POWER_CONTROLLER_BLE_PROTECTIONSTATUS_H
#define POWER_CONTROLLER_BLE_PROTECTIONSTATUS_H

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <MockArduino.h>
#endif

class ProtectionStatus {
public:
    ProtectionStatus();

    explicit ProtectionStatus(uint16_t status);

    void updateProtectionStatus(uint16_t status);

    void clearFaultCounts();

    void printFaultCounts(Stream *client) const;

#ifndef UNIT_TEST
private:
#endif
    bool singleCellOvervoltageProtection;
    bool singleCellUndervoltageProtection;
    bool wholePackOvervoltageProtection;
    bool wholePackUndervoltageProtection;
    bool chargingOverTemperatureProtection;
    bool chargingLowTemperatureProtection;
    bool dischargeOverTemperatureProtection;
    bool dischargeLowTemperatureProtection;
    bool chargingOvercurrentProtection;
    bool dischargeOvercurrentProtection;
    bool shortCircuitProtection;
    bool frontEndDetectionIcError;
    bool softwareLockMos;
    uint8_t singleCellOvervoltageProtectionCount;
    uint8_t singleCellUndervoltageProtectionCount;
    uint8_t wholePackOvervoltageProtectionCount;
    uint8_t wholePackUndervoltageProtectionCount;
    uint8_t chargingOverTemperatureProtectionCount;
    uint8_t chargingLowTemperatureProtectionCount;
    uint8_t dischargeOverTemperatureProtectionCount;
    uint8_t dischargeLowTemperatureProtectionCount;
    uint8_t chargingOvercurrentProtectionCount;
    uint8_t dischargeOvercurrentProtectionCount;
    uint8_t shortCircuitProtectionCount;
    uint8_t frontEndDetectionIcErrorCount;
    uint8_t softwareLockMosCount;
};
#endif //POWER_CONTROLLER_BLE_PROTECTIONSTATUS_H
