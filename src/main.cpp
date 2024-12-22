#include <ArduinoJson.h>
#include <LittleFS.h>

#include "server/server.h"
#include "led/led.h"
#include "fs/fs.h"

void setup() {
    Serial.begin(115200);
    wifiInit();
    fsInit();
    ledInit();
    serverInit();
}

void loop() {
    serverTask();
}
