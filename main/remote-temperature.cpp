#include "remote-temperature.h"
#include <esp32-hal-log.h>
#include "BLEDevice.h"
#include "BLEScan.h"
#include "BLEBeacon.h"
#include "BLEAddress.h"

static const char *TAG = "RemoteTemperature";

static BLEAddress bleAddress("12:3b:6a:1b:67:19");

RemoteTemperature::RemoteTemperature() {
    //Init BLE device
    ESP_LOGI(TAG, "Remote temperature: setup");
    // Disable classic BT and free ram
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    BLEDevice::init("ESP32");
}

float RemoteTemperature::getTemp() {
    BLEScan* scanner = BLEDevice::getScan();
    ESP_LOGI(TAG, "Starting scan");
    BLEScanResults foundDevices = scanner->start(4);
    ESP_LOGI(TAG, "Devices found: %d", foundDevices.getCount());

    for (int i = 0; i < foundDevices.getCount(); i++) {
        BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);
        ESP_LOGD(TAG, "Device found: %s", advertisedDevice.toString().c_str());

        if (advertisedDevice.getAddress().toString() != bleAddress.toString()) {
            ESP_LOGD(TAG, "Device has wrong address");
            continue;
        }

        const std::string &data = advertisedDevice.getManufacturerData();

        if (data.length() != 25) { // length as per BLEBeacon#setData
            ESP_LOGD(TAG, "Did not get correct payload size (was %i)", data.length());
            continue;
        }

        BLEBeacon myBeacon;
        myBeacon.setData(data);

        //temperature is in the upper byte of the beacons minor value
        //this just shifts that byte to the right
        int temp = myBeacon.getMinor() >> 8;
        if (temp == 0) {
            ESP_LOGD(TAG, "Device has invalid temp, ignoring");
            continue;
        }

        ESP_LOGI(TAG, "Temp: %ic", temp);

        return (float) temp;
    }

    return 0.0;
}
