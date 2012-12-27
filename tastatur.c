#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "scancodes.h"
#include "tastatur.h"
#include "led.h"
#include "lcd.h"

volatile uint16_t tastaturdata = 0x0000;
volatile uint8_t empfangszaehler = 0;

volatile uint8_t scancode = 0;
volatile uint8_t pos = 0;
volatile uint8_t parity = 0;
volatile uint8_t scancode_ok = 0;

uint8_t shift_pressed = 0;
uint8_t alt_pressed = 0;
uint8_t ctrl_pressed = 0;

void tastatur_init() {
	//Dateneingang ist PC0
	DDRC &= ~(1<<PC0);
	PORTC |= (1<<PC0);
	//default: Mikrocontroller empfaengt Daten von der Tastatur
	//dazu: Clock und Data auf HIGH, Tastatur generiert selbst Clockpegel
	fdevopen(NULL, keyb_get);
	scancode_ok = 1;
	return;
}

//Interruptroutine der Tastatur
ISR(SIG_INTERRUPT1)
{
/*	uint16_t dummy1 = 0;
	dummy1 = (PINC & 0x0001);
	dummy1 = (dummy1<<empfangszaehler);
	tastaturdata |= dummy1;
	empfangszaehler++;*/
if (scancode_ok != 0) {
	uint8_t databit = PINC & 0x0001;
	if (pos == 0) {	// start bit
		if (databit != 0) {
			return; // start bit error
		}
		scancode = 0;
		parity = 0;
	} else if (pos>0 && pos<9) { // data bits
		parity ^= databit;
		scancode = ( (scancode>>1) | (databit<<7) );
	} else if (pos == 9) { // parity
		parity ^= databit;
	} else if (pos == 10) { // stop bit
		if (parity != 0 && databit != 0) { // no errors
			scancode_ok = 0;			
			DDRC |= (1<<PD3);
		} else { // parity or stop bit error
			scancode = 0;
			pos = 0;
			scancode_ok = 1;
			return;
		}
	}
	pos++;
}

}

uint8_t paritaetsberechnung(uint8_t wort, uint8_t paritybit)
{
	uint8_t paritaet = 0;
    for (int i = 0; i<=7; i++)
        {
            paritaet ^= ((wort>>i) & 0x01);
        }
    paritaet ^= paritybit;
    return paritaet;
}

uint8_t check_word()
{
	sei();

	uint8_t wort =0;
	uint8_t paritybit = 0;
	
	wort = (uint8_t)((tastaturdata>>1) & 0xFF);
	paritybit = ((tastaturdata>>9) & 0x01);	
	uint8_t paritaet = paritaetsberechnung(wort, paritybit);
	//Start-/Stopbitueberpruefung auf Korrektheit
	if ((tastaturdata & 0x0001) != 0 )
	{
		return 1; //startbit
	} else if (((tastaturdata>>10) & 0x0001) != 1)
	{
		return 2; //stopbit
	} else if (paritaet != 1 )
	{
		return 3;//paritaet
	} 
	return 0;
}

uint8_t keyb_get_scancode(void)
{
/*	if (empfangszaehler == 11)
	{	
		uint8_t wort = 0x00;
		uint8_t success = check_word();			
		wort = (uint8_t)((tastaturdata>>1) & 0xFF);
		for(int i = 0; i<=(SCANTABLE_LENGTH/3); i++)
		{
			if (scantable[i].scan == wort)
			{	
				led_toggle(2);
				empfangszaehler = 0;
				tastaturdata = 0;
				lcd_locate(0,0);
				printf("%c", scantable[i].ascii_lwr);
				return scantable[i].ascii_lwr;
			}
		}
	}
	return 0;*/
	pos = 0;
	DDRD &= ~(1<<PD3);
	scancode_ok=2;
	while (scancode_ok != 0) {}
	if (scancode == 0xAA) {
		return keyb_get_scancode();
	}
	return scancode;
}

