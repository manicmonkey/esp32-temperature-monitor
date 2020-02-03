#ifndef TEMPERATURE_SENSOR_SNTP_MANAGEMENT_H
#define TEMPERATURE_SENSOR_SNTP_MANAGEMENT_H

#include <freertos/event_groups.h>

#ifdef __cplusplus
extern "C" {
#endif

void start_sntp_task(EventGroupHandle_t eventGroupHandle, uint8_t waitFlag);

#ifdef __cplusplus
}
#endif

#endif //TEMPERATURE_SENSOR_SNTP_MANAGEMENT_H
