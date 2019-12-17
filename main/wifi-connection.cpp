#include <WiFi.h>
#include "wifi-connection.h"

WifiConnection::WifiConnection(const char *ssid, const char *password) {
    Serial.print("SSID: "); Serial.println(ssid);
    Serial.print("Password: "); Serial.println(password);

    WiFi.begin(ssid, password);

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
