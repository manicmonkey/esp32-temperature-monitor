#include "misc.h"
#include "esp32-hal-log.h"
#include "Esp.h"

void printHeapSpace() {
    ESP_LOGI(TAG, "Free heap: %i", ESP.getFreeHeap());
}
