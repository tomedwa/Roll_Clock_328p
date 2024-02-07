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

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define F_CPU 16000000L	/* Clock frequency of CPU */
#include <util/delay.h>

#include "MODE_A.h"
#include "../MCP7940N_RTCC/MCP7940N.h"
#include "../SH1106_OLED/SH1106.h"
#include "../XBM_symbols/XBM_symbols.h"

static uint8_t _menuHighlight; /* Which menu item is highlighted */
static uint8_t _menuSelection;	/* Which menu item is selected */
static uint8_t _settingsModeStatus;	/* Is mode A in settings mode or not */
static char _settingsString[9]; /* Used to display and modify the time, date and alarm */

/* Current index of digit in _settingsString selected when changing the time, date or alarm */
static uint8_t _selectedDigit; 

static uint8_t _digitIncrementFlag;	/* Should the selected digit be increased or not */

/*
To modify the date accurately, the order follows a specific sequence: Year Tens, Year Ones, 
Month Tens, Month Ones, Day Tens, Day Ones. This array establishes a connection between the 
_selectedDigit index and the corresponding index to be modified when changing the date. 
The '-1' values indicate the positions of non-integer characters in the _settingsString 
(e.g., the colons in HH:MM:SS).
*/
static const int8_t _dateDigitsIndexOffset[8] = {6, 7, -1, 3, 4, -1, 0, 1};
	
static uint8_t _selectPress; /* Signals when the select button has been pressed */
static uint8_t _nextPress; /* Signals when the next button has been pressed */

/* Private function prototypes */
static void _display_date_and_time();
static void _display_settings_menu();
static void _button_select_logic();
static void _menu_highlight_option();
static void _button_next_logic();
static void _display_set_time();
static void _display_set_alarm();
static void _display_set_date();
static void _increase_selected_time_digit();
static void _increase_selected_date_digit();
static void _selected_digit_highlight();
static void _string_init();
static void _string_confirm();


/*
* MODE_A_init()
* -------------
* External function to initialise the variables and external inputs
* for mode A.
*/
void MODE_A_init() {
	_menuHighlight = MODE_A_SETTINGS_SELECTION_SET_TIME;
	_menuSelection = MODE_A_SETTINGS_SELECTION_NONE;
	_settingsModeStatus = MODE_A_SETTINGS_OFF;
	_selectedDigit = MODE_A_STRING_INDEX_LEFT_TENS;
	_digitIncrementFlag = MODE_A_SETTINGS_HOLD_DIGIT;
	
	_selectPress = MODE_A_BUTTON_RELEASED;
	_nextPress = MODE_A_BUTTON_RELEASED;
	
	/* Set PD2 (INT0) and PD3 (INT1) as input with pull-down resistor enabled */
	DDRD &= ~((1 << 2) | (1 << 3));
	PORTD &= ~((1 << 2) | (1 << 3));
	
	/* Enable external interrupts INT0 and INT1 */
	EICRA |= (1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11); /* Rising edge triggers interrupts for INT0 and INT1 */
	EIMSK |= (1 << INT0) | (1 << INT1);   /* Enable INT0 and INT1 interrupts */
	
	// Enable global interrupts
	sei();
}

ISR(INT0_vect) {
	/* Disable further interrupts on INT0 */
	EIMSK &= ~(1 << INT0);
	
	/* Wait for short debounce period */
	_delay_ms(10);
		
	_selectPress = 1;

	 /* Re-enable interrupts on INT0 */
	 EIMSK |= (1 << INT0);
}

ISR(INT1_vect) {
	/* Disable further interrupts on INT1 */
	EIMSK &= ~(1 << INT1);
	
	/* Wait for debounce period */
	_delay_ms(10); 
	
	_nextPress = 1;

	/* Re-enable interrupts on INT1 */
	EIMSK |= (1 << INT1);
}

/*
* MODE_A_control()
* ----------------
* External function that controls all of the main functionality of mode A.
*/
void MODE_A_control() {
	
	/* If an interrupt for the select button is detected or if 
	the select button has been held down */
	if (_selectPress == 1 || (PIND & (1 << 2))) {
		_selectPress = 0;
		_button_select_logic();
	}
	
	/* If an interrupt for the next button is detected or if 
	the next button has been held down */
	if (_nextPress == 1 || (PIND & (1 << 3))) {
		_nextPress = 0;
		_button_next_logic();
	}
	
	
	if (_settingsModeStatus == MODE_A_SETTINGS_OFF) {
		_display_date_and_time();
	} else {
		if (_menuSelection == MODE_A_SETTINGS_SELECTION_NONE) {
			_display_settings_menu();
		} else if (_menuSelection == MODE_A_SETTINGS_SELECTION_SET_TIME) {
			_display_set_time();
		} else if (_menuSelection == MODE_A_SETTINGS_SELECTION_SET_DATE) {
			_display_set_date();
		} else if (_menuSelection == MODE_A_SETTINGS_SELECTION_SET_ALARM) {
			_display_set_alarm();
		}
	}
	
}

