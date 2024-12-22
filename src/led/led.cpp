#include <Arduino.h>

#include "led.h"
#include "led_config.h"

void ledInit() {
    for (uint32 i = 0; i < getNumLeds(); i++) {
        led_config_t led_config = leds_config_table[i];
        pinMode(led_config.gpio_pin, led_config.gpio_mode);
        digitalWrite(led_config.gpio_pin, led_config.gpio_init_state);
    }
}

void updatePattern(uint16 i, bool led_state) {

}

int getLedState(uint16 index, int *state) {
    if (index >= getNumLeds()) return -1;
    if (!state) return -2;
    *state = digitalRead(leds_config_table[index].gpio_pin);
    return 0;
}
