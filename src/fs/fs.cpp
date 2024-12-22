#include <LittleFS.h>

#include "fs.h"

void fsInit() {
    if (!LittleFS.begin()) {
        Serial.println("Failed to initialize LittleFS");
        return;
    }
}