/*
* _display_date_and_time()
* ------------------------
* Private function used to display the current date and time.
*/
static void _display_date_and_time() {
	char currentTime[9];
	char dayDateString[9];
	
	RTC_get_time_string(currentTime);
	RTC_get_date_string(dayDateString);
	
	OLED_clear_buffer();
	
	OLED_draw_string(currentTime, 6, 4, 25, 5, MODE_A);
	OLED_draw_string(dayDateString, 5, 41, 16, 2, 0);
	
	/* Boxes surrounding the time and date */
	OLED_draw_horizontal_line(0, 127, 33);
	OLED_draw_horizontal_line(0, 127, 36);
	OLED_draw_vertical_line(36, 63, 102);
	OLED_draw_rectangle(0, 0, 127, 63, 0);
	
	/* Temp */
	OLED_draw_xbm(106, 37, alarmBellIconUnarmed, 18, 24, MODE_A);
	
	OLED_display_buffer();
}

/*
* _display_settings_menu()
* ------------------------
* Private function to display the settings menu. The menu options 
* are Set Time, Set Date, and Set Alarm.
*/
static void _display_settings_menu() {
	
	OLED_clear_buffer();
	OLED_draw_string("Set Time", 6, 2, 16, 2, MODE_A);
	OLED_draw_string("Set Date", 6, 22, 16, 2, MODE_A);
	OLED_draw_string("Set Alarm", 6, 42, 16, 2, MODE_A);
	_menu_highlight_option();
	OLED_display_buffer();
}

/*
* _menu_highlight_option()
* ------------------------
* Private function to highlight the menu option the user is currently
* looking at. Highlight = Invert the section of the OLED display.
*/
static void _menu_highlight_option() {
	
	switch(_menuHighlight) {
		case MODE_A_SETTINGS_SELECTION_SET_TIME:
			OLED_invert_rectangle(0, 128, 0, 20);
			break;
		
		case MODE_A_SETTINGS_SELECTION_SET_DATE:
			OLED_invert_rectangle(0, 128, 20, 40);
			break;
			
		case MODE_A_SETTINGS_SELECTION_SET_ALARM:
			OLED_invert_rectangle(0, 128, 40, 60);
			break;
	}
}

/*
* _button_select_logit()
* ----------------------
* Private function to handle the logic of the select button used for 
* selecting options in the menu and confirming changes made when setting
* the time, date and alarm time.
*/
static void _button_select_logic() {
	if (_settingsModeStatus == MODE_A_SETTINGS_OFF) {
		_settingsModeStatus = MODE_A_SETTINGS_ON;
		} else if (_menuSelection == MODE_A_SETTINGS_SELECTION_NONE) {
		_menuSelection = _menuHighlight;
		_menuHighlight = MODE_A_SETTINGS_SELECTION_SET_TIME;
		_string_init();
	} else if (	(_menuSelection == MODE_A_SETTINGS_SELECTION_SET_TIME)	||
				(_menuSelection == MODE_A_SETTINGS_SELECTION_SET_ALARM) ||
				(_menuSelection == MODE_A_SETTINGS_SELECTION_SET_DATE)) {
		if (_selectedDigit == MODE_A_STRING_INDEX_RIGHT_ONES) {
			_string_confirm();
			_menuSelection = MODE_A_SETTINGS_SELECTION_NONE;
			_selectedDigit = MODE_A_STRING_INDEX_LEFT_TENS;
			_digitIncrementFlag = MODE_A_SETTINGS_HOLD_DIGIT;
			_menuHighlight = MODE_A_SETTINGS_SELECTION_SET_TIME;
			_settingsModeStatus = MODE_A_SETTINGS_OFF;
		} else {
			_selectedDigit++;
			if ((_selectedDigit == 2) || (_selectedDigit == 5)) {
				_selectedDigit++;
			}
		}
	}
}

/*
* _button_next_logic()
* --------------------
* Private function to handle the logic of the next button used for cycling through the
* options in the menu and changing the values of the selected digits when setting the
* time, date or alarm time.
*/
static void _button_next_logic() {
	if (_settingsModeStatus == MODE_A_SETTINGS_ON) {
		if (_menuSelection == MODE_A_SETTINGS_SELECTION_NONE) {
			_menuHighlight = (_menuHighlight + 1) % 3;
		} else {
			_digitIncrementFlag = MODE_A_SETTINGS_INCREMENT_DIGIT;
		}
	}
}

