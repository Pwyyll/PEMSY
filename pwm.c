//Fuer Zaehler 0
#include <avr/io.h>
void pwm_init(void) 
{
	//Phasenkorrekte PWM
	DDRB |= (1<<PB3);
	TCCR0 |= (1<<WGM00);
	TCCR0 &= ~(1<<WGM01);
	//Nicht-invertierende PWM
	TCCR0 |= (1<<COM01);
	TCCR0 &= ~(1<<COM00);	
	//kein Prescaling
	TCCR0 &= ~(1<<CS00);	
	TCCR0 &= ~(1<<CS01);
	TCCR0 |= (1<<CS02);
	TCNT0 &= ~(0xFF);
	return;
}

void set_ocr(uint8_t wert)
{
	OCR0 = wert;
	return;
}
