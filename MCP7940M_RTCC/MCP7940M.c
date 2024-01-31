/*
 **************************************************************
 * MCP7940M.c
 * Author: Tom
 * Date: 17/11/2023
 * AVR Library for the real time clock and calendar chip,
 * MCP7940M. This library uses the Peter Fleury i2cmaster.h
 * interface to drive the i2c comms.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * RTC_init() - Initialise the RTC.
 * RTC_set_time() - Set the current time on the RTC.
 * RTC_update_current_time() - Get the current time on the RTC.
 * RTC_get_time_hour_tens_int() - Get hour tens digit as integer.
 * RTC_get_time_hour_ones_int() - Get hour ones digit as integer.
 * RTC_get_time_min_tens_int() - Get min tens digit as integer.
 * RTC_get_time_min_ones_int() - Get min ones digit as integer.
 * RTC_get_time_sec_tens_int() - Get sec tens digit as integer.
 * RTC_get_time_sec_ones_int() - Get sec ones digit as integer.
 * RTC_get_time_hour_string() - Get hour as string.
 * RTC_get_time_min_string() - Get minute as string.
 * RTC_get_time_sec_string() - Get seconds as string.
 * RTC_set_weekday(uint8_t day) - Set weekday on RTCC.
 * RTC_set_date_day(uint8_t dateDay) - Set the date of the day.
 * RTC_set_month(uint8_t month) - Set the month.
 * RTC_set_year(uint8_t year) - Set the year.
 * RTC_get_weekday_int() - Get the current weekday as an integer.
 * RTC_get_weekday_string() - Get the current weekday as a string.
 * RTC_get_date_day_string() - Get the date of the day as a string.
 * RTC_get_month_int() - Get the month as an int.
 * RTC_get_month_num_string() - Get the month number as as string.
 * RTC_get_month_name_string() - Get month name as a string.
 * RTC_get_year_int() - Get the year as an integer.
 * RTC_get_year_string() - Get the year as a string.
 **************************************************************
*/

#include <avr/io.h>
#include <stdio.h>

#include "MCP7940M.h"
#include "../pFleury_i2c_stuff/i2cmaster.h"

uint8_t RTC_TIME[6];

/*
 * RTC_init()
 * -----------
 * Initialise the RTC and enable external oscillator.
*/
void RTC_init() {
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(RTC_SECONDS_REGISTER);
	i2c_write(RTC_OSCILLATOR_ENABLE);
	i2c_stop();
}

/*
 * RTC_read_one_register()
 * -----------------------
 * Internal function to read a single register on the RTC.
*/
uint8_t RTC_read_one_register(uint8_t regAddr) {
	uint8_t data;
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(regAddr);
	i2c_rep_start(RTC_ADDR | RTC_READ);
	data = i2c_readNak();
	i2c_stop();
	return data;
}

/*
 * RTC_read_multiple_register()
 * -----------------------------
 * Internal function to read multiple registers on the RTC. The number of reads
 * required must be known.
*/
void RTC_read_multiple_register(uint8_t startAddr, uint8_t* data, uint8_t numOfReads) {
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(startAddr);
	i2c_stop();
	
	i2c_start(RTC_ADDR | RTC_READ);
	
	for (uint8_t i = 0; i < numOfReads - 1; i++) {
		data[i] = i2c_readAck();
	}
	data[numOfReads - 1] = i2c_readNak();
	i2c_stop();
	
}

/*
 * RTC_write_register()
 * ---------------------
 * Internal function to write a value to a register on the RTC.
*/
void RTC_write_register(uint8_t regAddr, uint8_t data) {
	i2c_start(RTC_ADDR | RTC_WRITE);
	i2c_write(regAddr);
	i2c_write(data);
	i2c_stop();
}

/*
 * RTC_set_time()
 * ---------------
 * Set the time on the RTC. The values must be in hexadecimal form. 
 * e.g. To set the time to 15:45:16, call the function:
 *		RTC_set_time( 0x15, 0x45, 0x16 )
 * As you can see the tens digit is stored in the upper byte and the ones
 * digit in the lower.
*/
void RTC_set_time(uint8_t hour, uint8_t min, uint8_t sec) {
	RTC_write_register(RTC_SECONDS_REGISTER, sec | RTC_OSCILLATOR_ENABLE);
	RTC_write_register(RTC_MINUTES_REGISTER, min);
	RTC_write_register(RTC_HOURS_REGISTER, hour);
}

/*
 * RTC_update_current_time()
 * ----------------------
 * Read the time keeping registers on the RTC and store the values in the RTC_TIME array.
*/
void RTC_update_current_time() {
	uint8_t rawTimeData[] = {0, 0, 0};	// Seconds, minutes, hours (needs to be in this order because of how the registers are set up on the MCP7940M)
	
	RTC_read_multiple_register(RTC_SECONDS_REGISTER, rawTimeData, 3);
	
	RTC_TIME[0] = (rawTimeData[0] & 0x70) >> 4;
	RTC_TIME[1] = (rawTimeData[0] & 0x0F);
	RTC_TIME[2] = (rawTimeData[1] & 0x70) >> 4;
	RTC_TIME[3] = (rawTimeData[1] & 0x0F);
	RTC_TIME[4] = (rawTimeData[2] & 0x30) >> 4;
	RTC_TIME[5] = (rawTimeData[2] & 0x0F);
}

