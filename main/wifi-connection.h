#ifndef TEMPERATURE_SENSOR_WIFI_CONNECTION_H
#define TEMPERATURE_SENSOR_WIFI_CONNECTION_H

class WifiConnection {
public:
    explicit WifiConnection(const char *ssid, const char *password);
    ~WifiConnection();
};

#endif //TEMPERATURE_SENSOR_WIFI_CONNECTION_H
