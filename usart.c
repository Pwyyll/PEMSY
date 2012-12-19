#include <avr/io.h>

volatile uint8_t rx_enabled = 0; //flag: 0 fuer empfangen, 1 fuer senden (vgl. uint8_t zustand in lcd.c)
//Initialisierung des USART (allgemein)
void usart_init()
{
//Interrupts enablen: RX, TX, UDR
UCSRB = (1<<RXCIE) | (1<<TXCIE) (1<<UDRIE);
//URSEL = 1, um UCSRC beschreiben zu können
UCSRC |= (1<<URSEL);
//UMSEL = 0: asynchron
UCSRC &= ~(1<<UMSEL);
//UPM1:0 = 0 0 : keine Paritaet
UCSRC &= ~((1<<UPM1) | (1<<UPM0));
//USBS = 0: kein Stopbit
UCSRC &= ~(1<<USBS);
//UCSZ2:0 = 0 0 0: 8-Bit Datenmodus
UCSRB = ~(1<<UCSZ2);
UCSRC = ((1<<UCSZ1) | (UCSZ0));
//URSEL = 0: zum Beschreiben von UBRR
UCSRC &= ~(1<<URSEL);
UBRR = 25; //Baudrate von 9600 bei f_Clock = 4MHz, U2X = 0 (kein double-speed)
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
//Aufruf zum enablen des Empfangens
UCSRB |= (1<<TXEN);
//Aufruf zum enablen des Sendens
UCSRB |= (1<<RXEN);

}

void send_data(uint8_t datenwort)
{
// nur Daten ins Register schreiben, falls UDR flag gesetzt ist!
while ((UCSRA & UDRE) != 1)
{
//warteschleife, bis Flag gesetzt wurde
}
UDR = datenwort;
}

ISR(SIG_USART_RECV)
{
	empfang = 1;
}

char usart_receive() 
{
	if (empfang ==1)
	{
		char tempwort = 0;
		if (tempwort != '\r'
		{
			return tempwort;
		} else if (tempwort == '\r')
			return ('\n');
		}
	}
	return;
}