/*
* RTC_get_time_hour_tens_int()
* ----------------------------
* Return last recorded value of the tens digit of the hour.
*/
uint8_t RTC_get_time_hour_tens_int() {
	return RTC_TIME[4];
}

/*
* RTC_get_time_hour_ones_int()
* ----------------------------
* Return last recorded value of the ones digit of the hour.
*/
uint8_t RTC_get_time_hour_ones_int() {
	return RTC_TIME[5];
}

/*
* RTC_get_time_min_tens_int()
* ----------------------------
* Return last recorded value of the tens digit of the minute.
*/
uint8_t RTC_get_time_min_tens_int() {
	return RTC_TIME[2];
}

/*
* RTC_get_time_min_ones_int()
* ----------------------------
* Return last recorded value of the ones digit of the minute.
*/
uint8_t RTC_get_time_min_ones_int() {
	return RTC_TIME[3];
}

/*
* RTC_get_time_sec_tens_int()
* ----------------------------
* Return last recorded value of the tens digit of the second.
*/
uint8_t RTC_get_time_sec_tens_int() {
	return RTC_TIME[0];
}

/*
* RTC_get_time_sec_ones_int()
* ----------------------------
* Return last recorded value of the ones digit of the second.
*/
uint8_t RTC_get_time_sec_ones_int() {
	return RTC_TIME[1];
}

/*
* RTC_get_time_hour_string()
* --------------------------
* Return last recorded value of the hour as a string.
*/
char* RTC_get_time_hour_string() {
	char tempTens[2];
	char tempOnes[2];
	static char tempHour[3];
	
	snprintf(tempTens, sizeof(tempTens), "%d", RTC_TIME[4]);
	snprintf(tempOnes, sizeof(tempOnes), "%d", RTC_TIME[5]);
	
	tempHour[0] = tempTens[0];
	tempHour[1] = tempOnes[0];
	tempHour[2] = '\0';
	
	return tempHour;
}

/*
* RTC_get_time_min_string()
* --------------------------
* Return last recorded value of the minute as a string.
*/
char* RTC_get_time_min_string() {
	char tempTens[2];
	char tempOnes[2];
	static char tempMin[3];
	
	snprintf(tempTens, sizeof(tempTens), "%d", RTC_TIME[2]);
	snprintf(tempOnes, sizeof(tempOnes), "%d", RTC_TIME[3]);
	
	tempMin[0] = tempTens[0];
	tempMin[1] = tempOnes[0];
	tempMin[2] = '\0';
	
	return tempMin;
}

/*
* RTC_get_time_sec_string()
* --------------------------
* Return last recorded value of the second as a string.
*/
char* RTC_get_time_sec_string() {
	char tempTens[2];
	char tempOnes[2];
	static char tempSec[3];
	
	snprintf(tempTens, sizeof(tempTens), "%d", RTC_TIME[0]);
	snprintf(tempOnes, sizeof(tempOnes), "%d", RTC_TIME[1]);
	
	tempSec[0] = tempTens[0];
	tempSec[1] = tempOnes[0];
	tempSec[2] = '\0';
	
	return tempSec;
}

/*
* RTC_set_day()
* -------------
* Set the day on the RTCC. Monday = 1, Tuesday = 2, yada yada, Sunday = 7.
* Only values from 1-7 are valid, any other values for the day will be ignored.
*/
void RTC_set_weekday(uint8_t day) {
	/* Check if the day is valid. (only numbers 1-7) */
	if ((day <= 0) || (day > 7)) {
		return;
	}	
	RTC_write_register(RTC_WEEKDAY_REGISTER, day);
}

/*
* RTC_get_weekday_int()
* ---------------------
* Return the value in the RTCWKDAY (0x03) register as an integer. The value 
* of the register includes a oscillator status bit at bit 5, whereas the day 
* is stored in the first 3 bits. This is why the result is masked to return 
* the first 3 bits.
* 1 = Monday, 2 = Tuesday, ..., 7 = Sunday.
*/
uint8_t RTC_get_weekday_int() {
	/*	Only interested in the first 3 bits */
	return RTC_read_one_register(RTC_WEEKDAY_REGISTER) & 0x07;
}

/*
* RTC_get_weekday_string()
* ------------------------
* Return the current day of the week stored in the RTCC as a string.
*/
char* RTC_get_weekday_string() {
	/*	Only interested in the first 3 bits */
	uint8_t dayInt = RTC_read_one_register(RTC_WEEKDAY_REGISTER) & 0x07;
	
	char* dayString;
	
	switch(dayInt) {
		case RTC_MONDAY: 
			dayString = "Monday";
			break;
		case RTC_TUESDAY:
			dayString = "Tuesday";
			break;
		case RTC_WEDNESDAY:
			dayString = "Wednesday";
			break;
		case RTC_THURSDAY:
			dayString = "Thursday";
			break;
		case RTC_FRIDAY:
			dayString = "Friday";
			break;
		case RTC_SATURDAY:
			dayString = "Saturday";
			break;
		case RTC_SUNDAY:
			dayString = "Sunday";
			break;
		default:
			dayString = "";
			break;
	}
	
	return dayString;
}

