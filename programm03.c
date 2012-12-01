#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timer.h"
#include "led.h"
#include "pwm.h"
#include "lcd.h"

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
	uint16_t wartezeit = 1000;
	char c1 = 0x48;
	char c2 = 0x41;
	char c3 = 0x4C;
	char c4 = 0x4F; 
	led_toggle(4);
	lcd_put(c1, NULL);
	lcd_put(c2, NULL);
	lcd_put(c3, NULL);
	lcd_put(c3, NULL);
	lcd_put(c4, NULL);
	timer_wait(1000);
	//char wort01 = 'b';
	printf("bläöübb");	
	
	while(1)
	{

	}
	
}


