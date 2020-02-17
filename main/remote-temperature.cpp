#include "remote-temperature.h"
#include <esp32-hal-log.h>
#include <cmath>
#include <esp_bt_main.h>
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

//    esp_err_t errRc;
//
//    esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
//    if (bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
//        errRc = esp_bluedroid_init();
//        if (errRc != ESP_OK) {
//            ESP_LOGE(TAG, "esp_bluedroid_init: rc=%d %s", errRc);
//            return;
//        }
//    }
//
//    if (bt_state != ESP_BLUEDROID_STATUS_ENABLED) {
//        errRc = esp_bluedroid_enable();
//        if (errRc != ESP_OK) {
//            ESP_LOGE(TAG, "esp_bluedroid_enable: rc=%d %s", errRc);
//            return;
//        }
//    }
//
//    errRc = esp_ble_gap_register_callback(BLEDevice::gapEventHandler); //todo define handler
//    if (errRc != ESP_OK) {
//        ESP_LOGE(TAG, "esp_ble_gap_register_callback: rc=%d %s", errRc);
//        return;
//    }
//
//    errRc = esp_ble_gattc_register_callback(BLEDevice::gattClientEventHandler);
//    if (errRc != ESP_OK) {
//        ESP_LOGE(TAG, "esp_ble_gattc_register_callback: rc=%d", errRc);
//        return;
//    }
//
//
//    errRc = ::esp_ble_gap_set_device_name(deviceName.c_str());
//    if (errRc != ESP_OK) {
//        ESP_LOGE(TAG, "esp_ble_gap_set_device_name: rc=%d", errRc);
//        return;
//    }
//
//    vTaskDelay(200 / portTICK_PERIOD_MS); // Delay for 200 msecs as a workaround to an apparent Arduino environment issue.

}

void RemoteTemperature::start() {
    BLEDevice::init("ESP32");
}

void RemoteTemperature::stop() {
    BLEDevice::deinit();
}

float RemoteTemperature::getTemp() {

//    esp_err_t errRc = ::esp_ble_gap_set_scan_params(&m_scan_params);
//
//    if (errRc != ESP_OK) {
//        log_e("esp_ble_gap_set_scan_params: err: %d", errRc);
//        return false;
//    }
//
//    errRc = ::esp_ble_gap_start_scanning(duration);
//
//    if (errRc != ESP_OK) {
//        ESP_LOGE(TAG, "esp_ble_gap_start_scanning: err: %d", errRc);
//        return false;
//    }
//

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

    return NAN;
}
