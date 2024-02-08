/*
 **************************************************************
 * buttons.c
 * Author: Tom
 * Date: 08/02/2024
 * Designed for my Roll Clock Project. This lib manages the 
 * interrupts and status of buttons for the project.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * buttons_init() - Initialise the buttons and interrupts.
 * buttons_select_status() - Status of select button.
 * buttons_next_status() - Status of the next button.
 * buttons_button_down() - Checks if a button has been pressed.
 * buttons_select_set_status() - Set the status of the select button.
 * buttons_next_set_status() - Set the status of the next button.
 **************************************************************
*/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define F_CPU 16000000L	/* Clock frequency of CPU */
#include <util/delay.h>

#include "buttons.h"

static uint8_t _selectButtonStatus;
static uint8_t _nextButtonStatus;

void buttons_init() {
	_selectButtonStatus = BUTTON_RELEASED;
	_nextButtonStatus = BUTTON_PRESSED;
	
	/* Set PD2 (INT0) and PD3 (INT1) as input with pull-down resistor enabled */
	DDRD &= ~((1 << 2) | (1 << 3));
	PORTD &= ~((1 << 2) | (1 << 3));
	
	/* Enable external interrupts INT0 and INT1 */
	EICRA |= (1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11); /* Rising edge triggers interrupts for INT0 and INT1 */
	EIMSK |= (1 << INT0) | (1 << INT1);   /* Enable INT0 and INT1 interrupts */
	
	// Enable global interrupts
	sei();
	
}

uint8_t buttons_select_status() {
	return _selectButtonStatus;
}

uint8_t buttons_next_status() {
	return _nextButtonStatus;
}

uint8_t buttons_button_down(uint8_t button) {
	uint8_t returnVal = 0;
	
	switch(button) {
		case BUTTON_SELECT:
			returnVal = !!(PIND & (1 << 2));
			break;
		case BUTTON_NEXT:
			returnVal = !!(PIND & (1 << 3));
			break;
	}
	
	return returnVal;
}

void buttons_select_set_status(uint8_t status) {
	_selectButtonStatus = status;
}
void buttons_next_set_status(uint8_t status) {
	_nextButtonStatus = status;
}

ISR(INT0_vect) {
	/* Disable further interrupts on INT0 */
	EIMSK &= ~(1 << INT0);
	
	/* Wait for short debounce period */
	_delay_ms(50);
	
	_selectButtonStatus = BUTTON_PRESSED;

	/* Re-enable interrupts on INT0 */
	EIMSK |= (1 << INT0);
}

ISR(INT1_vect) {
	/* Disable further interrupts on INT1 */
	EIMSK &= ~(1 << INT1);
	
	/* Wait for debounce period */
	_delay_ms(50);
	
	_nextButtonStatus = BUTTON_PRESSED;

	/* Re-enable interrupts on INT1 */
	EIMSK |= (1 << INT1);
}