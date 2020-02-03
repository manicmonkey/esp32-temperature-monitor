#include <esp_sntp.h>
#include <esp32-hal-log.h>
#include "sntp-management.h"

typedef struct {
    EventGroupHandle_t eventGroupHandle;
    uint8_t waitFlag;
} NtpTaskParams;

void ntp_task(void *param) {
    NtpTaskParams *params = (NtpTaskParams *) param;

    EventBits_t uxBits;

    do {
        uxBits = xEventGroupWaitBits(params->eventGroupHandle, params->waitFlag, false, false, portMAX_DELAY);
    } while (!(uxBits & params->waitFlag));

    if (!sntp_enabled()) {
        ESP_LOGI(TAG, "Setting up NTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        char server[] = "pool.ntp.org";
        sntp_setservername(0, server);
        sntp_init();
    }

    // todo don't send mqtt until time sync'd (MQTT_READY == WIFI_CONNECTED & TIME_SYNCED)
    // sntp_set_time_sync_notification_cb

    vTaskDelete(NULL);
}

void start_sntp_task(EventGroupHandle_t eventGroupHandle, uint8_t waitFlag) {
    NtpTaskParams *ntpTaskParams = pvPortMalloc(sizeof(NtpTaskParams));
    ntpTaskParams->eventGroupHandle = eventGroupHandle;
    ntpTaskParams->waitFlag = waitFlag;
    xTaskCreate(ntp_task, "ntp_task", 4096, (void *) ntpTaskParams, 3, NULL);
}