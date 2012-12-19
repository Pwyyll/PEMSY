#if !defined __USART_H
#define __USART_H

#include <avr/io.h>

volatile uint8_t rx_enabled = 0;

void init();

void send_data(uint8_t datenwort);

ISR(SIG_USART_RECV);

char usart_receive();

#endif
