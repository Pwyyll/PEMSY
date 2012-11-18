#if !defined __PWM_H
#define __PWM_H

#include <avr/io.h>

void pwm_init(void);

void set_ocr(uint8_t wert);

#endif
