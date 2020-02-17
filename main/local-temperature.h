#ifndef LocalTemperature_h
#define LocalTemperature_h

#include "temperature-sensor.h"
#include "Adafruit_MCP9808.h"

class LocalTemperature : public TemperatureSensor {
  public:
    LocalTemperature(int sda, int scl);
    ~LocalTemperature() override;
    void start() override;
    void stop() override;
    float getTemp() override;
  private:
    Adafruit_MCP9808 *_tempsensor;
};

#endif