/*
* _display_set_time()
* -------------------
* Private function used to display the set time section of the settings.
*/
static void _display_set_time() {
	_increase_selected_time_digit();
	OLED_clear_buffer();
	OLED_draw_string("Set Time", 21, 7, 16, 2, MODE_A);
	OLED_draw_string(_settingsString, 6, 33, 25, 5, MODE_A);
	_selected_digit_highlight();
	OLED_display_buffer();
}

/* 
* _string_init()
* ---------------
* Private function to initialise the _settingsString to represent the changes that will
* be made in the settings. The string will be initialised with the current time when
* when setting a new time. Initialised with 00-00-00 when setting a new date. And initialised
* with the current alarm time when setting a new alarm time.
*/
static void _string_init() {
	switch(_menuSelection) {
		
		case MODE_A_SETTINGS_SELECTION_SET_TIME:
			
			RTC_get_time_string(_settingsString);
			break;
		
		case MODE_A_SETTINGS_SELECTION_SET_DATE:
		
			for (uint8_t i = 0; i < 8; i++) {
				if ((i == 2) || (i == 5)) {
					_settingsString[i] = '-';
					} else {
					_settingsString[i] = '0';
				}
			}
			_settingsString[8] = '\0';
			break;
			
		case MODE_A_SETTINGS_SELECTION_SET_ALARM:
			RTC_get_alarm_time_string(_settingsString);
			break;
	}
}

/*
* _increase_selected_time_digit()
* -------------------------------
* Private function for changing the selected digit in the time or alarm time. 
*/
static void _increase_selected_time_digit() {
	if (_digitIncrementFlag == MODE_A_SETTINGS_HOLD_DIGIT) {
		return;
	}
	
	_digitIncrementFlag = MODE_A_SETTINGS_HOLD_DIGIT;
	
	/* Used to extract the tens and ones digits as integers */
	uint8_t tempTen;
	uint8_t tempOne;
	
	/* Increase hours tens digit */
	if (_selectedDigit == MODE_A_STRING_INDEX_LEFT_TENS) {
		tempTen = _settingsString[_selectedDigit] - '0';
		_settingsString[_selectedDigit] = ((tempTen + 1) % 3) + '0';
	
	/* Increase hours ones digit */
	} else if (_selectedDigit == MODE_A_STRING_INDEX_LEFT_ONES) {
		tempTen = _settingsString[MODE_A_STRING_INDEX_LEFT_TENS] - '0';
		tempOne = _settingsString[_selectedDigit] - '0';
		
		if (tempTen == 2) {
			_settingsString[_selectedDigit] = ((tempOne + 1) % 4) + '0';
			
		} else {
			_settingsString[_selectedDigit] = ((tempOne + 1) % 10) + '0';
		}
	
	/* Increase minutes tens digit */	
	} else if (_selectedDigit == MODE_A_STRING_INDEX_MIDDLE_TENS) {
		tempTen = _settingsString[_selectedDigit] - '0';
		_settingsString[_selectedDigit] = ((tempTen + 1) % 6) + '0';
		
	/* Increase minutes ones digit */
	} else if (_selectedDigit == MODE_A_STRING_INDEX_MIDDLE_ONES) {
		tempOne = _settingsString[_selectedDigit] - '0';
		_settingsString[_selectedDigit] = ((tempOne + 1) % 10) + '0';
	
	/* Increase seconds tens digit */	
	} else if (_selectedDigit == MODE_A_STRING_INDEX_RIGHT_TENS) {
		tempTen = _settingsString[_selectedDigit] - '0';
		_settingsString[_selectedDigit] = ((tempTen + 1) % 6) + '0';
	
	/* Increase seconds ones digit */
	} else if (_selectedDigit == MODE_A_STRING_INDEX_RIGHT_ONES) {
		tempOne = _settingsString[_selectedDigit] - '0';
		_settingsString[_selectedDigit] = ((tempOne + 1) % 10) + '0';
	}
	
	
}

