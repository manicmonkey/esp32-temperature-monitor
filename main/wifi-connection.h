#ifndef TEMPERATURE_SENSOR_WIFI_CONNECTION_H
#define TEMPERATURE_SENSOR_WIFI_CONNECTION_H

#include <freertos/event_groups.h>

static const uint8_t WIFI_CONNECTED_BIT = BIT0;

void init_wifi(EventGroupHandle_t wifi_event_group);
void start_wifi();
void stop_wifi();

#endif //TEMPERATURE_SENSOR_WIFI_CONNECTION_H
