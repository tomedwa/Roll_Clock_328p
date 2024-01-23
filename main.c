/*
 **************************************************************
 * main.c
 * Author: Tom
 * Date: 19/01/2024
 * Main file for Roll Clock project.
 **************************************************************
*/

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define F_CPU 16000000L /* Using external 16MHz crystal oscillator */
#include <util/delay.h>

#include "SH1106_OLED/SH1106.h"
#include "Atmega328p_SPI/Atmega328p_SPI.h"



#include "pFleury_i2c_stuff/i2cmaster.h"
//#include "i2c_328p/i2c_328p.h"


#include "timer0_1ms_interrupts/timer0_1ms_interrupts.h"
#include "ADXL343_accelerometer/ADXL343.h"
#include "MCP7940N_RTCC/MCP7940N.h"
#include "Atmega328p_USART/Atmega328p_USART.h"
#include "XBM_symbols.h"
#include "AM2320_temperature_humidity/AM2320_temperature_humidity.h"
#include "piezo_buzzer_328p/piezo_buzzer_328p.h"

////////////////////////
#include "float_to_string.h"

/* Defines for keeping track of delays */
#define NUM_PREVIOUS_TIMES 3
#define ADXL_AXIS_READ_INTERVAL 200 /* Every 200ms */
#define ADXL_PREV_TIME_INDEX 0
#define INVERT_DISPLAY_ALARM_INDEX 1
#define INVERT_DISPLAY_ALARM_INTERVAL 1000 /* Every 1s */
#define AM2320_UPDATE_INTERVAL 2000 /* Every 2s */
#define AM2320_UPDATE_INDEX 2
#define RTC_UPDATE_TIME_INDEX 3
#define RTC_UPDATE_TIME_INTERVAL 50 /* Every 50ms */

/* Thresholds for determining the orientation of the screen */
#define AXIS_ACTIVE 1400
#define AXIS_INACTIVE 500

/* Different modes based on display orientation */
#define MODE_A 0
#define MODE_B 1
#define MODE_C 2
#define MODE_D 3

#define DISPLAY_INVERTED	0x01
#define DISPLAY_NORMAL		0x00

/* Function prototypes */
void hardware_init();
uint8_t current_orientation(uint8_t lastOrientation);
void display_date_and_time(uint8_t currentOrientation, uint8_t displayInvertedStatus);
void update_ADXL_data(uint32_t currentTime, uint32_t* previousTimes, uint8_t* lastOrientation, uint8_t* currentOrientation);
void initialise_current_and_previous_times(uint32_t* currentTime, uint32_t* previousTimes);

