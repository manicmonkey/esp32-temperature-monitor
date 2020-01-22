#include <nvs_flash.h>
#include "esp_event_loop.h"
#include <esp_smartconfig.h>
#include <cstring>
#include <esp_wifi.h>
#include "esp32-hal-log.h"
#include "wifi-connection.h"

/**
 * Callback to process smartconfig events
 * @param status current status
 * @param pdata status specific data
 */
void smartconfig_callback(smartconfig_status_t status, void *pdata) {
    ESP_LOGI(TAG, "Got callback with status: %i", status);
    if (status == SC_STATUS_LINK) {
        auto *config = static_cast<wifi_config_t *>(pdata);
        const char *ssid = reinterpret_cast<char *>(config->sta.ssid);
        ESP_LOGI(TAG, "Got SSID: %s", ssid);
        const char *password = reinterpret_cast<char *>(config->sta.password);
        ESP_LOGI(TAG, "Got password: %s", password);
        ESP_ERROR_CHECK(esp_wifi_disconnect())
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, config))
        ESP_ERROR_CHECK(esp_wifi_connect())
    } else if (status == SC_STATUS_LINK_OVER) {
        ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
        if (pdata != nullptr) {
            uint8_t phone_ip[4] = {0};
            memcpy(phone_ip, (uint8_t *) pdata, 4);
            ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
        }
        ESP_LOGI(TAG, "smartconfig over");
    }
}

/**
 * Background task to perform smartconfig lifecycle management if WiFi not connected within 10 seconds
 */
void smartconfig_task(void *param) {

    EventGroupHandle_t wifi_event_group = param;

    sleep(10);
    EventBits_t eventBits = xEventGroupGetBits(wifi_event_group);
    if (eventBits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi Connected to ap");
        vTaskDelete(nullptr);
        return;
    }

    ESP_LOGI(TAG, "Starting smartconfig");
    ESP_ERROR_CHECK(esp_smartconfig_start(smartconfig_callback, 1))

    do {
        eventBits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, false, portMAX_DELAY);
    } while (!(eventBits & WIFI_CONNECTED_BIT));

    ESP_LOGI(TAG, "WiFi connected to AP");

    esp_smartconfig_stop();
    vTaskDelete(nullptr);
}

// todo not ideal having the 'system' event handler declared here
static esp_err_t system_event_handler(void *ctx, system_event_t *event) {
    EventGroupHandle_t wifi_event_group = ctx;

    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            xTaskCreate(smartconfig_task, "smartconfig_task", 4096, wifi_event_group, 3, nullptr);
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "Got IP!");
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from WiFi...");
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void start_wifi(EventGroupHandle_t wifi_event_group) {
    ESP_ERROR_CHECK(nvs_flash_init())

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(system_event_handler, wifi_event_group))

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    ESP_ERROR_CHECK(esp_wifi_init(&cfg))
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH)) //instead of WIFI_STORAGE_RAM
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA))
    ESP_ERROR_CHECK(esp_wifi_start())
    ESP_ERROR_CHECK(esp_wifi_connect())
}
