/*
 **************************************************************
 * buttons.h
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

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON_RELEASED	0x00
#define BUTTON_PRESSED	0x01

#define BUTTON_SELECT	0x00
#define BUTTON_NEXT		0x01

void buttons_init();
uint8_t buttons_select_status();
uint8_t buttons_next_status();
uint8_t buttons_button_down(uint8_t button);
void buttons_select_set_status(uint8_t status);
void buttons_next_set_status(uint8_t status);

#endif /* BUTTONS_H_ */