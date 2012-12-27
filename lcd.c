#include <avr/io.h>
#include "pwm.h"
#include <stdio.h>
#include "lcd.h"
#include "timer.h"

static volatile uint8_t *const lcd_port[4] = {&PORTA,&PORTA,&PORTA,&PORTA};
static const uint8_t lcd_pin[4] = {PA3, PA2, PA1,PA0};
static volatile uint8_t *const lcd_reg[4] = {&DDRA,&DDRA,&DDRA,&DDRA};
volatile const uint8_t wait_long = 50;
volatile const uint8_t wait_short = 8;
//zustand: false fuer lesen, true fuer schreiben
uint8_t zustand = 1;
uint8_t init_mode = 1; // flag for Init des CR mit EINEM Nibble
volatile const uint8_t pwm_standard = 20;

void lcd_set_contrast(uint8_t contrast)
{
	set_ocr(contrast);
	return;
}


void lcd_init(void) {
	char init_nibble_1 = 0x03;
	char init_nibble_2 = 0x02;
	char init_byte_1 = 0x28;
	char init_byte_2 = 0x0F;
	char init_clear = 0x01;
	// Wait 50ms for internal initialization
	sei();
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
	// Set 8bit mode 3 times
	for (int i = 0; i<=2; i++) {
		lcd_write_cmd(init_nibble_1);
		timer_wait(wait_short);
	}

	// Set 4bit mode
	lcd_write_cmd(init_nibble_2);

	timer_wait(wait_short);
	init_mode = 0;
	lcd_write_cmd(init_clear);
	// Select Display
	// Write 0x28 into CR
	lcd_write_cmd(init_byte_1);

	timer_wait(wait_short);
	// Activate Display, Cursor, Blinking
	// Write 0x0F into CR
	lcd_write_cmd(init_byte_2);
	fdevopen(lcd_put, NULL);
	timer_wait(wait_short);
	return;
}
char lcd_init_read(uint8_t option) {
	//option = 0 fuer command lesen, option = 1 fuer data lesen
	/*
	Funktion, um Daten vom LCD zu erhalten
	dazu Konfig:
	PB2: HI, -> Register Select: Data Register
	PB1: LO, -> Read/Write auf Write
	PA0-3 (Mikrocontroller): Eingang
	*/
	char worteins = 0x00;
	char wortzwei = 0x00;
	char wortges = 0x00;
	//momentaner zustand an PortB: aus Schutz for ungewollten 
	//Ueberschreibungen
	uint8_t portb_alt = PINB;
	//Konfiguration, ob data oder command lesen
	if (option == 0) {
		PORTB = (portb_alt & 0xFB);//PB2 ist low => command
	} else {
		PORTB = (portb_alt & 0xFF);//PB2 ist high => data
	}
	//PA0-3 auf Eingang
	for (int i = 0; i<=3; i++) {
		*lcd_reg[i] &= ~(1<<lcd_pin[i]);
		*lcd_port[i] |= (1<<lcd_pin[i]);
	}
	// Read/Write
	DDRB &= ~(1<<PB1); // Eingang
	PORTB |= (1<<PB1); //Write
	//################################
	//High Nibble
	//################################
	// Enable auf high, 8ms
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	worteins = PINA;
	// Enable auf low, 8ms
	PORTB &= ~(1<<PB0);
	timer_wait(wait_short);
	//################################
	// Low Nibble
	//################################
	// Enable auf high, 8ms
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	wortzwei = PINA;
	// Enable auf low, 8ms
	PORTB &= ~(1<<PB0);
	timer_wait(wait_short);
	// mergen der nibbles bzw. chars
	wortges = (((worteins & 0x0F)<<4) | (wortzwei & 0x0F));
	zustand == 0;
	return wortges;
}
void lcd_init_write(void) {
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
	for (int i = 0; i<=3; i++) {
		*lcd_reg[i] |= (1<<lcd_pin[i]);
		*lcd_port[i] &= ~(1<<lcd_pin[i]);
	}
	zustand = 1;
	return;
}

