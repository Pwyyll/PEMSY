#include <avr/io.h>

volatile uint8_t empfang_usart = 0;

volatile uint8_t rx_enabled_usart = 0; //flag: 0 fuer empfangen, 1 fuer senden (vgl. uint8_t zustand in lcd.c)

//Initialisierung des USART (allgemein)
void usart_init(uint16_t baud)
{
	DDRD |= (1<<PD1);	//AUsgang
	PORTD &= ~(1<<PD1);

	DDRD &= ~(1<<PD0);	//EIngang
	PORTD |= (1<<PD0);


	UCSRB &= ~(1<<URSEL);	//URSEL = 0: zum Beschreiben von UBRR
	UBRRH = (0x00>>8);
	UBRRL = 103;	//Baudrate von 9600 bei f_Clock = 4MHz, U2X = 0 (kein double-speed)
	UCSRB = (1<<RXEN) | (1<<TXEN);
	//UCSRC &= ~((1<<UPM1) | (1<<UPM0));	//UPM1:0 = 0 0 : keine Paritaet
	UBRRH |= (1<<URSEL);	//URSEL = 1, um UCSRC beschreiben zu können
	UCSRC |= (1<<UCSZ0);	// 8-Bit Datenmodus
	UCSRC |= (1<<UCSZ1);
	//UCSRC &= ~(1<<USBS);	//USBS = 0: kein Stopbit
	//UCSRC &= ~(1<<UMSEL);	//UMSEL = 0: asynchron
	UCSRC |= (1<<URSEL);	

	//Interrupts enablen: RX, TX, UDR
//	UCSRB = (1<<RXCIE) | (1<<TXCIE) | (1<<UDRIE);
	UCSRB = (1<<TXCIE);
	

}

void send_data(unsigned char datenwort)
{
//	unsigned char data = 'A';
	// nur Daten ins Register schreiben, falls UDR flag gesetzt ist!
	while (!(UCSRA & (1<<UDRE)))
	{
		;	//warteschleife, bis Flag gesetzt wurde
	}
	UDR = 'A';
//	printf("GESENDET");
//	lcd_clear();
}

ISR(SIG_USART_RECV)
{
	empfang_usart = 1;
}

char usart_receive()
{
	if (empfang_usart ==1)
	{
		char tempwort = 0;
		if (tempwort != '\r')
		{
			return tempwort;
		} else if (tempwort == '\r')
		{
			return ('\n');
		}
	}
	return;
}

/*
Muss fuer neues Senden TXEN wieder gesetzt werden, falls zwischendurch Daten empfangen wurden?
Also so wie bei der lcd.c?
Oder muss man alles einmal enablen, und dann werden Daten empfangen/gesendet, je nachdem,
was im Datenregister steht?
falls immer wieder neu konfiguriert werden muss: zwei neue Funktionen:

void send_init()
{
UCSRB |= (1<<RXEN);
rx_enabled = 1;
}

void receive_init()
{
UCSRB |= (1<<TXEN);
rx_enabled = 0;
}

*/
