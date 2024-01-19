/*
 **************************************************************
 * Atmega328p_USART.c
 * Author: Tom
 * Date: 16/11/2023
 * Simple AVR Library for USART serial communication. Only
 * transmission has been added for this so far.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * USART_init() - Initialise the 328p for USART comms.
 * USART_transmit_character() - Transmit a char over USART.
 * USART_transmit_string() - Transmit a string over USART.
 **************************************************************
*/

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#include "Atmega328p_USART.h"

/*
 * USART_init()
 * -------------
 * Initialise the Atmega328p for USART communication.
*/
void USART_init() {
	UBRR0H = (BRC >> 8); //Set baud rate
	UBRR0L = BRC;
	UCSR0C =  (1 << UCSZ00) | (1 << UCSZ01); //8bit data frame
	UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0); //Enable tx and rx enable
	UCSR0A = (1<<RXC0);
}

/*
 * USART_transmit_character()
 * --------------------------
 * Transmit a single character over the USART tx line.
*/
void USART_transmit_character(unsigned char data) {
	while (!( UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

/*
 * USART_transmit_string()
 * -----------------------
 * Transmit a string of characters over the USART tx line. Ensure the string
 * is null terminated.
*/
void USART_transmit_string(char* string) {
	for (int i = 0; i < strlen(string); i++) {
		USART_transmit_character(string[i]);
	}
}

ISR(USART_RX_vect) {
	
}