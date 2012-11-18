#if !defined __TIMER_H
#define __TIMER_H


#include <avr/io.h>
#include <avr/interrupt.h>

static uint32_t overflow_count;

void timer_init(void);

uint32_t timer_get(void);

void timer_wait(uint32_t delay);

#endif
