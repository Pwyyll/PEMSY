#if !defined __LCD_H
#define __LCD_H
#include <stdbool.h>
static volatile uint8_t *const lcd_port[4];
static const uint8_t lcd_pin[4];
static volatile uint8_t *const lcd_reg[4];
volatile const uint8_t wait_long;
volatile const uint8_t wait_short;
//zustand: false fuer lesen, true fuer schreiben
bool zustand;
bool init_mode; // flag for Init des CR mit EINEM Nibble
volatile const uint8_t pwm_standard;

void lcd_init(void);

void lcd_set_contrast(uint8_t contrast);

void lcd_init_write(void);

void lcd_write_data(char data);

void lcd_write_cmd(char cmd);

#endif
