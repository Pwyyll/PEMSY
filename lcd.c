#include <avr/io.h>
#include "pwm.h"
#include <stdbool.h>
#include "lcd.h"
#include "timer.h"
#include "led.h"


static volatile uint8_t *const lcd_port[4] = {&PORTA,&PORTA,&PORTA,&PORTA};
static const uint8_t lcd_pin[4] = {PA3, PA2, PA1,PA0};
static volatile uint8_t *const lcd_reg[4] = {&DDRA,&DDRA,&DDRA,&DDRA};
volatile const uint8_t wait_long = 50;
volatile const uint8_t wait_short = 8;
//zustand: false fuer lesen, true fuer schreiben
bool zustand = true;
bool init_mode = true; // flag for Init des CR mit EINEM Nibble
volatile const uint8_t pwm_standard = 40;


void lcd_set_contrast(uint8_t contrast)
{

	set_ocr(contrast);
	return;
}


void lcd_init(void)
{
	char init_nibble_1 = 0x03;
	char init_nibble_2 = 0x02;
	char init_byte_1 = 0x28;
	char init_byte_2 = 0x0F;
	
	// Wait 50ms for internal initialization
	timer_wait(wait_long);

	//##################################################################
	//Mikrocontroller-Initialisierung
	// Register Select: immer Ausgang, Pin1
	// HIGH: Datenbyte(Data Register), LOW: Befehlsbyte (Command Register)
	DDRB |= (1<<PB2);
	// Enable: immer Ausgang, DDRB, Pin0
	// wechselt zwischen HI und LO	
	DDRB |= (1<<PB0);	
	lcd_set_contrast(pwm_standard);
	//##################################################################
	//default: write
	lcd_init_write();
	led_on(0);
	// Set 8bit mode 3 times
	for (int i = 0; i<=2; i++)
	{
		lcd_write_cmd(init_nibble_1);
		timer_wait(wait_short);
	}	
	// Set 4bit mode
	lcd_write_cmd(init_nibble_2);
	led_on(1);
	timer_wait(wait_short);
	init_mode = false;
	// Select Display
	// Write 0x28 into CR
	lcd_write_cmd(init_byte_1);
	led_on(2);
	timer_wait(wait_short);
	// Activate Display, Cursor, Blinking
	// Write 0x0F into CR
	lcd_write_cmd(init_byte_2);
	led_on(3);
	timer_wait(wait_short);
	return;
}

void lcd_init_write(void)
{
	/*
	Funktion, um Daten an LCD zu senden
	dazu Konfig:
	PB2: HI, -> Register Select: Data Register
	PB1: LO, -> Read/Write auf Write
	PA0-3: Ausgang
	*/
	// Register Select auf HI -- default write data
	PORTB |= (1<<PB2);
	// Read/Write
	DDRB |= (1<<PB1); //Ausgang
	PORTB &= ~(1<<PB1); //Write
	//PA0-3 auf Ausgang
	for (int i = 0; i<=3; i++)
	{
		*lcd_reg[i] |= (1<<lcd_pin[i]);
		*lcd_port[i] &= ~(1<<lcd_pin[i]);	
	}
	zustand = true;
	return;	
}

void lcd_write_data(char data)
{
	/* uint8_t k = 7;*/
	if (zustand == false)
	{
		lcd_init_write();
	}
	// Set RS to High -- write Data
	PORTB |= (1<<PB2);
	// Set PA0, PA1, PA2, PA3 to high Nibble
	
	// Set Enable to HIGH for 8ms	
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	PORTB &= ~(1<<PB0);	
	timer_wait(wait_short);
	// Set PA0, PA1, PA2, PA3 to low Nibble
	
	// Set Enable to HIGH for 8ms	
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	PORTB &= ~(1<<PB0);	
	timer_wait(wait_short);

}

void lcd_write_cmd(char cmd)
{
	if (zustand == false)
	{
		lcd_init_write();
	}
	// Set RS to Low -- write Data
	PORTB &= ~(1<<PB2);
	char cmd_high = (cmd>>4);
	cmd_high = cmd_high & 0x0F;
	char cmd_low = cmd & 0x0F;

	if (init_mode == false)
	{
		// Set PA0, PA1, PA2, PA3 to high Nibble
		PORTA |= cmd_high;
		// Set Enable to HIGH for 8ms	
		PORTB |= (1<<PB0);
		timer_wait(wait_short);
		PORTB &= ~(1<<PB0);	
		timer_wait(wait_short);
	}
	
	// Set PA0, PA1, PA2, PA3 to low Nibble
	PORTA |= cmd_low;
	// Set Enable to HIGH for 8ms	
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	PORTB &= ~(1<<PB0);	
	timer_wait(wait_short);
	
}




	/*
	for(int j = 0;j<=1;j++)
	{
		for(int i = 0;i<=3; i++)
		{
			if(in_datastream[k] == '0')
			{
				PORTB &= ~(1<<lcd_pin[i]);
			//	led_on(0);
			}else if(in_datastream[k] == '1')
			{
				PORTB |= (1<<lcd_pin[i]);
			//	led_on(1);
			}else
			{
				led_on(4);
			}
			k--;		
		}
	
	}
	*/
		/*// Set RW to Output and High -- Output / Write
	DDRB |= (1<<PB1);
	PORTB &= ~(1<<PB1);
	//PA0, PA1, PA2, PA3 auf Ausgang
	for (int i = 0; i<=3; i++)
	{
		*lcd_reg[i] |= (1<<lcd_pin[i]);
		*lcd_port[i] &= ~(1<<lcd_pin[i]);	
	}*/
	
		/*// Set RW to Output and High -- Output / Write
	DDRB |= (1<<PB1);
	PORTB &= ~(1<<PB1);
	//PA0, PA1, PA2, PA3 auf Ausgang
	for (int i = 0; i<=3; i++)
	{
		*lcd_reg[i] |= (1<<lcd_pin[i]);
		*lcd_port[i] &= ~(1<<lcd_pin[i]);	
	}*/
	
