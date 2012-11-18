#if !defined __LED_H
#define __LED_H


#include <avr/io.h>

static volatile uint8_t *const led_port[5];
static const uint8_t led_pin[5];
static volatile uint8_t *const led_reg[5];

void led_init(void);

void led_on(uint32_t pos);

void led_off(uint32_t pos);

void led_toggle(uint32_t pos);

#endif
