/*
 **************************************************************
 * main.c
 * Author: Tom
 * Date: 19/01/2024
 * Main file for Roll Clock project.
 **************************************************************
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define F_CPU 16000000L /* Using external 16MHz crystal oscillator */
#include <util/delay.h>

#include "SH1106_OLED/SH1106.h"
#include "Atmega328p_SPI/Atmega328p_SPI.h"
#include "pFleury_i2c_stuff/i2cmaster.h"
#include "timer0_1ms_interrupts/timer0_1ms_interrupts.h"
#include "ADXL343_accelerometer/ADXL343.h"
#include "MCP7940N_RTCC/MCP7940N.h"
#include "Atmega328p_USART/Atmega328p_USART.h"
#include "XBM_symbols/XBM_symbols.h"
#include "AM2320_temperature_humidity/AM2320_temperature_humidity.h"
#include "piezo_buzzer_328p/piezo_buzzer_328p.h"
#include "roll_clock_modes/MODE_A.h"

/* Defines for keeping track of delays */
#define NUM_PREVIOUS_TIMES 5
#define ADXL_PREV_TIME_INDEX 0
#define AM2320_UPDATE_READINGS_INDEX 1
#define RTC_UPDATE_CURRENT_TIME_INDEX 2
#define ALARM_INVERT_DISPLAY_INDEX 3
#define INVERT_DISPLAY_ALARM_INDEX 4
#define ADXL_AXIS_READ_INTERVAL 813
#define AM2320_UPDATE_READINGS_INTERVAL 20000
#define RTC_UPDATE_CURRENT_TIME_INTERVAL 3
#define ALARM_INVERT_DISPLAY_INTERVAL 213
#define INVERT_DISPLAY_ALARM_INTERVAL 500

/* Thresholds for determining the orientation of the screen */
#define AXIS_ACTIVE 1400
#define AXIS_INACTIVE 500

/* Different modes based on display orientation */
#define MODE_B 0x01
#define MODE_C 0x02
#define MODE_D 0x03

/* Display Macros */
#define DISPLAY_INVERTED	0x01
#define DISPLAY_NORMAL		0x00

/* Function prototypes */
void hardware_init();
void initialise_current_and_previous_times(uint32_t* currentTime, uint32_t* previousTimes);
uint8_t update_current_orientation(uint8_t lastOrientation);
void update_ADXL_data(uint32_t currentTime, uint32_t* previousTimes, uint8_t* lastOrientation, uint8_t* currentOrientation);
void update_rtc_current_time(uint32_t currentTime, uint32_t* previousTimes);
void update_temp_humidity_sensor(uint32_t currentTime, uint32_t* previousTimes);
void display_date_and_time();
void display_temp_humidity();
void alarm_match_handling(uint32_t currentTime, uint32_t* previousTimes, uint8_t* displayInvertedStatus);

int main(void) {
	
	MODE_A_init();
	hardware_init();
	RTC_alarm_enable_disable(RTC_ALARM_ENABLED);
	
	RTC_set_time(0x23, 0x59, 0x55);
	RTC_set_alarm_time(0x00, 0x00, 0x05);
	RTC_set_weekday(1);
	RTC_set_date(0x31, 0x12, 0x98);
	
	/* Current and previous times used for delays */
	uint32_t currentTime;
	uint32_t previousTimes[NUM_PREVIOUS_TIMES];
	initialise_current_and_previous_times(&currentTime, previousTimes);
	
	/* Initial orientation of the display */
	uint8_t currentOrientation = MODE_A;
	uint8_t lastOrientation = currentOrientation;
	
	/* Status of the display (inverted or not) */
	uint8_t displayInvertedStatus = DISPLAY_NORMAL;
	
	/* Initialise buzzer tone */
	buzzer_set_frequency(444);
	buzzer_stop_tone();
	

	
	
	/* Not permanent solution */
	DDRC &= ~(1 << 4); // Alarm off

	
    while (1) {
		/* Update current system time */
		currentTime = timer0_get_current_time(); 
		
		/* Update the time from the RTC */
		update_rtc_current_time(currentTime, previousTimes);
		alarm_match_handling(currentTime, previousTimes, &displayInvertedStatus);
		
		/* Update axis readings from ADXL343 and orientation value */
		update_ADXL_data(currentTime, previousTimes, &lastOrientation, &currentOrientation);
		
		/* Different functionality based on orientation */
		switch(currentOrientation) {
			case MODE_A:
				/*
				Mode A controls displaying the date and time.
				Also allows user to set the time, date and alarm time.
				*/
				MODE_A_control();	
				break;
			case MODE_B:
				/* Update the temperature and humidity readings from AM2320 sensor */
				update_temp_humidity_sensor(currentTime, previousTimes);
				display_temp_humidity();
				break;
			case MODE_C:
				OLED_clear_buffer();
				OLED_draw_string("NRF MODE?", 0, 0, 8, 2, MODE_C);
				OLED_display_buffer();
				break;
			case MODE_D:
				OLED_clear_buffer();
				OLED_draw_string("Mode D", 0, 0, 8, 2, MODE_D);
				OLED_display_buffer();
				break;
		}
    }
}

