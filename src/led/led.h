#ifndef LED_H_
#define LED_H_

typedef uint8 gpio_dir_t;
typedef uint8 gpio_mode_t;

typedef struct {
    uint32 gpio_pin;
    gpio_dir_t gpio_mode;
    gpio_mode_t gpio_init_state;
} led_config_t;

void ledInit(void);
void updatePattern(uint16 i, bool led_state);
int getLedState(uint16 index, int *state);

#endif /* LED_H_ */
