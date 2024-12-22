#include <Arduino.h>

#include "macros.h"
#include "led_config.h"

const led_config_t leds_config_table[] = {
    { 5, OUTPUT, LOW },
    { 4, OUTPUT, LOW },
    { 0, OUTPUT, LOW },
    { 2, OUTPUT, LOW },
};

uint32 getNumLeds(void) {
   return ARRAY_DIM(leds_config_table);
}
