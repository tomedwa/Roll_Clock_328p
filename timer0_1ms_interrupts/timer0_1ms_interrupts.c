/*
 **************************************************************
 * timer0_1ms_interrupts.h
 * Author: Tom
 * Date: 18/01/2024
 * Use timer0 on the Atmega328p to generate an interrupt
 * approximately every 1ms.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * timer0_init() - Initialise timer0 for 1ms interrupts.
 * timer0_get_current_time() - return the last updated time.
 **************************************************************
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0_1ms_interrupts.h"

/* 
* 'volatile' informs the compiler that the value can be changed
* by an external mechanism (e.g interrupt) 
*/
static volatile uint32_t clockTicks;

/*
* Set up timer0 to generate an interrupt approximately every 1ms.
*
* Calculation of interrupt frequency:
*
*	CPU frequency = 16MHz
*	Prescaler = 256
*	Output compare value = 62
*
* So, (256/16MHz) * 62 = 0.000992 Seconds = 1ms
*/
void timer0_init() {
	clockTicks = 0L;
	TCNT0 = 0;		/* Clear the timer */
	OCR0A = 62;		/* Set the output compare value to be 124 */
	TCCR0A = (1<<WGM01);	/* Set timer to clear on compare match (CTC mode) */
	TCCR0B = (1<<CS02);	/* Divide the clock by 256 */
	TIMSK0 |= (1<<OCIE0A);	/* Enable interrupts on output compare match */
	TIFR0 &= (1<<OCF0A);	/* Clear interrupt flag */
}

/*
* Return the current value of the global clockTicks variable, accounting for 
* possible overflow. To ensure the returned value is consistent, interrupts
* are temporarily disabled during the read operation.
* 
*/
uint32_t timer0_get_current_time() {
	uint32_t returnValue;
	uint8_t interruptsOn = bit_is_set(SREG, SREG_I);
	cli();	/* Disable interrupts */
	 
	/* Ensure the return value is valid and any overflow is accounted for */
	returnValue = clockTicks % (UINT32_MAX + 1);
	
	if(interruptsOn) {
		sei(); /* Re-enable interrupts */
	}
	return returnValue;
}

/* Increment our clock tick count every 1ms */
ISR(TIMER0_COMPA_vect) {
	clockTicks++;
}