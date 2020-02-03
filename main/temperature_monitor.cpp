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
#include "sntp-management.h"

#include <ctime>
#include <sstream>
#include <freertos/queue.h>
#include <esp_smartconfig.h>
#include <esp_sntp.h>

#define uS_TO_S_FACTOR 1000000    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3.0        /* Time ESP32 will go to sleep (in seconds) */

#define RST_OLED 16               //OLED Reset

#define GPIO_BTN 17
#define GPIO_LED 25

#define I2C_SDA 4
#define I2C_SCL 15

// todo convert to semaphore
auto wifi_event_group = xEventGroupCreate(); // NOLINT(cert-err58-cpp)

typedef struct {
    char type[6];
    time_t time;
    int temp; // temp celsius multiplied by 10
} TemperatureReading;

auto temperatureQueue = xQueueCreate(40, sizeof(TemperatureReading)); // NOLINT(cert-err58-cpp)

Display *display;

TemperatureSensor *displaySensor;
TemperatureSensor *localTemperature;
TemperatureSensor *remoteTemperature;

MqttConnection *mqttConnection;

bool isShowRemoteTemperature = false;

void toggleSensor();

void send_to_mqtt_task(void *param) {
    EventBits_t uxBits;

    do {
        uxBits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, false, portMAX_DELAY);
    } while (!(uxBits & WIFI_CONNECTED_BIT));

    ESP_LOGI(TAG, "WiFi Connected to ap");

    std::string mqttAddress("mqtt://");
    mqttAddress.append(MDNSLookup::lookup("james-xps13"));
    mqttConnection = new MqttConnection(mqttAddress.c_str());

    TemperatureReading reading;
    while (xQueueReceive(temperatureQueue, &reading, portMAX_DELAY)) {

        char *time = formatDatetime(&reading.time);
        std::ostringstream msgBuffer;
        msgBuffer << R"({ "timestamp": ")" << time
                  << R"(", "type": ")" << reading.type
                  << R"(", "value": )" << (reading.temp / 10.0) << R"( })";
        delete time;

        mqttConnection->submit("/topic/temperature", msgBuffer.str().c_str());

        ESP_LOGI(TAG, "Free heap: %i", ESP.getFreeHeap());
    }
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

    start_sntp_task(wifi_event_group, WIFI_CONNECTED_BIT);
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

    if (isnan(temp))
        return;

    char str[13];
    snprintf(str, sizeof str, "%3.1fc", temp);

    display->show(str);
}

void submitTemperature(TemperatureSensor *temperatureSensor, const std::string &type) {
    float tempValue = temperatureSensor->getTemp();

    if (isnan(tempValue))
        return;

    ESP_LOGI(TAG, "Got temp: %f", tempValue);

    time_t tempTime;
    time(&tempTime);

    auto tempRecord = TemperatureReading{};
    strncpy(tempRecord.type, type.c_str(), 6);
    tempRecord.time = tempTime;
    tempRecord.temp = (int) (tempValue * 10);

    xQueueSend(temperatureQueue, &tempRecord, 0 /*don't wait if full*/);
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
