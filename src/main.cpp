#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WiFiClient.h>

// Replace with your network credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Define the pins for LEDs
const int ledPins[] = {5, 4, 0, 2};  // GPIO5, GPIO4, GPIO0, GPIO2
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

ESP8266WebServer server(80);

static void handleRoot();
static void handleSetLedPattern();
static void handleSaveLedPattern();
static void handleLoadLedPattern();

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize LED pins
    for (int i = 0; i < numLeds; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);  // Turn off all LEDs initially
    }

    // Initialize LittleFS file system
    if (!LittleFS.begin()) {
        Serial.println("Failed to initialize LittleFS");
        return;
    }

    // Define HTTP server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/set_led_pattern", HTTP_POST, handleSetLedPattern);
    server.on("/save_led_pattern", HTTP_POST, handleSaveLedPattern);
    server.on("/load_led_pattern", HTTP_GET, handleLoadLedPattern);

    // Start the server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    // Handle client requests
    server.handleClient();
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
    server.send(200, "text/html", html);
}

static void handleSetLedPattern() {
    if (!server.hasArg("yamlfile")) {
        server.send(400, "text/plain", "No file uploaded");
        return;
    }

    String yamlContent = server.arg("yamlfile");

    // Parse the YAML content (simplified version of YAML parsing)
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, yamlContent);
    if (error) {
        server.send(400, "text/plain", "Failed to parse YAML");
        return;
    }

    // Get LED pattern from YAML and apply it
    JsonArray pattern = doc["ledPattern"].as<JsonArray>();

    for (int i = 0; i < numLeds; i++) {
        if (i < pattern.size()) {
        bool ledState = pattern[i].as<bool>();
        digitalWrite(ledPins[i], ledState ? HIGH : LOW);
        }
    }

    server.send(200, "text/plain", "LED pattern applied successfully");
}

static void handleSaveLedPattern() {
    // Read the current LED state and save it to a file
    DynamicJsonDocument doc(1024);
    JsonArray pattern = doc.createNestedArray("ledPattern");

    for (int i = 0; i < numLeds; i++) {
        bool ledState = digitalRead(ledPins[i]);
        pattern.add(ledState);
    }

    // Open the file for writing
    File file = LittleFS.open("/led_pattern.json", "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to save LED pattern");
        return;
    }

    // Write the JSON object to the file
    serializeJson(doc, file);
    file.close();

    server.send(200, "text/plain", "LED pattern saved successfully");
}

static void handleLoadLedPattern() {
    // Open the file for reading
    File file = LittleFS.open("/led_pattern.json", "r");
    if (!file) {
        server.send(500, "text/plain", "Failed to load saved LED pattern");
        return;
    }

    // Read the file content into a JSON document
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        server.send(500, "text/plain", "Failed to parse saved LED pattern");
        return;
    }

    // Get LED pattern from the loaded JSON and apply it
    JsonArray pattern = doc["ledPattern"].as<JsonArray>();

    for (int i = 0; i < numLeds; i++) {
        if (i < pattern.size()) {
        bool ledState = pattern[i].as<bool>();
        digitalWrite(ledPins[i], ledState ? HIGH : LOW);
        }
    }

    server.send(200, "text/plain", "LED pattern loaded and applied successfully");
}
