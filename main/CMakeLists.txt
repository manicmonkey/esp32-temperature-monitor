SET (COMPONENT_REQUIRES
        "arduino-esp32"
        "adafruit-mcp9808"
        "thingpulse-ssd1306"
        "mqtt"
        "mdns"
        "nvs_flash"
        )

set(COMPONENT_SRCS
        "display.h"
        "display.cpp"
        "font.h"
        "local-temperature.h"
        "local-temperature.cpp"
        "local-time.h"
        "local-time.cpp"
        "mdns-lookup.h"
        "mdns-lookup.cpp"
        "misc.h"
        "misc.cpp"
        "mqtt-connection.h"
        "mqtt-connection.cpp"
        "remote-temperature.h"
        "remote-temperature.cpp"
        "temperature_monitor.cpp"
        "temperature-sensor.h"
        "wifi-connection.h"
        "wifi-connection.cpp")

set(COMPONENT_ADD_INCLUDEDIRS ".")

set(IDF_COMPILE_DEFINITIONS WIFI_SSID="$ENV{WIFI_SSID}" WIFI_PASSWORD="$ENV{WIFI_PASSWORD}")

register_component()
