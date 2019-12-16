#include "mqtt-connection.h"

//unsigned portBASE_TYPE uxQueueLength = 10;
//unsigned portBASE_TYPE uxItemSize = sizeof(float);

//QueueHandle_t queue;
//QueueHandle_t queue = xQueueCreate(uxQueueLength, uxItemSize);

/*void blahdeblah() {
  float fl = 100.0f;
  xQueueSend(queue, &fl, 100);
}*/

MqttConnection::MqttConnection(const char *uri) {
    Serial.println("Init");

    auto *mqtt_cfg = new esp_mqtt_client_config_t();
    mqtt_cfg->uri = uri;
    mqtt_cfg->event_handle = mqtt_event_handler;

    client = esp_mqtt_client_init(mqtt_cfg);
    esp_mqtt_client_start(client);
}

MqttConnection::~MqttConnection() {
    Serial.println("De-init");
    esp_mqtt_client_destroy(client);
}

void MqttConnection::submit(const char *topic, const char *data) {
    Serial.println("Submit");
    esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}
