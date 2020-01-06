#include "wifi-connection.h"
#include <WiFi.h>

WifiConnection::WifiConnection(const char *ssid, const char *password) {
//    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));//instead of WIFI_STORAGE_RAM

    ESP_LOGI(TAG, "SSID: %s", ssid);
    ESP_LOGI(TAG, "Password: %s", password);

    WiFi.begin(ssid, password);

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(500);
//        ESP_LOGI(TAG, ".");
    }


    ESP_LOGI(TAG, "WiFi connected");
    ESP_LOGI(TAG, "IP address: %s", WiFi.localIP().toString().c_str());
}
