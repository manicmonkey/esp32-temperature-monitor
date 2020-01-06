#ifndef MqttConnection_h
#define MqttConnection_h

#include "mqtt_client.h"
#include "esp32-hal-log.h"

class MqttConnection {
public:
    explicit MqttConnection(const char *uri);
    ~MqttConnection();
    void submit(const char *topic, const char *data);
private:
    esp_mqtt_client_handle_t client;
    static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
};

#endif
