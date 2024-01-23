/*
 * piezo_buzzer_328p.h
 *
 * Created: 23/01/2024 2:48:28 PM
 *  Author: Tom
 */ 


#ifndef PIEZO_BUZZER_328P_H_
#define PIEZO_BUZZER_328P_H_

#define F_CPU 16000000L
#define BUZZER_PIN 2 /* Pin C2 */
#define BUZZER_SOUND_ON		0x01
#define BUZZER_SOUND_OFF	0x00

uint8_t BUZZER_SOUND;

void buzzer_init();
void buzzer_set_frequency(uint16_t frequency);
void buzzer_play_tone();
void buzzer_stop_tone();

#endif /* PIEZO_BUZZER_328P_H_ */