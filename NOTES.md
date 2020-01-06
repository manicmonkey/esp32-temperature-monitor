# Development Notes

### Logging

If `Ardunio.h` is included, logging is implemented by `esp32-hal-log.h` rather than `esp_log.h`.
 It has a nicer log formatter which is colourised, and includes the class and line number.
 On the downside, it means that `esp_log_level_set` has no effect.

If `Arduino.h` is not included, then the same logging can be utilised by including `esp32-hal-log.h` directly.