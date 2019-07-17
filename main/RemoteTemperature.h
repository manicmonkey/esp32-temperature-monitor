#ifndef TEMPERATUREMONITOR_REMOTETEMPERATURE_H
#define TEMPERATUREMONITOR_REMOTETEMPERATURE_H

#include "BLEDevice.h"
#include "BLEScan.h"
#include "BLEBeacon.h"
#include "BLEAddress.h"

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306

//bluetooth
//carkit: 22:22:22:e1:50:9a (class 0x240408)
//phone: 94:65:2d:6f:01:06
//obd2: 00:1d:ae:40:f2:b9 (class 0x1f00)

static BLEAddress bleAddress("12:3b:6a:1b:67:19");

class RemoteTemperature {
public:
    void setup();
    void display(SSD1306* display);
};

#endif //TEMPERATUREMONITOR_REMOTETEMPERATURE_H
