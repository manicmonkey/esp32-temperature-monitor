#include "RemoteTemperature.h"
#include "Arduino.h"

void RemoteTemperature::setup() {
    //Init BLE device
    Serial.println("Remote temperature: setup");
    BLEDevice::init("ESP32");
}

void RemoteTemperature::display(SSD1306 *display) {
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

        //display on screen
        char tempStr[12];
        snprintf(tempStr, sizeof tempStr, "%uc", temp);
        Serial.println("Displaying temp: " + temp);
        display->clear();
        display->drawString(3, 8, String(tempStr).c_str());
        display->display();
    }
    Serial.println();
}