#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306

class Display {
  public:
    Display(int sda, int scl);
    ~Display();
    void show(const char* str);
    void show(const char* str, uint16_t len);
    void clear();
  private:
    SSD1306 *_display;
};

#endif
