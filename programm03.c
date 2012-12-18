#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timer.h"
#include "led.h"
#include "pwm.h"
#include "lcd.h"
#include "tastatur.h"
#include "usart.h"



void init(void)
{
	led_init();
	timer_init();
	pwm_init();
	lcd_init();
	tastatur_init();
	usart_init();
	return;
}

void interrupt_init(void)
{
	cli();
//PD3/INT1 als Interrupt fuer Tastatur
// mit Clock der Tastatur verbinden
	MCUCR |= (1<<ISC11); //fallende Flanke
	MCUCR &= ~(1<<ISC10);
	GICR |= (1<<INT1);
	DDRD &= ~(1<<PD3); // Eingang
	PORTD |= (1<<PD3); // Pull-up Widerstand auf high

//PD2/INT0 als Interrupt fuer Taster
	MCUCR |= (1<<ISC01); // fallende Flanke
	MCUCR &= ~(1<<ISC00);
	GICR |= (1<<INT0);
	DDRD &= ~(1<<PD2); // Eingang
	PORTD |= (1<<PD2); // Pull-up Widerstand auf high
	
	sei();
	return;
}

//Interruptroutine des Tasters
ISR(SIG_INTERRUPT0)
{
	led_toggle(0);
}

int main(void)
{
init();
interrupt_init();
sei(); //INTERRUPTS EINSCHALTEN NICHT VERGESSEN!!!!!!!!!!!!!!!!!!!!
lcd_locate(0,0);
char empfangswort = 0;
	while(1)
	{
		int wert = keyb_get(NULL);
		empfangswort = usart_receive();
	}

}


