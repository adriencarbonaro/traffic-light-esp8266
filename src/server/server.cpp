#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClient.h>
#include "ArduinoJson.h"
#include "led/led.h"
#include "led/led_config.h"

#include "server.h"
#include "server_config.h"

ESP8266WebServer server(SERVER_PORT);

void serverTask() {
    server.handleClient();
}

void serverInit() {
    for (uint16 i = 0; i < getNumRoutes(); i++) {
        route_config_t config = server_routes_config[i];
        server.on(config.route, config.method, config.function);
    }

    server.begin();
    Serial.println("HTTP server started");
}

void wifiInit() {
    WiFi.begin(SERVER_SSID, SERVER_PASSWD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}
