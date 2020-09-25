#include "ProtectionStatus.h"

ProtectionStatus::ProtectionStatus(){
    singleCellOvervoltageProtection    = false;
    singleCellUndervoltageProtection   = false;
    wholePackOvervoltageProtection     = false;
    wholePackUndervoltageProtection    = false;
    chargingOverTemperatureProtection  = false;
    chargingLowTemperatureProtection   = false;
    dischargeOverTemperatureProtection = false;
    dischargeLowTemperatureProtection  = false;
    chargingOvercurrentProtection      = false;
    dischargeOvercurrentProtection     = false;
    shortCircuitProtection             = false;
    frontEndDetectionIcError           = false;
    softwareLockMos                    = false;
    singleCellOvervoltageProtectionCount = 0;
    singleCellUndervoltageProtectionCount = 0;
    wholePackOvervoltageProtectionCount = 0;
    wholePackUndervoltageProtectionCount = 0;
    chargingOverTemperatureProtectionCount = 0;
    chargingLowTemperatureProtectionCount = 0;
    dischargeOverTemperatureProtectionCount = 0;
    dischargeLowTemperatureProtectionCount = 0;
    chargingOvercurrentProtectionCount = 0;
    dischargeOvercurrentProtectionCount = 0;
    shortCircuitProtectionCount = 0;
    frontEndDetectionIcErrorCount = 0;
    softwareLockMosCount = 0;
}

ProtectionStatus::ProtectionStatus(uint16_t status) : ProtectionStatus (){
    singleCellOvervoltageProtection    = status & 0b0000000000000001u;
    singleCellUndervoltageProtection   = status & 0b0000000000000010u;
    wholePackOvervoltageProtection     = status & 0b0000000000000100u;
    wholePackUndervoltageProtection    = status & 0b0000000000001000u;
    chargingOverTemperatureProtection  = status & 0b0000000000010000u;
    chargingLowTemperatureProtection   = status & 0b0000000000100000u;
    dischargeOverTemperatureProtection = status & 0b0000000001000000u;
    dischargeLowTemperatureProtection  = status & 0b0000000010000000u;
    chargingOvercurrentProtection      = status & 0b0000000100000000u;
    dischargeOvercurrentProtection     = status & 0b0000001000000000u;
    shortCircuitProtection             = status & 0b0000010000000000u;
    frontEndDetectionIcError           = status & 0b0000100000000000u;
    softwareLockMos                    = status & 0b0001000000000000u;
}

void ProtectionStatus::updateProtectionStatus(uint16_t status){
    if (!singleCellOvervoltageProtection &&    status & 0b0000000000000001u) { singleCellOvervoltageProtectionCount++; }
    if (!singleCellUndervoltageProtection &&   status & 0b0000000000000010u) { singleCellUndervoltageProtectionCount++; }
    if (!wholePackOvervoltageProtection &&     status & 0b0000000000000100u) { wholePackOvervoltageProtectionCount++; }
    if (!wholePackUndervoltageProtection &&    status & 0b0000000000001000u) { wholePackUndervoltageProtectionCount++; }
    if (!chargingOverTemperatureProtection &&  status & 0b0000000000010000u) { chargingOverTemperatureProtectionCount++; }
    if (!chargingLowTemperatureProtection &&   status & 0b0000000000100000u) { chargingLowTemperatureProtectionCount++; }
    if (!dischargeOverTemperatureProtection && status & 0b0000000001000000u) { dischargeOverTemperatureProtectionCount++; }
    if (!dischargeLowTemperatureProtection &&  status & 0b0000000010000000u) { dischargeLowTemperatureProtectionCount++; }
    if (!chargingOvercurrentProtection &&      status & 0b0000000100000000u) { chargingOvercurrentProtectionCount++; }
    if (!dischargeOvercurrentProtection &&     status & 0b0000001000000000u) { dischargeOvercurrentProtectionCount++; }
    if (!shortCircuitProtection &&             status & 0b0000010000000000u) { shortCircuitProtectionCount++; }
    if (!frontEndDetectionIcError &&           status & 0b0000100000000000u) { frontEndDetectionIcErrorCount++; }
    if (!softwareLockMos &&                    status & 0b0001000000000000u) { softwareLockMosCount++; }
    singleCellOvervoltageProtection    = status & 0b0000000000000001u;
    singleCellUndervoltageProtection   = status & 0b0000000000000010u;
    wholePackOvervoltageProtection     = status & 0b0000000000000100u;
    wholePackUndervoltageProtection    = status & 0b0000000000001000u;
    chargingOverTemperatureProtection  = status & 0b0000000000010000u;
    chargingLowTemperatureProtection   = status & 0b0000000000100000u;
    dischargeOverTemperatureProtection = status & 0b0000000001000000u;
    dischargeLowTemperatureProtection  = status & 0b0000000010000000u;
    chargingOvercurrentProtection      = status & 0b0000000100000000u;
    dischargeOvercurrentProtection     = status & 0b0000001000000000u;
    shortCircuitProtection             = status & 0b0000010000000000u;
    frontEndDetectionIcError           = status & 0b0000100000000000u;
    softwareLockMos                    = status & 0b0001000000000000u;
}

void ProtectionStatus::clearFaultCounts(){
    singleCellOvervoltageProtectionCount = 0;
    singleCellUndervoltageProtectionCount = 0;
    wholePackOvervoltageProtectionCount = 0;
    wholePackUndervoltageProtectionCount = 0;
    chargingOverTemperatureProtectionCount = 0;
    chargingLowTemperatureProtectionCount = 0;
    dischargeOverTemperatureProtectionCount = 0;
    dischargeLowTemperatureProtectionCount = 0;
    chargingOvercurrentProtectionCount = 0;
    dischargeOvercurrentProtectionCount = 0;
    shortCircuitProtectionCount = 0;
    frontEndDetectionIcErrorCount = 0;
    softwareLockMosCount = 0;
}

void ProtectionStatus::printFaultCounts(Stream *client) const{
    client->println(R"===("faults": [)===");
    char buffer[64] = {0};
    sprintf(buffer,R"===({"fault": "Single Cell Over-Voltage", "count": %d},)===", singleCellOvervoltageProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Single Cell Under-Voltage", "count": %d},)===", singleCellUndervoltageProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Whole Pack Over-Voltage", "count": %d},)===", wholePackOvervoltageProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Whole Pack Under-Voltage", "count": %d},)===", wholePackUndervoltageProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Charging Over Temperature", "count": %d},)===", chargingOverTemperatureProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Charging Low Temperature", "count": %d},)===", chargingLowTemperatureProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Discharge Over Temperature", "count": %d},)===", dischargeOverTemperatureProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Discharge Low Temperature", "count": %d},)===", dischargeLowTemperatureProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Charging Over-Current", "count": %d},)===", chargingOvercurrentProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Discharge Over-Current", "count": %d},)===", dischargeOvercurrentProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Short Circuit", "count": %d},)===", shortCircuitProtectionCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Front End Detection Ic Error", "count": %d},)===", frontEndDetectionIcErrorCount);
    client->println(buffer);
    sprintf(buffer,R"===({"fault": "Software Lock Mos", "count": %d})===", softwareLockMosCount);
    client->println(buffer);
    client->println(R"===(],)===");
}
