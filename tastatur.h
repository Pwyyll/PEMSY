#if !defined __TASTATUR_H
#define __TASTATUR_H

#include <avr/io.h>

void tastatur_init(void);

ISR(SIG_INTERRUPT1);

uint8_t paritaetsberechnung(uint8_t wort, uint8_t paritybit);

int keyb_get(FILE *p);

uint8_t check_word();

#endif
