#include "local-temperature.h"
#include "Adafruit_MCP9808.h"

static const char *TAG = "LocalTemperature";

LocalTemperature::LocalTemperature(int sda, int scl) {
  // Initialise wire library so we can communicate with the temperature sensor
  Wire.begin(sda, scl);
  _tempsensor = new Adafruit_MCP9808();
  if (!_tempsensor->begin()) {
    ESP_LOGE(TAG, "Couldn't find MCP9808!");
    while (1);
  }
}

LocalTemperature::~LocalTemperature() {
  _tempsensor->shutdown();
  delete _tempsensor;
  _tempsensor = nullptr;
}

float LocalTemperature::getTemp() {
  _tempsensor->wake();   // wake up, ready to read!
  float temp = _tempsensor->readTempC();
  _tempsensor->shutdown(); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
  ESP_LOGI(TAG, "temp=%3.1fc", temp);
  return temp;
}
