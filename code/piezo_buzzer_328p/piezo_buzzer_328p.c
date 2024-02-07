/*
 * piezo_buzzer_328p.c
 *
 * Created: 23/01/2024 2:48:43 PM
 *  Author: Tom
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "piezo_buzzer_328p.h"

void buzzer_init() {
	BUZZER_SOUND = BUZZER_SOUND_OFF;

	TCCR1B |= (1 << WGM12);
	OCR1A = 10000;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS12);
	
	DDRC |= (1 << BUZZER_PIN);
	PORTC &= ~(1 << BUZZER_PIN);
}

void buzzer_set_frequency(uint16_t frequency) {
	OCR1A = (uint16_t)((F_CPU / (2UL * 256UL * frequency)) - 1);
}

void buzzer_play_tone() {
	BUZZER_SOUND = BUZZER_SOUND_ON;
}

void buzzer_stop_tone() {
	BUZZER_SOUND = BUZZER_SOUND_OFF;
}

ISR(TIMER1_COMPA_vect) {
	if (BUZZER_SOUND == BUZZER_SOUND_ON) {
		PORTC ^= (1 << BUZZER_PIN);
	} else {
		PORTC &= ~(1 << BUZZER_PIN);
	}
}