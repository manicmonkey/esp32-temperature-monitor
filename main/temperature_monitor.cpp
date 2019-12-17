/**************************************************************************/
/*!
  This is my awesome temperature sensor
  Tested on HelTec WiFi 32 with pinout: https://tinyurl.com/esp32pins

  Useful guide: https://learn.sparkfun.com/tutorials/esp32-thing-hookup-guide
*/
/**************************************************************************/

#include "Arduino.h"
#include "temperature-sensor.h"
#include "local-temperature.h"
#include "remote-temperature.h"
#include "display.h"
#include "wifi-connection.h"
#include "mqtt-connection.h"

#define uS_TO_S_FACTOR 1000000    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  2.0        /* Time ESP32 will go to sleep (in seconds) */

#define RST_OLED 16                     //OLED Reset

#define GPIO_BTN 17
#define GPIO_LED 25

#define I2C_SDA 4
#define I2C_SCL 15

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

Display *display;

TemperatureSensor *temperatureSensor;

//WifiConnection *wifiConnection;
//MqttConnection *mqttConnection;

bool isShowRemoteTemperature = false;

void toggleSensor();

void setup() {
    //Setup serial output
    Serial.begin(115200);
    Serial.println("Startup...");

    //Setup sleep behaviour
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

    //Prepare temp sensor
    temperatureSensor = new LocalTemperature(I2C_SDA, I2C_SCL);

    //todo what are we doing with this instance? seems like it could be static. maybe should have disconnect to switch
//    wifiConnection = new WifiConnection(ssid, password);

    //todo externlise this, maybe even use mDNS to lookup
//    mqttConnection = new MqttConnection("mqtt://192.168.19.166");

    //Prepare button and feedback LED
    pinMode(GPIO_BTN, INPUT_PULLUP);
    pinMode(GPIO_LED, OUTPUT);

    //Setup OLED
    display = new Display(I2C_SDA, I2C_SCL);
    display->show("Hi!");
}

void loop() {
    int btnState = digitalRead(GPIO_BTN);

    if (btnState == LOW) {
        Serial.println("BTN PRESSED");
        digitalWrite(GPIO_LED, HIGH);
        display->show("OK!");

        delay(2000);
        digitalWrite(GPIO_LED, LOW);
        display->show("...");
        toggleSensor();
    }

    float temp = temperatureSensor->getTemp();

    char str[13];
    snprintf(str, sizeof str, "%3.1fc", temp);

    display->show(str);
//    mqttConnection->submit("/topic/qos1", str);

    delay(3000);

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
    printf("Hello world!\n");

    initArduino();
    setup();
    while (true) {
        loop();
    }
}
