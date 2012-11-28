#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "led.h"
#include "pwm.h"
#include "lcd.h"
#include <stdbool.h>
void init(void)
{
	led_init();
	timer_init();
	pwm_init();
	lcd_init();
	return;
}

int main(void)
{
	init();
	sei(); //INTERRUPTS EINSCHALTEN NICHT VERGESSEN!!!!!!!!!!!!!!!!!!!!
	//uint8_t contrast = 0;
	//uint32_t wartezeit = 1000;
	while(1)
	{

	}
}

