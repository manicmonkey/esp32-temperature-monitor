#include "display.h"

#include "Arduino.h"

#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "font.h"    // font builder (change first char to uint8_t) http://oleddisplay.squix.ch/

#define RST_OLED 16                     //OLED Reset
#define OLED_UPDATE_INTERVAL 500        //OLED

Display::Display(int sda, int scl) {
  //Special setup routine required for OLED
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);        // turn D16 low to reset OLED
  delay(50);
  digitalWrite(RST_OLED, HIGH);       // while OLED is running, must set D16 in high

  _display = new SSD1306(0x3C, sda, scl);
  _display->init();
  _display->flipScreenVertically();
  _display->setFont(Roboto_32);
// todo center align
  _display->setTextAlignment(TEXT_ALIGN_LEFT);
}

Display::~Display() {
  delete _display;
}

void Display::show(const char* str) {
    ESP_LOGI(TAG, "str=%s", str);
    _display->clear();
//    _display->drawString(10, 14, str);
    uint16_t sWidth = _display->getStringWidth(str, strlen(str));
    int16_t offset = 128 - sWidth;
    _display->drawString(offset / 2, 18, str);
    _display->display();
}

void Display::clear() {
  ESP_LOGD(TAG, "Clear");
  _display->clear();
}