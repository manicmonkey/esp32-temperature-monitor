SET (COMPONENT_REQUIRES
        "arduino-esp32"
        "adafruit-mcp9808"
        "thingpulse-ssd1306"
        "mqtt"
        )

set(COMPONENT_SRCS
        "temperature_monitor.cpp"
        "local-temperature.h"
        "local-temperature.cpp"
        "remote-temperature.h"
        "remote-temperature.cpp"
        "mqtt-connection.h"
        "mqtt-connection.cpp"
        "display.h"
        "display.cpp"
        "font.h"
        "wifi-connection.h"
        "wifi-connection.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
