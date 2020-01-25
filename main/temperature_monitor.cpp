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
#include "local-time.h"
#include "wifi-connection.h"

#include <esp_smartconfig.h>
#include <esp_sntp.h>
#include <sstream>

#define uS_TO_S_FACTOR 1000000    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3.0        /* Time ESP32 will go to sleep (in seconds) */

#define RST_OLED 16               //OLED Reset

#define GPIO_BTN 17
#define GPIO_LED 25

#define I2C_SDA 4
#define I2C_SCL 15

// todo convert to semaphore
auto wifi_event_group = xEventGroupCreate(); // NOLINT(cert-err58-cpp)

Display *display;

TemperatureSensor *displaySensor;
TemperatureSensor *localTemperature;
TemperatureSensor *remoteTemperature;

MqttConnection *mqttConnection;

bool isShowRemoteTemperature = false;

void toggleSensor();

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

    // todo it would be cleaner to use a callback or passthrough the bit flag to avoid potential overlap of flags
    start_wifi(wifi_event_group);

//    esp_pm_config_esp32_t config;
//    config.light_sleep_enable = true;
//    config.max_freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ;
//    config.min_freq_mhz = CONFIG_ESP32_XTAL_FREQ;
//    ESP_ERROR_CHECK(esp_pm_configure(&config));

//    esp_pm_lock_handle_t espPmLockHandle;
//    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "wifi", &espPmLockHandle));
//    ESP_ERROR_CHECK(esp_pm_lock_acquire(espPmLockHandle));

    //Prepare temp sensor
    localTemperature = new LocalTemperature(I2C_SDA, I2C_SCL);
    remoteTemperature = new RemoteTemperature();
    displaySensor = localTemperature;

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

void displayTemperature() {
    float temp = displaySensor->getTemp();

    if (temp == NAN)
        return;

    char str[13];
    snprintf(str, sizeof str, "%3.1fc", temp);

    display->show(str);
}

void submitTemperature(TemperatureSensor *temperatureSensor, const std::string& type) {
    float temp = temperatureSensor->getTemp();

    if (temp == NAN)
        return;

    if (mqttConnection) {
        char *time = getTime();
        std::ostringstream msg;
        msg << R"({ "timestamp": ")" << time
            << R"(", "type": )" << type
            << R"(", "value": )" << temp << R"( })";
        mqttConnection->submit("/topic/temperature", msg.str().c_str());
        delete time;
    }
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

    //todo use queues
    displayTemperature();
    submitTemperature(localTemperature, "local");
    submitTemperature(remoteTemperature, "remote");

    // Note this disconnects bluetooth SPP and Wifi
//    esp_light_sleep_start();
}

void toggleSensor() {
    isShowRemoteTemperature = !isShowRemoteTemperature;

    if (isShowRemoteTemperature) {
        displaySensor = remoteTemperature;
    } else {
        displaySensor = localTemperature;
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