/*
* _selected_digit_highlight()
* ---------------------------
* Private function to highlight the currently selected digit when
* setting the time, date or alarm time.
*/
static void _selected_digit_highlight() {
	
	if ((_menuSelection == MODE_A_SETTINGS_SELECTION_SET_TIME) || (_menuSelection == MODE_A_SETTINGS_SELECTION_SET_ALARM)) {
		switch(_selectedDigit) {
			case MODE_A_STRING_INDEX_LEFT_TENS:
				OLED_invert_rectangle(3, 21, 30, 61);
				break;
			case MODE_A_STRING_INDEX_LEFT_ONES:
				OLED_invert_rectangle(20, 38, 30, 61);
				break;
			case MODE_A_STRING_INDEX_MIDDLE_TENS:
				OLED_invert_rectangle(46, 64, 30, 61);
				break;
			case MODE_A_STRING_INDEX_MIDDLE_ONES:
				OLED_invert_rectangle(63, 81, 30, 61);
				break;
			case MODE_A_STRING_INDEX_RIGHT_TENS:
				OLED_invert_rectangle(89, 107, 30, 61);
				break;
			case MODE_A_STRING_INDEX_RIGHT_ONES:
				OLED_invert_rectangle(106, 124, 30, 61);
				break;
		}
	} else if (_menuSelection == MODE_A_SETTINGS_SELECTION_SET_DATE) {
		int8_t highlight = _dateDigitsIndexOffset[_selectedDigit];
		
		if (highlight == -1) {
			return;
		}
		
		switch(highlight) {
			case MODE_A_STRING_INDEX_LEFT_TENS:
			OLED_invert_rectangle(3, 19, 30, 52);
			break;
			case MODE_A_STRING_INDEX_LEFT_ONES:
			OLED_invert_rectangle(18, 34, 30, 52);
			break;
			case MODE_A_STRING_INDEX_MIDDLE_TENS:
			OLED_invert_rectangle(48, 64, 30, 52);
			break;
			case MODE_A_STRING_INDEX_MIDDLE_ONES:
			OLED_invert_rectangle(63, 79, 30, 52);
			break;
			case MODE_A_STRING_INDEX_RIGHT_TENS:
			OLED_invert_rectangle(93, 109, 30, 52);
			break;
			case MODE_A_STRING_INDEX_RIGHT_ONES:
			OLED_invert_rectangle(108, 124, 30, 52);
			break;
		}
	}
	
}

/*
* _string_confirm()
* -----------------
* Private string to confirm the changes made when setting the time, date or alarm time. 
*/
static void _string_confirm() {
	uint8_t temp1;
	uint8_t temp2;
	uint8_t temp3;
	
	switch (_menuSelection) {
		
		case MODE_A_SETTINGS_SELECTION_SET_TIME:
		
		temp1 = ((_settingsString[0] - 48) << 4) | (_settingsString[1] - 48); /*  Hours	*/
		temp2 = ((_settingsString[3] - 48) << 4) | (_settingsString[4] - 48); /*  Minutes	*/
		temp3 = ((_settingsString[6] - 48) << 4) | (_settingsString[7] - 48); /*  Seconds	*/
		RTC_set_time(temp1, temp2, temp3);
		break;
		
		case MODE_A_SETTINGS_SELECTION_SET_DATE:
		
		temp1 = ((_settingsString[0] - 48) << 4) | (_settingsString[1] - 48); /*   Day	*/
		temp2 = ((_settingsString[3] - 48) << 4) | (_settingsString[4] - 48); /*   Month	*/
		temp3 = ((_settingsString[6] - 48) << 4) | (_settingsString[7] - 48); /*   Year	*/
		RTC_set_date(temp1, temp2, temp3);
		break;
		
		case MODE_A_SETTINGS_SELECTION_SET_ALARM:
		
		temp1 = ((_settingsString[0] - 48) << 4) | (_settingsString[1] - 48); /*  Hours	*/
		temp2 = ((_settingsString[3] - 48) << 4) | (_settingsString[4] - 48); /*  Minutes	*/
		temp3 = ((_settingsString[6] - 48) << 4) | (_settingsString[7] - 48); /*  Seconds */
		RTC_set_alarm_time(temp1, temp2, temp3);
		break;
		
	}
}

/*
* _display_set_alarm()
* --------------------
* Private function to display the set alarm section of the settings.
*/
static void _display_set_alarm() {
	_increase_selected_time_digit();
	OLED_clear_buffer();
	OLED_draw_string("Set Alarm", 16, 7, 16, 2, MODE_A);
	OLED_draw_string(_settingsString, 6, 33, 25, 5, MODE_A);
	_selected_digit_highlight();
	OLED_display_buffer();
}

/*
* _display_set_date()
* -------------------
* Private function to display the set date section of the settings.
*/
static void _display_set_date() {
	_increase_selected_date_digit();
	OLED_clear_buffer();
	OLED_draw_string("Set Date", 19, 7, 16, 2, MODE_A);
	OLED_draw_string(_settingsString, 6, 33, 16, 5, MODE_A);
	_selected_digit_highlight();
	OLED_display_buffer();
}

