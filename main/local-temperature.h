#ifndef TemperatureDisplay_h
#define TemperatureDisplay_h

#include "Arduino.h"
#include "Adafruit_MCP9808.h"
#include "temperature-sensor.h"

class LocalTemperature : public TemperatureSensor {
  public:
    LocalTemperature(int sda, int scl);
    ~LocalTemperature() override;
    float getTemp() override;
  private:
    Adafruit_MCP9808 *_tempsensor;
};

#endif
