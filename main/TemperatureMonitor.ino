/**************************************************************************/
/*!
This is my awesome temperature sensor
Tested on HelTec WiFi 32 with pinout: https://tinyurl.com/esp32pins

//useful guide https://learn.sparkfun.com/tutorials/esp32-thing-hookup-guide
*/
/**************************************************************************/

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "font.h"    // font builder (change first char to uint8_t) http://oleddisplay.squix.ch/

#include "LocalTemperature.h"
#include "RemoteTemperature.h"
#include "Display.h"

//#include "BluetoothSerial.h"

#define uS_TO_S_FACTOR 1000000    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  0.1        /* Time ESP32 will go to sleep (in seconds) */

#define RST_OLED 16                     //OLED Reset
#define OLED_UPDATE_INTERVAL 500        //OLED

#define GPIO_BTN 13
#define GPIO_LED 25

#define I2C_SDA 4
#define I2C_SCL 15

//#define BLE_TEMP_MONITOR "abtemp_6719"

//SSD1306 display(0x3C, I2C_SDA, I2C_SCL);

Display* display;

//BluetoothSerial SerialBT;

LocalTemperature localTemperature;
RemoteTemperature remoteTemperature;

void setup() {
  //Setup serial output
  Serial.begin(115200);
  Serial.println("Startup...");

  //SerialBT.begin("ESP32");

  //Special setup routine required for OLED
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);        // turn D16 low to reset OLED
  delay(50);
  digitalWrite(RST_OLED, HIGH);       // while OLED is running, must set D16 in high

  //Setup sleep behaviour
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  //Prepare temp sensors
  localTemperature.setup(I2C_SDA, I2C_SCL);
  remoteTemperature.setup();

  //Prepare button and feedback LED
  pinMode(GPIO_BTN, INPUT_PULLUP);
  pinMode(GPIO_LED, OUTPUT);

  //Setup OLED for output
//  display.init();
//  display.flipScreenVertically();
//  display.setFont(Dialog_plain_44);
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
    Display _display(I2C_SDA, I2C_SCL);
  display = &_display;

  display->setup();


    display->show("Hi!");
  //Say hello!
//  display.clear();
//  display.drawString(3, 8, "HI!");
//  display.display();
}

bool isShowRemoteTemperature = false;

void loop() {

    int btnState = digitalRead(GPIO_BTN);

  if (btnState == LOW) {
    Serial.println("BTN PRESSED");
    digitalWrite(GPIO_LED, HIGH);
    isShowRemoteTemperature = !isShowRemoteTemperature;
    display->show("OK!");
//    display.clear();
//    display.drawString(3, 8, "OK!");
//    display.display();
    delay(2000);
    digitalWrite(GPIO_LED, LOW);
    display->show("...");
//    display.clear();
//    display.drawString(3, 8, "..."); //todo need to change font size
//    display.display();
  }
  
  if (isShowRemoteTemperature) {
//    remoteTemperature.display(&display);
    display->show("Remote");
  } else {
      display->show("Local");
//      localTemperature.display(&display);
  }

  esp_light_sleep_start();
}
