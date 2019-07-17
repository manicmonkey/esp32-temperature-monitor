#include "Arduino.h"

#include "Display.h"

#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "font.h"    // font builder (change first char to uint8_t) http://oleddisplay.squix.ch/

#define RST_OLED 16                     //OLED Reset
#define OLED_UPDATE_INTERVAL 500        //OLED

//SSD1306 _display;

Display::Display(int sda, int scl) {
    SSD1306 display(0x3C, sda, scl);
    _display = &display;
}

//void Display::setup(int sda, int scl) {
void Display::setup() {
  // display setup
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);        // turn D16 low to reset OLED
  delay(50);
  digitalWrite(RST_OLED, HIGH);       // while OLED is running, must set D16 in high

//  _display = &SSD1306(0x3C, 4, 15);
  _display->init();
  _display->flipScreenVertically();
  _display->setFont(Dialog_plain_44);
  //display.setFont(ArialMT_Plain_24);
  _display->setTextAlignment(TEXT_ALIGN_LEFT);

  // Initialise wire library with OLED I2C pins or the display stops working when we access the temp sensor
//  Wire.begin(sda, scl);
}

void Display::show(const char* str) {
//    _display->clear();
    _display->drawString(3, 8, str);
    _display->display();
}

void Display::clear() {
  _display->clear();
//  _display.display();
}

