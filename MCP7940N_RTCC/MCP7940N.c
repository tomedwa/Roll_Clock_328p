/*
 **************************************************************
 * MCP7940N.c
 * Author: Tom
 * Date: 17/11/2023
 * AVR Library for the real time clock and calendar chip,
 * MCP7940N. This library uses the Peter Fleury i2cmaster.h
 * interface to drive the i2c comms. This library is intended
 * to be used with the Atmega328p MCU, but Im sure it can be
 * modified to be compatible with other MCUs.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * RTC_init() - Initialise the RTC.
 * RTC_set_time() - Set the current time on the RTC.
 * RTC_update_current_time() - Get the current time on the RTC.
 * RTC_get_time_seconds_int() - Get time in seconds as integer.
 * RTC_get_time_minutes_int() - Get time in minutes as integer.
 * RTC_get_time_hours_int() - Get time in hours as integer.
 * RTC_get_time_string() - Get current time formatted as a string.
 * RTC_set_weekday() - Set the weekday on the RTC.
 * RTC_get_weekday_int() - Get current weekday as integer.
 * RTC_get_weekday_string() - Get current weekday as a string.
 * RTC_set_date_day() - Set date of day on RTC.
 * RTC_get_date_day_string() - Get date of day as string.
 * RTC_set_month() - Set current month on RTC.
 * RTC_get_month_int() - Get current month as integer.
 * RTC_get_month_num_string() - Get current month integer as string.
 * RTC_get_month_name_string() - Get current month as string.
 * RTC_set_year() - Set year on RTC.
 * RTC_get_year_int() - Get the year as an integer.
 * RTC_get_year_string() - Get year as a string.
 * RTC_set_date() - Set the current date on the RTC.
 * RTC_alarm_enable_disable() - Enable or disable the alarm.
 * RTC_set_alarm_time() - Set the current alarm time.
 * RTC_check_alarm_match() - Check if its alarm time.
 * RTC_alarm_deactivate() - Deactivate alarm.
 * RTC_get_alarm_time_hex() - Get the alarm time as hex integer.
 * RTC_get_current_time_hex() - Get the current time as hex int.
 * RTC_get_date_string() - Get current date as a formatted string.
 * RTC_get_alarm_time_seconds_int() - Get alarm time seconds as int.
 * RTC_get_alarm_time_minutes_int() - Get alarm time minutes as int.
 * RTC_get_alarm_time_hours_int() - Get alarm time hours as int.
 * RTC_get_alarm_time_string() - Get alarm time as a formatted string.
 **************************************************************
*/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "MCP7940N.h"
#include "../pFleury_i2c_stuff/i2cmaster.h"

static uint8_t _currentTime[6];
static uint8_t _alarmTime[3];
static uint8_t _alarmEnabled;
static uint8_t _alarmStatus;

/*
 * _read_register()
 * -----------------------
 * Internal function to read a single register on the RTC.
*/
uint8_t _read_register(uint8_t regAddr) {
	uint8_t data;
	i2c_set_bitrate(RTC_I2C_BITRATE);
	i2c_start(RTC_ADDR | RTC_I2C_WRITE);
	i2c_write(regAddr);
	i2c_rep_start(RTC_ADDR | RTC_I2C_READ);
	data = i2c_readNak();
	i2c_stop();
	return data;
}

/*
 * _read_multiple_registers()
 * -----------------------------
 * Internal function to read multiple registers on the RTC. The number of reads
 * required must be known.
*/
void _read_multiple_registers(uint8_t startAddr, uint8_t* data, uint8_t numOfReads) {
	i2c_set_bitrate(RTC_I2C_BITRATE);
	i2c_start(RTC_ADDR | RTC_I2C_WRITE);
	i2c_write(startAddr);
	i2c_stop();
	
	i2c_start(RTC_ADDR | RTC_I2C_READ);
	
	for (uint8_t i = 0; i < numOfReads - 1; i++) {
		data[i] = i2c_readAck();
	}
	data[numOfReads - 1] = i2c_readNak();
	i2c_stop();
	
}

/*
 * _write_register()
 * ---------------------
 * Internal function to write a value to a register on the RTC.
*/
void _write_register(uint8_t regAddr, uint8_t data) {
	i2c_set_bitrate(RTC_I2C_BITRATE);
	i2c_start(RTC_ADDR | RTC_I2C_WRITE);
	i2c_write(regAddr);
	i2c_write(data);
	i2c_stop();
}

