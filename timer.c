#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t overflow_count = 0;

ISR(SIG_OVERFLOW2){
	overflow_count++;
}

void timer_init(void)
{
	// External Timer Oscillator
	ASSR |= (1<<AS2);
	// Enable Overflow Interrupt
	TIMSK |= (1<<TOIE2);
	// Setting Timer Counter to 0
	TCNT2 &= ~(0xFF);
	// No Prescaling & Start timer
	TCCR2 |= (1<<CS20);
}

uint32_t timer_get(void)
{
	// Umrechnung der TCNT2 in ms Werte
	uint32_t time_now_ms = overflow_count*(125>>4);
	return time_now_ms;
}

void timer_wait(uint32_t delay)
{
	uint32_t t_start = timer_get();
	uint32_t t_stop = 0;
	do {
		t_stop = timer_get();
	} while ((t_stop - t_start) < delay);
}
