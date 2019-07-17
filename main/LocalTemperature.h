#ifndef TemperatureDisplay_h
#define TemperatureDisplay_h

#include "Arduino.h"
#include "Adafruit_MCP9808.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306

class LocalTemperature {
  public:
    void setup(int sda, int scl);
    void display(SSD1306* display);
  private:
    Adafruit_MCP9808 _tempsensor;
};

#endif