/*
 * RTC_init()
 * -----------
 * External function to initialise the RTC and enable external oscillator.
*/
void RTC_init() {
	i2c_init();
	i2c_set_bitrate(RTC_I2C_BITRATE);
	i2c_start(RTC_ADDR | RTC_I2C_WRITE);
	i2c_write(RTC_SECONDS_REGISTER);
	i2c_write(RTC_OSCILLATOR_ENABLE);
	i2c_stop();
	
	_alarmEnabled = RTC_ALARM_DISABLED;
	_alarmStatus = RTC_ALARM_INACTIVE;
	
	sei();
		
}

/*
 * RTC_set_time()
 * ---------------
 * External function to set the time on the RTC. The values must be in hexadecimal form. 
 * e.g. To set the time to 15:45:16, call the function:
 *		RTC_set_time( 0x15, 0x45, 0x16 )
 * As you can see the tens digit is stored in the upper byte and the ones
 * digit in the lower.
*/
void RTC_set_time(uint8_t hour, uint8_t min, uint8_t sec) {
	_write_register(RTC_SECONDS_REGISTER, sec | RTC_OSCILLATOR_ENABLE);
	_write_register(RTC_MINUTES_REGISTER, min);
	_write_register(RTC_HOURS_REGISTER, hour);
}

/*
 * RTC_update_current_time()
 * ----------------------
 * External function to read the time keeping registers on the RTC and store the values in the RTC_TIME array.
*/
void RTC_update_current_time() {
	uint8_t rawTimeData[] = {0, 0, 0};
	
	_read_multiple_registers(RTC_SECONDS_REGISTER, rawTimeData, 3);
	
	_currentTime[0] = (rawTimeData[0] & 0x70) >> 4;
	_currentTime[1] = (rawTimeData[0] & 0x0F);
	_currentTime[2] = (rawTimeData[1] & 0x70) >> 4;
	_currentTime[3] = (rawTimeData[1] & 0x0F);
	_currentTime[4] = (rawTimeData[2] & 0x30) >> 4;
	_currentTime[5] = (rawTimeData[2] & 0x0F);
}

/*
* RTC_get_current_time_seconds_int()
* -----------------------------------
* External function that returns the last recorded time in seconds as 
* a base 10 integer.
*/
uint8_t RTC_get_time_seconds_int() {
	return ( _currentTime[0] * 10 ) + _currentTime[1];
}

/*
* RTC_get_current_time_minutes_int()
* -----------------------------------
* External function that returns the last recorded time in minutes as
* a base 10 integer.
*/
uint8_t RTC_get_time_minutes_int() {
	return ( _currentTime[2] * 10 ) + _currentTime[3];
}

/*
* RTC_get_current_time_hours_int()
* -----------------------------------
* External function that returns the last recorded time in hours as
* a base 10 integer.
*/
uint8_t RTC_get_time_hours_int() {
	return ( _currentTime[4] * 10 ) + _currentTime[5];
}

/*
* RTC_get_time_string()
* ----------------------
* External function that populates a given string with the current
* time formatted as: HH:MM:SS
*/
void RTC_get_time_string(char string[9]) {
	string[0] = _currentTime[4] + 48;
	string[1] = _currentTime[5] + 48;
	string[2] = ':';
	string[3] = _currentTime[2] + 48;
	string[4] = _currentTime[3] + 48;
	string[5] = ':';
	string[6] = _currentTime[0] + 48;
	string[7] = _currentTime[1] + 48;
	string[8] = '\0';
}

/*
* RTC_set_weekday()
* -------------
* External function to set the day on the RTCC. Monday = 1, 
* Tuesday = 2, yada yada, Sunday = 7. Only values from 1-7 
* are valid, any other values for the day will be ignored.
*/
void RTC_set_weekday(uint8_t day) {
	/* Check if the day is valid. (only numbers 1-7) */
	if ((day <= 0) || (day > 7)) {
		return;
	}
	_write_register(RTC_WEEKDAY_REGISTER, day);
}

/*
* RTC_get_weekday_int()
* ---------------------
* External function to return the value in the RTCWKDAY (0x03) 
* register as an integer. The value of the register includes 
* a oscillator status bit at bit 5, whereas the day is stored 
* in the first 3 bits. This is why the result is masked to return
* the first 3 bits.
* 1 = Monday, 2 = Tuesday, ..., 7 = Sunday.
*/
uint8_t RTC_get_weekday_int() {
	/*	Only interested in the first 3 bits */
	return _read_register(RTC_WEEKDAY_REGISTER) & 0x07;
}

