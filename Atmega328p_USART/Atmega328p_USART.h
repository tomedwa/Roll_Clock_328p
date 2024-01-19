/*
 **************************************************************
 * Atmega328p_USART.h
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

#ifndef ATMEGA328P_USART_H_
#define ATMEGA328P_USART_H_

#ifndef F_CPU
	#define F_CPU 16000000 // 16MHz
#endif

#define BAUD 9600
#define BRC F_CPU/16/BAUD-1

void USART_init();
void USART_transmit_character(unsigned char data);
void USART_transmit_string(char* string);

#endif /* ATMEGA328P_USART_H_ */