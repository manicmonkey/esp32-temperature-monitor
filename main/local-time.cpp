#include <ctime>
#include "esp32-hal-log.h"

char *formatDatetime(time_t *datetime) {
    // Set timezone to Australian Eastern Standard Time
    setenv("TZ", "AEST", 1);
    tzset();

    struct tm timeinfo{};
    localtime_r(datetime, &timeinfo);

    size_t buf_size = 64;
    char *strftime_buf = new char[buf_size];
    strftime(strftime_buf, buf_size, "%FT%T%z", &timeinfo);

    return strftime_buf;
}

void printTime() {
    time_t now;
    time(&now);
    char *time = formatDatetime(&now);
    ESP_LOGI(TAG, "The current date/time in Brisbane is: %s", time);
    delete time;
}