/*
* RTC_get_weekday_string()
* ------------------------
* External function that returns the current day of the 
* week stored in the RTCC as a string.
*/
const char* RTC_get_weekday_string() {
	uint8_t dayInt = RTC_get_weekday_int();
	
	static const char* dayStrings[] = {
		"Error",    /* 0 is an invalid day */
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		"Sunday"
	};
	
	if (dayInt >= 1 && dayInt <= 7) {
		return dayStrings[dayInt];
	} else {
		return dayStrings[0]; /* Return "Error" for invalid day */
	}
}

/*
* RTC_set_date_day()
* ------------------
* External function to set the date of the day in a month. 
* The input dateDay needs to be a hex number with the tens digit 
* being a number from 0-3, and the ones digit from 0-9.
* e.g. setting the date to be the 23rd,
*		RTC_set_date_day( 0x23 );
*/
void RTC_set_date_day(uint8_t dateDay) {
	_write_register(RTC_DATE_DAY_REGISTER, dateDay);
}

/*
* RTC_get_day_date_int()
* ----------------------
* External function that returns the current date on the RTCC 
* as a base 10 integer.
*/
uint8_t RTC_get_date_day_int() {
	uint8_t dateDayHex = _read_register(RTC_DATE_DAY_REGISTER);
	uint8_t dateDayDec = (((dateDayHex & 0x30) >> 4) * 10) + (dateDayHex & 0x0F);
	return dateDayDec;
}

/*
* RTC_get_date_day_string()
* -------------------------
* External function to populate a given string with the current date on the RTCC.
*/
void RTC_get_date_day_string(char string[3]) {
	uint8_t dateDayDec = RTC_get_date_day_int();
	if (dateDayDec < 10) {
		string[0] = '0';
		string[1] = dateDayDec + 48;
		string[2] = '\0';
	} else {
		string[0] = (dateDayDec / 10) + 48;
		string[1] = (dateDayDec % 10) + 48;
		string[2] = '\0';
	}
}

/*
* RTC_set_month()
* ---------------
* External functions to set the month on the RTCC. The input 
* month needs to be a hex number with the tens digit being 
* stored in bit 5 and having a value of 0-1. The ones digit 
* is stored in bits 0-4 and has a value from 0-9.
* e.g. To set the month to be September:
*		RTC_set_month(0x09);
* e.g. December:
*		RTC_set_month(0x12);
*/
void RTC_set_month(uint8_t month) {
	_write_register(RTC_MONTH_REGISTER, month);
}

/*
* RTC_get_month_int()
* -------------------
* External function to return the value stored in the 
* RTCMTH register (0x05). The value stored in the register 
* is a hex number that represents a decimal number. The tens
* digit is stored in bit 5, and the ones digit is stored in 
* bits 0-4.
* e.g. 0x12 = 12
*/
uint8_t RTC_get_month_int() {
	uint8_t monthHex = _read_register(RTC_MONTH_REGISTER) & 0x1F;
	uint8_t monthDec = (((monthHex & 0x10) >> 4) * 10) + (monthHex & 0x0F);
	return monthDec;
}

/*
* RTC_get_month_num_string()
* --------------------------
* External function to populate a given string with the number 
* representing the month as a string.
*/
void RTC_get_month_num_string(char string[3]) {
	uint8_t monthNum = RTC_get_month_int();
	
	if (monthNum < 10) {
		string[0] = '0';
		string[1] = monthNum + 48;
		string[2] = '\0';
	} else {
		string[0] = (monthNum / 10) + 48;
		string[1] = (monthNum % 10) + 48;
		string[2] = '\0';
	}
}

