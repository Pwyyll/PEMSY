#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "led.h"
#include "pwm.h"
#include "lcd.h"

void init(void)
{
	led_init();
	timer_init();
	pwm_init();
	return;
}

int main(void)
{
	init();
	uint8_t contrast = 0;
	uint32_t wartezeit = 1000;
	sei();
	while(1)
	{
		set_ocr(contrast);
		led_on(3);
		contrast++;
//		led_toggle(1);
		timer_wait(wartezeit);
		led_off(3);
		timer_wait(wartezeit);
		led_toggle(0);
	}
}