/*
* hardware_init()
* ---------------
* Initialise any hardware for the project here.
*/
void hardware_init() {
	sei();						/* Enable global interrupts */
	i2c_init();					/* i2c for MCU */
	A328p_SPI_init();			/* SPI for MCU */
	OLED_init();				/* SH1106 OLED display */
	timer0_init();				/* Initialise timer0 to generate interrupts every 1ms */
	RTC_init();					/* Clock IC */
	USART_init();				/* USART for MCU */
	ADXL343_setup_axis_read();	/* Using i2c mode */
	buzzer_init();
}

/*
* Returns the current orientation of the accelerometer based on the current axis readings.
* 0 = Normal. Mode A
* 1 = Rotated counter clockwise 90 degrees from normal. Mode B
* 2 = upside down (180 degrees from normal). Mode C
* 3 = rotated clockwise 90 degrees from normal. Mode D
*
* The return from this function can be used in the 'screenOrientation' input for the
* OLED_draw_string() funciton.
*/
uint8_t update_current_orientation(uint8_t lastOrientation) {
	/* Get the current readings from the accelerometer */
	int32_t x = ADXL343_get_x_axis_int();
	int32_t y = ADXL343_get_y_axis_int();
	int32_t z = ADXL343_get_z_axis_int();
	
	uint8_t currentOrientation = lastOrientation;
	
	/* If the clock is not upright then stay in the previous orientation */
	if ((z > (AXIS_INACTIVE * 1.5)) || (z < -(AXIS_INACTIVE *1.5))) {
		/* Do nothing */
	
	/* Mode A */
	} else if ((y < -AXIS_ACTIVE) && ((x > -AXIS_INACTIVE) || (x < AXIS_INACTIVE))) {
		currentOrientation = MODE_A;
	
	/* Mode B */
	} else if ((x < -AXIS_ACTIVE) && ((y > -AXIS_INACTIVE) || (y < AXIS_INACTIVE))) {
		currentOrientation = MODE_B;
	
	/* Mode C */
	} else if ((y > AXIS_ACTIVE) && ((x > -AXIS_INACTIVE) || (x < AXIS_INACTIVE))) {
		currentOrientation = MODE_C;
	
	/* Mode D */
	} else if ((x > AXIS_ACTIVE) && ((y > -AXIS_INACTIVE) || (y < AXIS_INACTIVE))) {
		currentOrientation = MODE_D;
	}
	
	return currentOrientation;
}




void display_temp_humidity() {
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

/*
* update_ADXL_data()
* ----------------
* Update the axis data in the adxl343 and update the variable holding the current orientation of the display
* based on the new axis data.
*/
void update_ADXL_data(uint32_t currentTime, uint32_t* previousTimes, uint8_t* lastOrientation, uint8_t* currentOrientation) {
	if ((currentTime - previousTimes[ADXL_PREV_TIME_INDEX]) > ADXL_AXIS_READ_INTERVAL) {
		ADXL343_update_axis_readings();
		
		*lastOrientation = *currentOrientation;
		*currentOrientation = update_current_orientation(*lastOrientation); /* Update the orientation of the display */

		previousTimes[ADXL_PREV_TIME_INDEX] = currentTime; /* Update previous time for ADXL to reset delay */
	}
}

/*
* initialise_current_and_previous_times()
* ---------------------------------------
* Initialise the current time and previous times. Just to reduce the amount of code in main function.
*/
void initialise_current_and_previous_times(uint32_t* currentTime, uint32_t* previousTimes) {
	*currentTime = timer0_get_current_time() + 30000L;
	for (uint8_t i = 0; i < NUM_PREVIOUS_TIMES; i++) {
		previousTimes[i] = *currentTime;
	}
}

void update_rtc_current_time(uint32_t currentTime, uint32_t* previousTimes) {

	if ((currentTime - previousTimes[RTC_UPDATE_CURRENT_TIME_INDEX]) > RTC_UPDATE_CURRENT_TIME_INTERVAL) {
		RTC_update_current_time();
		previousTimes[RTC_UPDATE_CURRENT_TIME_INDEX] = currentTime;
	}
	
}

void update_temp_humidity_sensor(uint32_t currentTime, uint32_t* previousTimes) {
	if ((currentTime - previousTimes[AM2320_UPDATE_READINGS_INDEX]) > AM2320_UPDATE_READINGS_INTERVAL) {
		AM2320_update_temperature_humidity();
		previousTimes[AM2320_UPDATE_READINGS_INDEX] = currentTime;
	}
}

void alarm_match_handling(uint32_t currentTime, uint32_t* previousTimes, uint8_t* displayInvertedStatus) {
	/* Check if the alarm is activated and invert the display if necessary */
	if (RTC_check_alarm_match() == RTC_ALARM_ACTIVE) {
		if ((currentTime - previousTimes[INVERT_DISPLAY_ALARM_INDEX]) > INVERT_DISPLAY_ALARM_INTERVAL) {
			*displayInvertedStatus ^= 1;
			previousTimes[INVERT_DISPLAY_ALARM_INDEX] = currentTime;
			OLED_display_invert(*displayInvertedStatus);
		}
	} else {
		if (*displayInvertedStatus == DISPLAY_INVERTED) {
			*displayInvertedStatus = DISPLAY_NORMAL;
			OLED_display_invert(*displayInvertedStatus);
		}
	}
	
	if (!!(PINC & (1<<3)) == 1) {
		RTC_alarm_deactivate();
	}
	
	if (*displayInvertedStatus == DISPLAY_INVERTED) {
		buzzer_play_tone();
	} else {
		buzzer_stop_tone();
	}
}