/*
* RTC_set_date_day()
* ------------------
* Set the date of the day in a month. The input dateDay needs to be a 
* hex number with the tens digit being a number from 0-3, and the ones
* digit from 0-9. 
* e.g. setting the date to be the 23rd, 
*		RTC_set_date_day( 0x23 );
*/
void RTC_set_date_day(uint8_t dateDay) {

	RTC_write_register(RTC_DATE_DAY_REGISTER, dateDay);
}

/*
* RTC_get_date_day_string()
* -------------------------
* Return the current date on the RTCC as a string. To do this the value
* stored in the RTCDATE register (0x04) must be converted to a decimal value
* where bits 4 and 5 are the tens digit and the lower 4 bits are the ones digit.
* e.g. 0x23 = 23
*/
char* RTC_get_date_day_string() {
	static char dateDayString[3];
	uint8_t dateDayHex = RTC_read_one_register(RTC_DATE_DAY_REGISTER);
	uint8_t dateDayDec = (((dateDayHex & 0x30) >> 4) * 10) + (dateDayHex & 0x0F);
	snprintf(dateDayString, sizeof(dateDayString), "%d", dateDayDec);
	return dateDayString;
}

/*
* RTC_set_month()
* ---------------
* Set the month on the RTCC. The input month needs to be a hex number with the tens
* digit being stored in bit 5 and having a value of 0-1. The ones digit is stored
* in bits 0-4 and has a value from 0-9.
*/
void RTC_set_month(uint8_t month) {
	RTC_write_register(RTC_MONTH_REGISTER, month);
}

/*
* RTC_get_month_int()
* -------------------
* Return the value stored in the RTCMTH register (0x05). The value stored
* in the register is a hex number that represents a decimal number. The tens
* digit is stored in bit 5, and the ones digit is stored in bits 0-4.
* e.g. 0x12 = 12
*/
uint8_t RTC_get_month_int() {
	uint8_t monthHex = RTC_read_one_register(RTC_MONTH_REGISTER) & 0x1F;
	uint8_t monthDec = (((monthHex & 0x10) >> 4) * 10) + (monthHex & 0x0F);
	return monthDec;
}

/*
* RTC_get_month_num_string()
* --------------------------
* Return the number representing the month as a string.
*/
char* RTC_get_month_num_string() {
	static char num[3];
	snprintf(num, sizeof(num), "%d", RTC_get_month_int());
	return num;
}

/*
* RTC_get_month_name_string()
* ---------------------------
* Return the name of the month.
*/
char* RTC_get_month_name_string() {
	uint8_t monthNum = RTC_read_one_register(RTC_MONTH_REGISTER);
	char* monthString;
	
	switch(monthNum) {
		case RTC_JANUARY:
			monthString = "January";
			break;
		case RTC_FEBRUARY:
			monthString = "February";
			break;
		case RTC_MARCH:
			monthString = "March";
			break;
		case RTC_APRIL:
			monthString = "April";
			break;
		case RTC_MAY:
			monthString = "May";
			break;
		case RTC_JUNE:
			monthString = "June";
			break;
		case RTC_JULY:
			monthString = "July";
			break;
		case RTC_AUGUST:
			monthString = "August";
			break;
		case RTC_SEPTEMBER:
			monthString = "September";
			break;
		case RTC_OCTOBER: 
			monthString = "October";
			break;
		case RTC_NOVEMBER:
			monthString = "November";
			break;
		case RTC_DECEMBER:
			monthString = "December";
			break;
		default:
			monthString = "";
			break;
	}
	return monthString;
}

/*
* RTC_set_year()
* --------------
* Set the year on the RTCC. The year must be represented as a hex number
* with the upper 4 bits as the tens digit, and lower 4 bits as the ones
* digit.
*/
void RTC_set_year(uint8_t year) {
	RTC_write_register(RTC_YEAR_REGISTER, year);
}

/*
* RTC_get_year_int()
* ------------------
* Return the current year stored on the RTCC as a decimal number.
*/
uint8_t RTC_get_year_int() {
	uint8_t yearHex = RTC_read_one_register(RTC_YEAR_REGISTER);
	uint8_t yearDec = (((yearHex & 0xF0) >> 4) * 10) + (yearHex & 0x0F);
	return yearDec;
}

/*
* RTC_get_year_string()
* ---------------------
* Return the current year stored on the RTCC as a string.
*/
char* RTC_get_year_string() {
	uint8_t year = RTC_get_year_int();
	static char yearString[3];
	snprintf(yearString, sizeof(yearString), "%d", year);
	return yearString;
}