/* 
* _increase_selected_date_digit()
* -------------------------------
* Private function for changing the selected digit when setting the date.
*/
static void _increase_selected_date_digit() {
	if (_digitIncrementFlag == MODE_A_SETTINGS_HOLD_DIGIT) {
		return;
	}
	
	int8_t selectedDigit = _dateDigitsIndexOffset[_selectedDigit];
	
	if (selectedDigit == -1) {
		return;
	}
	
	/* Used to extract the tens and ones digits as integers */
	uint8_t tempTen;
	uint8_t tempOne;
	
	uint8_t year = ((_settingsString[MODE_A_STRING_INDEX_RIGHT_TENS] - '0') * 10) + (_settingsString[MODE_A_STRING_INDEX_RIGHT_ONES] - '0');
	uint8_t month = ((_settingsString[MODE_A_STRING_INDEX_MIDDLE_TENS] - '0') * 10) + (_settingsString[MODE_A_STRING_INDEX_MIDDLE_ONES] - '0');
	uint8_t isLeapYear = !(year % 4);
	
	/* Increase day tens */
	if (selectedDigit == MODE_A_STRING_INDEX_LEFT_TENS) {
		tempTen = _settingsString[selectedDigit] - '0';
		
		if (month == RTC_FEBRUARY) {
			_settingsString[selectedDigit] = ((tempTen + 1) % 3) + '0';
		} else {
			_settingsString[selectedDigit] = ((tempTen + 1) % 4) + '0';
		}
	
	/* Increase day ones */
	} else if (selectedDigit == MODE_A_STRING_INDEX_LEFT_ONES) {
		tempTen = _settingsString[MODE_A_STRING_INDEX_LEFT_ONES] - '0';
		tempOne = _settingsString[selectedDigit] - '0';
		
		if (month == RTC_FEBRUARY) {
			if (!isLeapYear && (tempTen == 2)) {
				_settingsString[selectedDigit] = ((tempOne + 1) % 8) + '0';
			} else {
				_settingsString[selectedDigit] = ((tempOne + 1) % 9) + '0';
			}
		} else if (	month == RTC_JANUARY	||
					month == RTC_MARCH		||
					month == RTC_MAY		||
					month == RTC_JULY		||
					month == RTC_AUGUST		||
					month == RTC_OCTOBER	||
					month == RTC_DECEMBER) {
			
			if (tempTen == 3) {
				_settingsString[selectedDigit] = ((tempOne + 1) % 2) + '0';	
			} else {
				_settingsString[selectedDigit] = ((tempOne + 1) % 10) + '0';
			}
						
		} else if (	month == RTC_APRIL		||
					month == RTC_JUNE		||
					month == RTC_SEPTEMBER	||
					month == RTC_NOVEMBER) {
		
			if (tempTen == 3) {
				_settingsString[selectedDigit] = ((tempOne + 1) % 1) + '0';	
			} else {
				_settingsString[selectedDigit] = ((tempOne + 1) % 10) + '0';
			}			
		}
	
	/* Increase month tens */
	} else if (selectedDigit == MODE_A_STRING_INDEX_MIDDLE_TENS) {
		tempTen = _settingsString[selectedDigit] - '0';
		
		if (tempTen == 1) {
			_settingsString[selectedDigit] = ((tempTen + 1) % 2) + '0';
		} else {
			_settingsString[selectedDigit] = ((tempTen + 1) % 10) + '0';
		}
	
	/* Increase month ones */
	} else if (selectedDigit == MODE_A_STRING_INDEX_MIDDLE_ONES) {
		tempTen = _settingsString[MODE_A_STRING_INDEX_MIDDLE_TENS] - '0';
		tempOne = _settingsString[selectedDigit] - '0';
		
		if (tempTen == 1) {
			_settingsString[selectedDigit] = ((tempOne + 1) % 3) + '0';
		} else {
			_settingsString[selectedDigit] = ((tempOne + 1) % 10) + '0';
		}
		
	/* Increase year tens */
	} else if (selectedDigit == MODE_A_STRING_INDEX_RIGHT_TENS) {
		tempTen = _settingsString[selectedDigit] - '0';
		_settingsString[selectedDigit] = ((tempTen + 1) % 10) + '0';
	/* Increase year ones */
	} else if (selectedDigit == MODE_A_STRING_INDEX_RIGHT_ONES) {
		tempOne = _settingsString[selectedDigit] - '0';
		_settingsString[selectedDigit] = ((tempOne + 1) % 10) + '0';
	}
	
	_digitIncrementFlag = MODE_A_SETTINGS_HOLD_DIGIT;
}