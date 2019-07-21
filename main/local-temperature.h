#ifndef TemperatureDisplay_h
#define TemperatureDisplay_h

#include "Arduino.h"

class LocalTemperature {
  public:
    LocalTemperature(int sda, int scl);
    ~LocalTemperature();
    float getTemp();
  private:
    Adafruit_MCP9808 *_tempsensor;
};

#endif