int keyb_get(FILE *p)
{	
	uint8_t code;
	//uint8_t row = 0;
	//uint8_t col = 0;
	for(;;)
	{
		code = keyb_get_scancode();
		
		switch(code) {
			case 0xE0: // extended make code
				code = keyb_get_scancode();
				switch(code) {
					case 0x14:
						ctrl_pressed = 1;
						break;
					case 0x11:
						ctrl_pressed = 1;
						alt_pressed = 1;
						break;
					case 0xF0: // extended break code
						code = keyb_get_scancode();
						switch(code) {
							case 0x14:
								ctrl_pressed = 0;
								break;
							case 0x11:
								alt_pressed = 0;
								ctrl_pressed = 0;
								break;
						}
						break;
					case 0x5A:
						return scancode2ascii(code);
						break;
				}	
				break;
			case 0xF0: // break code
				code = keyb_get_scancode();
				switch(code) {
					case 0x12:
						shift_pressed = 0;
						break;
					case 0x59:
						shift_pressed = 0;
						break;
					case 0x58:
						shift_pressed ^= 1;
						break;
					case 0x14:
						ctrl_pressed = 0;
						break;
					case 0x11:
						alt_pressed = 0;
						break;
				}
				break;
			case 0x12:
				shift_pressed = 1;
				break;
			case 0x59:
				shift_pressed = 1;
				break;
			case 0x14:
				ctrl_pressed = 1;
				break;
			case 0x11:
				alt_pressed = 1;
				break;
			case 0x76: // ESC
				lcd_clear(); 
				break;
			default:
				if (ctrl_pressed && alt_pressed) {
					switch(code) {
						case 0x3D:
							code = '{';
							break;
						case 0x3E:
							code = '[';
							break;
						case 0x46:
							code = ']';
							break;
						case 0x45:
							code = '}';
							break;
						case 0x4E:
							// no '\'
							break;
						case 0x15:
							code = '@';
							break;
						case 0x3A:
							code = 0xE4; // 'µ'
							break;
						case 0x1A:
							code = 0x7C; // '|'
							// buggy??
							break;
						case 0x5B:
							// no '~'
							break;
						default:
							break;
					}
				} else {
					switch (code) {
						case 0x0E:
							if (shift_pressed) {
								code = 0xDF; // '°'
							} else {
								code = 0x5E; // '^'
							}	
							break;
						case 0x26:
							if (shift_pressed) {
								// no '§'
							} else {
								code = '3';
							}	
							break;
						case 0x4E:
							if (shift_pressed) {
								code = '?'; // '?'
							} else {
								code = 0xE2; // 'ß'
							}	
							break;
						case 0x55:
							if (shift_pressed) {
								code = 0x60; // '`'
							} else {
								code = 0x27; // '´'
							}	
							break;
						case 0x66: // Backspace
							//lcd_get_cursor(row, col);
							//col -= 1;
							//row ^= 1;
							
							//lcd_locate(row, col-1);
							//lcd_write_data(col);
							//code = ' ';
							break;
						case 0x54:
							code = 0xF5; // 'ü'
							break;
						case 0x4C:
							code = 0xEF; // 'ö'
							break;
						case 0x52:
							code = 0xE1; // 'ä'
							break;
						//case 0x5A: // Enter - manual linebreak
							//lcd_get_cursor(row, col);
							//row ^= 1;
							//lcd_locate(row,0);
							//code = ' ';
							break;
						default:
							code = scancode2ascii(code);
							break;
					}
				}
				printf("%c",code);
				//printf("%d",col);
				//putchar(code);
				return code;
		}			
		//code = scancode2ascii(code);	
	}
}

int scancode2ascii(uint8_t code) {
	uint8_t i;
	for(i = 0; i<=(SCANTABLE_LENGTH/3); i++) {
		if (scantable[i].scan == code) {	
			//lcd_locate(0,0);
			//printf("%c", scantable[i].ascii_lwr);
			if (shift_pressed) {
				return scantable[i].ascii_upr;
			} else {
				return scantable[i].ascii_lwr;
			}
		}
	}
	return 0;
}
