/**************************************************************************/
/*!
  This is my awesome temperature sensor
  Tested on HelTec WiFi 32 with pinout: https://tinyurl.com/esp32pins

  Useful guide: https://learn.sparkfun.com/tutorials/esp32-thing-hookup-guide
*/
/**************************************************************************/

#include "display.h"
#include "temperature-sensor.h"
#include "local-temperature.h"
#include "remote-temperature.h"
#include "mdns-lookup.h"
#include "misc.h"
#include "mqtt-connection.h"

//#include <esp_pm.h>
#include <mdns.h>
#include <esp_smartconfig.h>
#include <esp_wifi.h>
//#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include <nvs_flash.h>
#include <esp_sntp.h>
#include <sstream>

#define uS_TO_S_FACTOR 1000000    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3.0        /* Time ESP32 will go to sleep (in seconds) */

#define RST_OLED 16               //OLED Reset

#define GPIO_BTN 17
#define GPIO_LED 25

#define I2C_SDA 4
#define I2C_SCL 15

static EventGroupHandle_t wifi_event_group;

static const uint8_t WIFI_CONNECTED_BIT = BIT0;
static const uint8_t ESPTOUCH_DONE_BIT = BIT1;

Display *display;

TemperatureSensor *temperatureSensor;

MqttConnection *mqttConnection;

bool isShowRemoteTemperature = false;

void toggleSensor();

void smartconfig_task(void *param);

char *getTime() {
    time_t now;
    time(&now);

    // Set timezone to Australian Eastern Standard Time
    setenv("TZ", "AEST", 1);
    tzset();

    struct tm timeinfo{};
    localtime_r(&now, &timeinfo);

    size_t buf_size = 64;
    char *strftime_buf = new char[buf_size];
    strftime(strftime_buf, buf_size, "%FT%T%z", &timeinfo);

    return strftime_buf;
}

void printTime() {
    char *time = getTime();
    ESP_LOGI(TAG, "The current date/time in Brisbane is: %s", time);
    delete time;
}

static esp_err_t system_event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            xTaskCreate(smartconfig_task, "smartconfig_task", 4096, nullptr, 3, nullptr);
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
        xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

/**
 * Background task to perform smartconfig lifecycle management if WiFi not connected within 10 seconds
 */
void smartconfig_task(void *param) {

    sleep(10);
    EventBits_t eventBits = xEventGroupGetBits(wifi_event_group);
    if (eventBits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi Connected to ap");
        vTaskDelete(nullptr);
        return;
    }

    ESP_LOGI(TAG, "Starting smartconfig");
    ESP_ERROR_CHECK(esp_smartconfig_start(smartconfig_callback, 1))

    EventBits_t connected = WIFI_CONNECTED_BIT | ESPTOUCH_DONE_BIT;

    do {
        eventBits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | ESPTOUCH_DONE_BIT, false, false, portMAX_DELAY);
    } while (!(eventBits & connected));

    if (eventBits & WIFI_CONNECTED_BIT)
        ESP_LOGI(TAG, "WiFi Connected to ap");
    if (eventBits & ESPTOUCH_DONE_BIT)
        ESP_LOGI(TAG, "smartconfig over");

    esp_smartconfig_stop();
    vTaskDelete(nullptr);
}

void start_wifi() {
    ESP_ERROR_CHECK(nvs_flash_init())

    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();

    //    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_loop_init(system_event_handler, nullptr))

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    ESP_ERROR_CHECK(esp_wifi_init(&cfg))
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH)) //instead of WIFI_STORAGE_RAM
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA))
    ESP_ERROR_CHECK(esp_wifi_start())
    ESP_ERROR_CHECK(esp_wifi_connect())
}

