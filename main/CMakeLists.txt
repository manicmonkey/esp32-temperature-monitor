SET (COMPONENT_REQUIRES
        "arduino-esp32"
        "adafruit-mcp9808"
        "thingpulse-ssd1306"
        "mqtt"
        "mdns"
        "nvs_flash"
        )

set(COMPONENT_SRCS
        "temperature_monitor.cpp"
        "temperature-sensor.h"
        "local-temperature.h"
        "local-temperature.cpp"
        "remote-temperature.h"
        "remote-temperature.cpp"
        "mdns-lookup.h"
        "mdns-lookup.cpp"
        "misc.h"
        "misc.cpp"
        "mqtt-connection.h"
        "mqtt-connection.cpp"
        "local-time.h"
        "local-time.cpp"
        "display.h"
        "display.cpp"
        "font.h"
        "wifi-connection.h"
        "wifi-connection.cpp")

set(COMPONENT_ADD_INCLUDEDIRS ".")

set(IDF_COMPILE_DEFINITIONS WIFI_SSID="$ENV{WIFI_SSID}" WIFI_PASSWORD="$ENV{WIFI_PASSWORD}")

register_component()
