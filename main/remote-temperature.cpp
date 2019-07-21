#include "remote-temperature.h"
#include "Arduino.h"
#include "BLEDevice.h"
#include "BLEScan.h"
#include "BLEBeacon.h"
#include "BLEAddress.h"

static BLEAddress bleAddress("12:3b:6a:1b:67:19");

RemoteTemperature::RemoteTemperature() {
    //Init BLE device
    Serial.println("Remote temperature: setup");
    BLEDevice::init("ESP32");
}

float RemoteTemperature::getTemp() {
    Serial.println("Remote temperature: display");
    BLEScan* scanner = BLEDevice::getScan();
    Serial.println("Starting scan");
    BLEScanResults foundDevices = scanner->start(4);
    Serial.println("Scan complete");
    Serial.println();
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());

    for (int i = 0; i < foundDevices.getCount(); i++) {
        BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);
        Serial.print("Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        if (advertisedDevice.getAddress().toString() != bleAddress.toString()) {
            Serial.println("Device has wrong address");
            continue;
        }

        BLEBeacon myBeacon;
        myBeacon.setData(advertisedDevice.getManufacturerData());

        //temperature is in the upper byte of the beacons minor value
        //this just shifts that byte to the right
        int temp = myBeacon.getMinor() >> 8;
        if (temp == 0) {
            Serial.println("Device has invalid temp, ignoring");
            continue;
        }

        Serial.print("Temp: "); Serial.print(temp); Serial.println("*C\t"); 

        return (float) temp;
    }
    Serial.println();
    return 0.0;
}
