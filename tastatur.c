#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "scancodes.h"
#include "tastatur.h"

volatile uint16_t tastaturdata = 0x0000;
volatile uint8_t empfangszaehler = 0;

void tastatur_init() {
	//Dateneingang ist PC0
	DDRC &= ~(1<<PC0);
	PORTC |= (1<<PC0);
	//default: Mikrocontroller empfaengt Daten von der Tastatur
	//dazu: Clock und Data auf HIGH, Tastatur generiert selbst Clockpegel
	fdevopen(NULL, keyb_get);

	return;
}

//Interruptroutine der Tastatur
ISR(SIG_INTERRUPT1)
{
	uint16_t dummy1 = 0;
	dummy1 = (PINC & 0x0001);
	dummy1 = (dummy1<<empfangszaehler);
	tastaturdata |= dummy1;
	empfangszaehler++;
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



int keyb_get(FILE *p)
{	

	if (empfangszaehler == 11)
	{	
		uint8_t wort = 0x00;
		uint8_t success = check_word();			
		wort = (uint8_t)((tastaturdata>>1) & 0xFF);
		for(int i = 0; i<=(SCANTABLE_LENGTH/3); i++)
		{
			if (scantable[i].scan == wort)
			{
				empfangszaehler = 0;
				tastaturdata = 0;
				//printf("%c", scantable[i].ascii_lwr);
				return scantable[i].ascii_lwr;
			}
		}
	
	}
	return 0;
}

