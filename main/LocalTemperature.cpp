#include "Arduino.h"

#include "LocalTemperature.h"

#include "Adafruit_MCP9808.h"
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"` docs - https://github.com/ThingPulse/esp8266-oled-ssd1306

void LocalTemperature::setup(int sda, int scl) {
    Serial.println("Local temperature: setup");
    Wire.begin(sda, scl);
  _tempsensor = Adafruit_MCP9808();
  if (!_tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

void LocalTemperature::display(SSD1306* display) {
    Serial.println("Local temperature: display");
  _tempsensor.wake();   // wake up, ready to read!
  float c = _tempsensor.readTempC();
  Serial.print("Temp: "); Serial.print(c); Serial.println("*C\t"); 
      
  char str[13];
  snprintf(str, sizeof str, "%3.1fc", c);
        
  display->clear();
  display->drawString(3, 8, str);
  display->display();

  _tempsensor.shutdown(); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
}
