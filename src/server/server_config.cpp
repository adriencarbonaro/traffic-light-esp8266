#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#include "macros.h"
#include "server.h"
#include "server_config.h"
#include "led/led.h"
#include "led/led_config.h"
#include "yaml/yaml_config.h"

static void handleRoot();
static void handleSetLedPattern();
static void handleSaveLedPattern();
static void handleLoadLedPattern();

#define HTTP_SEND(code, msg) server.send(code, TEXT_PLAIN, msg)
#define HTTP_200(msg) HTTP_SEND(200, msg)
#define HTTP_400(msg) HTTP_SEND(400, msg)
#define HTTP_500(msg) HTTP_SEND(500, msg)

const route_config_t server_routes_config[] = {
    { "/",                 HTTP_GET,  handleRoot },
    { "/set_led_pattern",  HTTP_POST, handleSetLedPattern },
    { "/save_led_pattern", HTTP_POST, handleSaveLedPattern },
    { "/load_led_pattern", HTTP_GET,  handleLoadLedPattern },
};

uint32 getNumRoutes() {
    return ARRAY_DIM(server_routes_config);
}

static void handleRoot() {
    String html = "<html><body>";
    html += "<h1>LED Pattern Upload</h1>";
    html += "<form action='/set_led_pattern' method='POST' enctype='multipart/form-data'>";
    html += "<input type='file' name='yamlfile' />";
    html += "<input type='submit' value='Upload YAML' />";
    html += "</form>";
    html += "<h2><a href='/save_led_pattern'>Save Current Pattern</a></h2>";
    html += "<h2><a href='/load_led_pattern'>Load Saved Pattern</a></h2>";
    html += "</body></html>";
    HTTP_200(html);
}

static void handleSetLedPattern() {
    if (!server.hasArg(YAML_FILE_ARG)) {
        HTTP_400("No file uploaded");
        return;
    }

    String yamlContent = server.arg(YAML_FILE_ARG);

    // Parse the YAML content (simplified version of YAML parsing)
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, yamlContent);

    if (error) {
        HTTP_400("Failed to parse YAML");
        return;
    }

    // Get LED pattern from YAML and apply it
    JsonArray pattern = doc["ledPattern"].as<JsonArray>();

    for (uint32 i = 0; i < pattern.size(); i++) {
        bool led_state = pattern[i].as<bool>();
        updatePattern(i, led_state ? HIGH : LOW);
    }

    HTTP_200("LED pattern applied successfully");
}

static void handleSaveLedPattern() {
    // Read the current LED state and save it to a file
    DynamicJsonDocument doc(1024);
    JsonArray pattern = doc.createNestedArray("ledPattern");

    for (uint32 i = 0; i < getNumLeds(); i++) {
        int led_state;
        if (!getLedState(i, &led_state)) {
            pattern.add(led_state);
        }
    }

    // Open the file for writing
    File file = LittleFS.open("/led_pattern.json", "w");
    if (!file) {
        HTTP_500("Failed to save LED pattern");
        return;
    }

    // Write the JSON object to the file
    serializeJson(doc, file);
    file.close();

    HTTP_200("LED pattern saved successfully");
}

static void handleLoadLedPattern() {
    // Open the file for reading
    File file = LittleFS.open("/led_pattern.json", "r");
    if (!file) {
        HTTP_500("Failed to load saved LED pattern");
        return;
    }

    // Read the file content into a JSON document
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        HTTP_500("Failed to parse saved LED pattern");
        return;
    }

    // Get LED pattern from the loaded JSON and apply it
    JsonArray pattern = doc["ledPattern"].as<JsonArray>();

    for (uint32 i = 0; i < pattern.size(); i++) {
        bool led_state = pattern[i].as<bool>();
        updatePattern(i, led_state ? HIGH : LOW);
    }

    HTTP_200("LED pattern loaded and applied successfully");
}
