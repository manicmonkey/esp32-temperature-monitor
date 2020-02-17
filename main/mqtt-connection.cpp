#include "mqtt-connection.h"

//unsigned portBASE_TYPE uxQueueLength = 10;
//unsigned portBASE_TYPE uxItemSize = sizeof(float);

//QueueHandle_t queue;
//QueueHandle_t queue = xQueueCreate(uxQueueLength, uxItemSize);

/*void blahdeblah() {
  float fl = 100.0f;
  xQueueSend(queue, &fl, 100);
}*/

//static const char *TAG = "MqttConnection";

MqttConnection::MqttConnection(const char *uri) {
    ESP_LOGI(TAG, "Creating connection to uri: '%s'", uri);

    auto *mqtt_cfg = new esp_mqtt_client_config_t();
    mqtt_cfg->uri = uri;
    mqtt_cfg->event_handle = MqttConnection::mqtt_event_handler;

    client = esp_mqtt_client_init(mqtt_cfg);
}

MqttConnection::~MqttConnection() {
    ESP_LOGI(TAG, "Destroy");
    esp_mqtt_client_destroy(client);
}

void MqttConnection::start() {
    esp_mqtt_client_start(client);
}

void MqttConnection::stop() {
    esp_mqtt_client_stop(client);
}

void MqttConnection::submit(const char *topic, const char *data) {
    ESP_LOGI(TAG, "Publishing value '%s' to topic '%s'", data, topic);
    if (connected)
        esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

bool MqttConnection::connected = false;

esp_err_t MqttConnection::mqtt_event_handler(esp_mqtt_event_handle_t event) {
//    esp_mqtt_client_handle_t client = event->client;
//    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
//            msg_id = esp_mqtt_client_subscribe(client, "/topic/toEsp", 0);
//            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            connected = true;
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
//            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGD(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}
