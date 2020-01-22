#ifndef TEMPERATURE_SENSOR_WIFI_CONNECTION_H
#define TEMPERATURE_SENSOR_WIFI_CONNECTION_H

#include <freertos/event_groups.h>

static const uint8_t WIFI_CONNECTED_BIT = BIT0;

void start_wifi(EventGroupHandle_t wifi_event_group);

#endif //TEMPERATURE_SENSOR_WIFI_CONNECTION_H
