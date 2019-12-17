#ifndef TEMPERATUREMONITOR_REMOTETEMPERATURE_H
#define TEMPERATUREMONITOR_REMOTETEMPERATURE_H

#include "temperature-sensor.h"

//#define BLE_TEMP_MONITOR "abtemp_6719"

//bluetooth
//carkit: 22:22:22:e1:50:9a (class 0x240408)
//phone: 94:65:2d:6f:01:06
//obd2: 00:1d:ae:40:f2:b9 (class 0x1f00)

class RemoteTemperature : public TemperatureSensor {
public:
    RemoteTemperature();
    float getTemp() override;
};

#endif //TEMPERATUREMONITOR_REMOTETEMPERATURE_H
