/*
 **************************************************************
 * MODE_B.h
 * Author: Tom
 * Date: 05/02/2024
 * Designed for my Roll Clock Project. This lib manages the 
 * display of the temperature and humidity.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * MODE_B_init() - Initialise mode B. (not used yet)
 * MODE_B_control() - Execute mode B functionality.
 **************************************************************
*/

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "MODE_B.h"
#include "../SH1106_OLED/SH1106.h"
#include "../XBM_symbols/XBM_symbols.h"
#include "../AM2320_temperature_humidity/AM2320_temperature_humidity.h"

/* Private function prototypes */
static void _display_temperature_humidity();

/*
* MODE_B_init()
* --------------
* External function to initialise variables.
*/
void MODE_B_init() {
	/* Just in case Mode B becomes more complex in the future.
	Right now this is not required.
	*/
}

/*
* MODE_B_control()
* ----------------
* External function that controls all of the main functionality for mode B.
*/
void MODE_B_control() {
	_display_temperature_humidity();
}

/*
* _display_temperature_humidity()
* -------------------------------
* Private function used to display the temperature (degrees Celsius) and 
* humidity (%) on the OLED screen.
*/
static void _display_temperature_humidity() {
	char temperatureString[7];
	char humidityString[5];
	char degreesCelsiusString[2] = {254, '\0'};
	
	AM2320_get_temperature_string_celsius(temperatureString);
	AM2320_get_humidity_string(humidityString);
	
	OLED_clear_buffer();
	
	OLED_draw_string("Temperature", 0, 16, 8, 1, MODE_B);
	OLED_draw_string(temperatureString, 0, 32, 16, 2, MODE_B);
	OLED_draw_string(degreesCelsiusString, 44, 32, 16, 1, MODE_B);
	
	OLED_draw_string("Humidity", 0, 80, 8, 1, MODE_B);
	OLED_draw_string(humidityString, 0, 96, 16, 2, MODE_B);
	OLED_draw_string("%", 49, 96, 16, 1, MODE_B);
	
	OLED_display_buffer();
}