void init_ntp_task(void *param) {
    EventBits_t uxBits;

    do {
        uxBits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, false, portMAX_DELAY);
    } while (!(uxBits & WIFI_CONNECTED_BIT));

    if (!sntp_enabled()) {
        ESP_LOGI(TAG, "Setting up NTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        char server[] = "pool.ntp.org";
        sntp_setservername(0, server);
        sntp_init();
    }
    
    // todo don't send mqtt until time sync'd (MQTT_READY == WIFI_CONNECTED & TIME_SYNCED)
    // sntp_set_time_sync_notification_cb

    vTaskDelete(nullptr);
}

void send_to_mqtt_task(void *param) {
    EventBits_t uxBits;

    do {
        uxBits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, false, portMAX_DELAY);
    } while (!(uxBits & WIFI_CONNECTED_BIT));

    ESP_LOGI(TAG, "WiFi Connected to ap");

    std::string mqttAddress("mqtt://");
    mqttAddress.append(MDNSLookup::lookup("james-xps13"));
    mqttConnection = new MqttConnection(mqttAddress.c_str());

    vTaskDelete(nullptr);
}

void setup() {
    //Setup serial output
    Serial.begin(115200);
    ESP_LOGI(TAG, "Startup...");

    printTime();

    //Setup sleep behaviour
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    ESP_LOGI(TAG, "Setup ESP32 to sleep for every %d Seconds", TIME_TO_SLEEP);

    ESP_LOGI(TAG, "Free heap: %i", ESP.getFreeHeap());

    // todo a linux command line tool for smart-config would be handy...
    start_wifi();

//    esp_pm_config_esp32_t config;
//    config.light_sleep_enable = true;
//    config.max_freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ;
//    config.min_freq_mhz = CONFIG_ESP32_XTAL_FREQ;
//    ESP_ERROR_CHECK(esp_pm_configure(&config));

//    esp_pm_lock_handle_t espPmLockHandle;
//    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "wifi", &espPmLockHandle));
//    ESP_ERROR_CHECK(esp_pm_lock_acquire(espPmLockHandle));

    //Prepare temp sensor
    temperatureSensor = new LocalTemperature(I2C_SDA, I2C_SCL);

    xTaskCreate(init_ntp_task, "init_ntp_task", 4096, nullptr, 3, nullptr);
    xTaskCreate(send_to_mqtt_task, "send_to_mqtt_task", 4096, nullptr, 3, nullptr);

    //Prepare button and feedback LED
    pinMode(GPIO_BTN, INPUT_PULLUP);
    pinMode(GPIO_LED, OUTPUT);

    //Setup OLED
    display = new Display(I2C_SDA, I2C_SCL);
    display->show("Hi!");

    printHeapSpace();
}

void loop() {
    ESP_LOGI(TAG, "Free heap: %i", ESP.getFreeHeap());

    int btnState = digitalRead(GPIO_BTN);

    if (btnState == LOW) {
        ESP_LOGI(TAG, "BTN PRESSED");
        digitalWrite(GPIO_LED, HIGH);
        display->show("OK!");

        delay(2000);
        digitalWrite(GPIO_LED, LOW);
        display->show("...");
        toggleSensor();
    }

    float temp = temperatureSensor->getTemp();

    if (temp == NAN)
        return;

    char str[13];
    snprintf(str, sizeof str, "%3.1fc", temp);

    display->show(str);

    if (mqttConnection) {
        char *time = getTime();
        std::ostringstream msg;
        msg << R"({ "timestamp": ")" << time << R"(", "value": )" << temp << R"( })";
        mqttConnection->submit("/topic/temperature", msg.str().c_str());
        delete time;
    }

    // Note this disconnects bluetooth SPP and Wifi
//    esp_light_sleep_start();
}

void toggleSensor() {
    delete temperatureSensor;

    isShowRemoteTemperature = !isShowRemoteTemperature;

    if (isShowRemoteTemperature) {
        temperatureSensor = new RemoteTemperature();
    } else {
        temperatureSensor = new LocalTemperature(I2C_SDA, I2C_SCL);
    }
}

// ESP-IDF entrypoint - chain into arduino code
extern "C" void app_main() {
    initArduino();
    setup();
    while (true) {
        loop();
        delay(2000);
    }
}
