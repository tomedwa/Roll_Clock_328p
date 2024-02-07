/*
 **************************************************************
 * MODE_A.h
 * Author: Tom
 * Date: 05/02/2024
 * Designed for my Roll Clock Project. This lib manages the 
 * display and modification of the current time, date and alarm
 * time.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * MODE_A_init() - Initialise mode A.
 * MODE_A_control() - Execute mode A functionality.
 **************************************************************
*/

#ifndef MODE_A_H_
#define MODE_A_H_

#define MODE_A 0x00

#define MODE_A_SETTINGS_OFF		0x00
#define MODE_A_SETTINGS_ON		0x01

#define MODE_A_SETTINGS_SELECTION_SET_TIME		0x00
#define MODE_A_SETTINGS_SELECTION_SET_DATE		0x01
#define MODE_A_SETTINGS_SELECTION_SET_ALARM		0x02
#define MODE_A_SETTINGS_SELECTION_NONE			0x03

#define MODE_A_SETTINGS_HOLD_DIGIT		0x00
#define MODE_A_SETTINGS_INCREMENT_DIGIT	0x01

/*	
Used to index the private string (_settingsString) used to display and modify 
the time, date and alarm time.
The structure of the string is, LL:MM:RR, where L = Left,
M = Middle, and R = Right. This can be used to represent 
the time or date. e.g Time = HH:MM:SS and Date = DD:MM:YY.
*/
#define MODE_A_STRING_INDEX_LEFT_TENS	0x00	/* Tens place of the left segment */
#define MODE_A_STRING_INDEX_LEFT_ONES	0x01	/* Ones place of the left segment */
#define MODE_A_STRING_INDEX_MIDDLE_TENS	0x03	/* Tens place of the middle segment */
#define MODE_A_STRING_INDEX_MIDDLE_ONES	0x04	/* Ones place of the middle segment */
#define MODE_A_STRING_INDEX_RIGHT_TENS	0x06	/* Tens place of the right segment */
#define MODE_A_STRING_INDEX_RIGHT_ONES	0x07	/* Ones place of the right segment */

#define MODE_A_BUTTON_RELEASED	0x00
#define MODE_A_BUTTON_PRESSED	0x01

void MODE_A_init();
void MODE_A_control();

#endif /* MODE_A_H_ */