/*
* RTC_get_month_name_string()
* ---------------------------
* External function to return the name of the month as string.
*/
const char* RTC_get_month_name_string() {
	uint8_t monthNum = _read_register(RTC_MONTH_REGISTER);
	const char* monthString;
	
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
* External function to set the year on the RTCC. 
* The year must be represented as a hex number
* with the upper 4 bits as the tens digit, and 
* lower 4 bits as the ones digit.
* e.g. To set year to 98:
*	RTC_set_year(0x98);
*/
void RTC_set_year(uint8_t year) {
	_write_register(RTC_YEAR_REGISTER, year);
}

/*
* RTC_get_year_int()
* ------------------
* External function to return the current year stored 
* on the RTCC as a base 10 integer.
*/
uint8_t RTC_get_year_int() {
	uint8_t yearHex = _read_register(RTC_YEAR_REGISTER);
	uint8_t yearDec = (((yearHex & 0xF0) >> 4) * 10) + (yearHex & 0x0F);
	return yearDec;
}

/*
* RTC_get_year_string()
* ---------------------
* External function to populate a given string with the 
* current year stored on the RTCC.
*/
void RTC_get_year_string(char string[3]) {
	uint8_t year = RTC_get_year_int();
	
	if (year < 10) {
		string[0] = '0';
		string[1] = year + 48;
		string[2] = '\0';
	} else {
		string[0] = (year / 10) + 48;
		string[1] = (year % 10) + 48;
		string[2] = '\0';
	}
}

/*
* RTC_set_date()
* --------------
* External function to set the date on the RTCC.
*/
void RTC_set_date(uint8_t dayDate, uint8_t month, uint8_t year) {
	RTC_set_date_day(dayDate);
	RTC_set_month(month);
	RTC_set_year(year);
}

/*
* RTC_alarm_enable_disable()
* --------------------------
* External function to enable and disable the alarm funcitonality.
*/
void RTC_alarm_enable_disable(uint8_t value) {
	_alarmEnabled = value;
	if (value == RTC_ALARM_DISABLED) {
		_alarmStatus = RTC_ALARM_INACTIVE;
	}
}

/*
* RTC_set_alarm_time()
* --------------------
* External function to set the alarm time. The alarm time input must be 
* hex numbers representing base 10 integers. 
* e.g. 12 = 0x12
* e.g. Set the alarm time to 21:20:33
*		RTC_set_alarm_time(0x21, 0x20, 0x33);
*/
void RTC_set_alarm_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	_alarmTime[0] = (((seconds & 0xF0) >> 4) * 10) + (seconds & 0x0F);
	_alarmTime[1] = (((minutes & 0xF0) >> 4) * 10) + (minutes & 0x0F);
	_alarmTime[2] = (((hours & 0xF0) >> 4) * 10) + (hours & 0x0F);
}

/*
* RTC_get_alarm_time_seconds_int()
* ---------------------------------
* External function to return the alarm time in seconds
* as a base 10 integer.
*/
uint8_t RTC_get_alarm_time_seconds_int() {
	return _alarmTime[0];
}

/*
* RTC_get_alarm_time_minutes_int()
* ---------------------------------
* External function to return the alarm time in seconds
* as a base 10 integer.
*/
uint8_t RTC_get_alarm_time_minutes_int() {
	return _alarmTime[1];
}

/*
* RTC_get_alarm_time_hours_int()
* ---------------------------------
* External function to return the alarm time in hours
* as a base 10 integer.
*/
uint8_t RTC_get_alarm_time_hours_int() {
	return _alarmTime[2];
}

/*
* RTC_check_alarm_match()
* -----------------------
* External function to check if the current alarm time 
* matches with the current time.
*/
uint8_t RTC_check_alarm_match() {
	if ((_alarmEnabled == RTC_ALARM_ENABLED) && (_alarmStatus == RTC_ALARM_INACTIVE)) {
		if ((RTC_get_time_seconds_int() == RTC_get_alarm_time_seconds_int()) &&
			(RTC_get_time_minutes_int() == RTC_get_alarm_time_minutes_int()) &&
			(RTC_get_time_hours_int() == RTC_get_alarm_time_hours_int())) {
				_alarmStatus = RTC_ALARM_ACTIVE;
		}
	}
	return _alarmStatus;
}

/*
* RTC_alarm_deactivate()
* ----------------------
* External function to deactivate the alarm.
*/
void RTC_alarm_deactivate() {
	_alarmStatus = RTC_ALARM_INACTIVE;
}

/*
* RTC_get_date_string()
* ---------------------
* External function to populate a given string with the
* current date formatted as: DD-MM-YY
*/
void RTC_get_date_string(char string[9]) {
	uint8_t dayDate = RTC_get_date_day_int();
	uint8_t month = RTC_get_month_int();
	uint8_t year = RTC_get_year_int();
	
	string[0] = (dayDate / 10) + 48;
	string[1] = (dayDate % 10) + 48;
	string[2] = '-';
	string[3] = (month / 10) + 48;
	string[4] = (month % 10) + 48;
	string[5] = '-';
	string[6] = (year / 10) + 48;
	string[7] = (year % 10) + 48;
	string[8] = '\0';
}

/*
* RTC_get_alarm_time_string()
* ---------------------------
* External function to populate a string with the current
* alarm time formatted as: HH:MM:SS
*/
void RTC_get_alarm_time_string(char string[9]) {
	string[0] = (_alarmTime[2] / 10) + 48;
	string[1] = (_alarmTime[2] % 10) + 48;
	string[2] = ':';
	string[3] = (_alarmTime[1] / 10) + 48;
	string[4] = (_alarmTime[1] % 10) + 48;
	string[5] = ':';
	string[6] = (_alarmTime[0] / 10) + 48;
	string[7] = (_alarmTime[0] % 10) + 48;
	string[8] = '\0';
}