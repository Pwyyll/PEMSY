#include <avr/io.h>

static volatile uint8_t *const led_port[5] = {&PORTA,&PORTA,&PORTA,&PORTA,&PORTB};
static const uint8_t led_pin[5] = {PA4, PA5, PA6,PA7, PB4};
static volatile uint8_t *const led_reg[5] = {&DDRA,&DDRA,&DDRA,&DDRA,&DDRB};

void led_init(void)
{
	int i = 0;
	for(i = 0;i<=4;i++)
	{
		*led_reg[i] |= (1<<led_pin[i]);
		*led_port[i] &= ~(1<<led_pin[i]);		
	}
	return;	
}

void led_on(uint32_t pos)
{
	*led_port[pos] |= (1<<led_pin[pos]);	
	return;
}

void led_off(uint32_t pos)
{
	*led_port[pos] &= ~(1<<led_pin[pos]);	
	return;
}

void led_toggle(uint32_t pos)
{
	*led_port[pos] ^= (1<<led_pin[pos]);	
	return;
}
