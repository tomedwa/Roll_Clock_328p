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

#ifndef TIMER0_1MS_INTERRUPTS_H_
#define TIMER0_1MS_INTERRUPTS_H_

#include <stdint.h>

void timer0_init();
uint32_t timer0_get_current_time();

#endif /* TIMER0_1MS_INTERRUPTS_H_ */