int main(void) {
	hardware_init();
	
	RTC_set_time(0x00, 0x00, 0x00); // user needs to be able to change this
	RTC_set_alarm(0x000005);		// User also needs to do this
	RTC_set_date(RTC_WEDNESDAY, 0x15, 0x12, 0x24);
	
	/* Current and previous times used for delays */
	uint32_t currentTime;
	uint32_t previousTimes[NUM_PREVIOUS_TIMES];
	initialise_current_and_previous_times(&currentTime, previousTimes);
	
	/* Initial orientation of the display */
	uint8_t currentOrientation = MODE_A;
	uint8_t lastOrientation = currentOrientation;
	
	uint8_t displayInvertedStatus = DISPLAY_NORMAL;
	
	// temp button for debugging
	DDRC &= ~(1 << 4);
	
	// temp shit
	char humidity[5];
	char temperature[5];
	char secs[3];
	char mins[3];
	char hours[3];
	
    while (1) {
		
		/* Update current system time */
		currentTime = timer0_get_current_time(); 
		
		/* Update the time from the RTC */
		if ((currentTime - previousTimes[RTC_UPDATE_TIME_INDEX]) > RTC_UPDATE_TIME_INTERVAL) {
			RTC_update_current_time();
			previousTimes[RTC_UPDATE_TIME_INDEX] = currentTime;
		}
		
		
		/* Update axis readings from ADXL343 and orientation value */
		update_ADXL_data(currentTime, previousTimes, &lastOrientation, &currentOrientation);
		
		if ((currentTime - previousTimes[AM2320_UPDATE_INDEX]) > AM2320_UPDATE_INTERVAL) {
			AM2320_update_temperature_humidity();
			previousTimes[AM2320_UPDATE_INDEX] = currentTime;
		}
		
		switch(currentOrientation) {
			case MODE_A:
				//display_date_and_time(currentOrientation, displayInvertedStatus);
				
				OLED_clear_buffer();
				//dtostrf(AM2320_get_temperature_float_celsius(), 2, 1, temperature);
				float_to_string(AM2320_get_temperature_float_celsius(), temperature, 1);
				OLED_draw_string(temperature, 0, 0, 8, 1, MODE_A);
				
				//dtostrf(AM2320_get_humidity_float(), 2, 1, humidity);
				float_to_string(AM2320_get_humidity_float(), humidity, 1);
				OLED_draw_string(humidity, 0, 12, 8, 1, MODE_A);
				
				if (!!(PINC & (1<<3)) == 1) {
					OLED_draw_string("PRESSED", 0, 24, 8, 1, 0);
					RTC_alarm_deactivate();
					} else {
					OLED_draw_string("NOT PRESSED", 0, 24, 8, 1, 0);
				}
				itoa(RTC_get_time_sec_int(), secs, 10);
				OLED_draw_string(secs, 30, 36, 8, 1, MODE_A);
				
				itoa(RTC_get_time_min_int(), mins, 10);
				OLED_draw_string(mins, 15, 36, 8, 1, MODE_A);
				
				itoa(RTC_get_time_hour_int(), hours, 10);
				OLED_draw_string(hours, 0, 36, 8, 1, MODE_A);
				
				OLED_draw_rectangle(70, 0, 35, 63,1);
				
				OLED_display_buffer();
				break;
			case MODE_B:
				OLED_clear_buffer();
				OLED_draw_string("Mode B", 0, 0, 8, 2, MODE_B);
				//OLED_draw_string(AM2320_get_temperature_string_celsius(), 0, 12, 8, 1, MODE_B);
				//OLED_draw_string(AM2320_get_humidity_string(), 0, 24, 8, 1, MODE_B);
				OLED_display_buffer();
				break;
			case MODE_C:
				OLED_clear_buffer();
				OLED_draw_string("Mode C", 0, 0, 8, 2, MODE_C);
				OLED_display_buffer();
				break;
			case MODE_D:
				OLED_clear_buffer();
				OLED_draw_string("Mode D", 0, 0, 8, 2, MODE_D);
				OLED_display_buffer();
				break;
		}
		
		/* Check if the alarm is activated and invert the display if necessary */
		if (RTC_alarm_active_check() == RTC_ALARM_ACTIVE) {
			if ((currentTime - previousTimes[INVERT_DISPLAY_ALARM_INDEX]) > INVERT_DISPLAY_ALARM_INTERVAL) {
				displayInvertedStatus ^= 1;
				previousTimes[INVERT_DISPLAY_ALARM_INDEX] = currentTime;
			}
		} else {
			if (displayInvertedStatus == DISPLAY_INVERTED) {
				displayInvertedStatus = DISPLAY_NORMAL;
			}
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
	RTC_alarm_init();			/* Initialise the RTCC alarm */
	USART_init();				/* USART for MCU */
	ADXL343_setup_axis_read();	/* Using i2c mode */
	AM2320_init();				/* Initialise temp and humidity sensor */
	buzzer_init();				/* Initialise the piezo buzzer */
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
uint8_t current_orientation(uint8_t lastOrientation) {
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

/*
* display_date_and_time()
* -----------------------
* Display current data and time on the display when the orientation is
* in MODE_A.
*
* DATE NOT CURRENTLY SUPPORTED!!! Just using a fake date string for now.
*/
void display_date_and_time(uint8_t currentOrientation, uint8_t displayInvertedStatus) {
	char date[9] = {'0', '0', '-', '0', '0', '-', '0', '0', '\0'};
	uint8_t dayDateInt = RTC_get_day_date_int();
	uint8_t monthInt = RTC_get_month_int();
	uint8_t yearInt = RTC_get_year_int();
	char* dayDateString = RTC_get_date_day_string();
	char* monthNumString = RTC_get_month_num_string();
	char* yearString = RTC_get_year_string();
	
	if (currentOrientation == MODE_A) {	
		OLED_clear_buffer();
		
		OLED_draw_string(RTC_get_time_hour_string(), 9, 4, 25, 3, MODE_A); /* Hours */
		OLED_draw_string(RTC_get_time_min_string(), 50, 4, 25, 3, MODE_A); /* Minutes */
		OLED_draw_string(RTC_get_time_sec_string(), 91, 4, 25, 3, MODE_A); /* Seconds */
		
		/* Colons separating the hour, min, sec values */
		OLED_draw_string(":", 41, 9, 16, 1, MODE_A);
		OLED_draw_string(":", 82, 9, 16, 1, MODE_A);		
		
		
		if (dayDateInt < 10) {
			date[1] = dayDateString[0];
			} else {
			date[0] = dayDateString[0];
			date[1] = dayDateString[1];
		}
		
		if (monthInt < 10) {
			date[4] = monthNumString[0];
			} else {
			date[3] = monthNumString[0];
			date[4] = monthNumString[1];
		}
		
		if (yearInt < 10) {
			date[7] = yearString[0];
			} else {
			date[6] = yearString[0];
			date[7] = yearString[1];
		}
		
		OLED_draw_string(date, 5, 41, 16, 2, MODE_A);
		
		
		
		
		/* Boxes surrounding the time and date */
		OLED_draw_horizontal_line(0, 127, 33);
		OLED_draw_horizontal_line(0, 127, 36);
		OLED_draw_rectangle(0, 0, 127, 63, 0);	
		
		OLED_draw_vertical_line(36, 63, 102);
		OLED_draw_xbm(106, 37, alarmBellIconUnarmed, 18, 24, MODE_A);
		
		
		/////////////////////////////////
		if (RTC_alarm_active_check() == RTC_ALARM_ACTIVE) {
			OLED_draw_string("ACTIVE", 0, 0, 8, 1, 0);
			} else {
			OLED_draw_string("INACTIVE", 0, 0, 8, 1, 0);
		}
		
		
		if (!!(PINC & (1<<3)) == 1) {
			OLED_draw_string("PRESSED", 0, 12, 8, 1, 0);
			RTC_alarm_deactivate();
		} else {
			OLED_draw_string("NOT PRESSED", 0, 12, 8, 1, 0);
		}
		///////////////////////////////////
		
		
		if (displayInvertedStatus == DISPLAY_INVERTED) {
			OLED_invert_buffer();
		}
		
		OLED_display_buffer();
	}
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
		*currentOrientation = current_orientation(*lastOrientation); /* Update the orientation of the display */

		previousTimes[ADXL_PREV_TIME_INDEX] = currentTime; /* Update previous time for ADXL to reset delay */
	}
}

/*
* initialise_current_and_previous_times()
* ---------------------------------------
* Initialise the current time and previous times. Just to reduce the amount of code in main function.
*/
void initialise_current_and_previous_times(uint32_t* currentTime, uint32_t* previousTimes) {
	*currentTime = timer0_get_current_time();
	for (uint8_t i = 0; i < NUM_PREVIOUS_TIMES; i++) {
		previousTimes[i] = *currentTime;
	}
}