void lcd_write_data(char data) {
	/* uint8_t k = 7;*/
	//if (zustand == 0) {
		lcd_init_write();
	//}
	// Set RS to High -- write Data
	PORTB |= (1<<PB2);
	
	char data_low = data & 0x0F;
	char data_high = (data>>4);
	data_high = data_high & 0x0F;
	
	// Set PA0, PA1, PA2, PA3 to high Nibble
	PORTA &= 0xF0;
	PORTA |= data_high;
	// Set Enable to HIGH for 8ms
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	// Set Enable to LOW for 8ms
	PORTB &= ~(1<<PB0);
	timer_wait(wait_short);


	// Set PA0, PA1, PA2, PA3 to low Nibble
	PORTA &= 0xF0;
	PORTA |= data_low;
	// Set Enable to HIGH for 8ms
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	PORTB &= ~(1<<PB0);
	timer_wait(wait_short);

}

void lcd_write_cmd(char cmd) {
	//if (zustand == 0) {
		lcd_init_write();
	//}

	// Set RS to Low -- write Command
	PORTB &= ~(1<<PB2);
	// Read/Write
	DDRB |= (1<<PB1); //Ausgang
	PORTB &= ~(1<<PB1); //Write
	
	char cmd_low = cmd & 0x0F;
	char cmd_high = (cmd>>4);
	cmd_high = cmd_high & 0x0F;
	
	if (init_mode == 0) {
		// Set PA0, PA1, PA2, PA3 to high Nibble
		PORTA &= 0xF0;
		PORTA |= cmd_high;
		// Set Enable to HIGH for 8ms
		PORTB |= (1<<PB0);
		timer_wait(wait_short);
		PORTB &= ~(1<<PB0);
		timer_wait(wait_short);
	}

	// Set PA0, PA1, PA2, PA3 to low Nibble
	PORTA &= 0xF0;
	PORTA |= cmd_low;
	// Set Enable to HIGH for 8ms
	PORTB |= (1<<PB0);
	timer_wait(wait_short);
	PORTB &= ~(1<<PB0);
	timer_wait(wait_short);
}

int lcd_put(char c, FILE *p) {
	char d =0x00;
	
	//uint8_t row = 0;
	//uint8_t col = 0;
	// auto linebreak
	/*lcd_get_cursor(row, col);
	if (col > 0x0F) {
		row ^= 1;
		lcd_locate(row,0);
	}*/

	if (c == '\244') {
		d = 0xE1;
		lcd_write_data(d);
	} else if (c == '\266' ) {
		d = 0xEF;
		lcd_write_data(d);
	} else if (c == '\274') {
		d = 0xF5;
		lcd_write_data(d);
	} else if (c == '\303') {
	} else {
		lcd_write_data(c);
	}
	return 0;
}

void lcd_locate(uint8_t row, uint8_t col) {
	/*
	argument ranges:
	row from 0 to 1
	col from 0 to 15
	*/
	char ddram_high = 0x08; // DB7 always 1
	char ddram_low = 0x00;
	char ddram = 0x00;

	// Set DDRAM Address
	// Set row
	if (row == 0) {
		ddram_high = 0x08;
	} else if (row == 1) {
		ddram_high = 0x0C;
	}
	// Set column
	ddram_low = col;
	// Merge nibbles
	ddram = (((ddram_high << 4) & 0xF0) | (ddram_low & 0x0F));

	// Write cmd
	lcd_write_cmd(ddram);
}

void lcd_clear(void) {
	char ddram = 0x01;
	lcd_write_cmd(ddram);
}


void lcd_get_cursor(uint8_t row, uint8_t col) {
	char cursor_loc = lcd_init_read(0);

	row = (cursor_loc >> 4)	& 0x07;
	col = cursor_loc & 0x0F;
	//printf("%x", cursor_loc);
	//printf("%d", row);
	//printf("%d", col);
}
