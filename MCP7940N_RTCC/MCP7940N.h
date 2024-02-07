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
#ifndef MCP7940M_H_
#define MCP7940M_H_

/* I2C */
#define RTC_ADDR		0xDE
#define RTC_I2C_BITRATE 400000L /* 400kHz SCL clock line */
#define RTC_I2C_READ	1
#define RTC_I2C_WRITE	0

/* RTC Commands */
#define RTC_OSCILLATOR_ENABLE	0x80

/* RTC Registers */
#define RTC_SECONDS_REGISTER	0x00
#define RTC_MINUTES_REGISTER	0x01
#define RTC_HOURS_REGISTER		0x02
#define RTC_WEEKDAY_REGISTER	0x03
#define RTC_DATE_DAY_REGISTER	0x04
#define RTC_MONTH_REGISTER		0x05
#define RTC_YEAR_REGISTER		0x06
#define RTC_CONTROL_REGISTER	0x07
#define RTC_ALARM_SECONDS_REGISTER	0x0A
#define RTC_ALARM_WEEKDAY_REGISTER	0x0D

/* Weekdays */
#define RTC_MONDAY		0x01
#define RTC_TUESDAY		0x02
#define RTC_WEDNESDAY	0x03
#define RTC_THURSDAY	0x04
#define RTC_FRIDAY		0x05
#define RTC_SATURDAY	0x06
#define RTC_SUNDAY		0x07

/* Months */
#define RTC_JANUARY		0x01
#define RTC_FEBRUARY	0x02
#define RTC_MARCH		0x03
#define RTC_APRIL		0x04
#define RTC_MAY			0x05
#define RTC_JUNE		0x06
#define RTC_JULY		0x07
#define RTC_AUGUST		0x08
#define RTC_SEPTEMBER	0x09
#define RTC_OCTOBER		0x10
#define RTC_NOVEMBER	0x11
#define RTC_DECEMBER	0x12

#define RTC_ALARM_DISABLED 0x00
#define RTC_ALARM_ENABLED 0x01
#define RTC_ALARM_INACTIVE 0x00
#define RTC_ALARM_ACTIVE 0x01

/* Private functions */
uint8_t _read_register(uint8_t regAddr);
void _read_multiple_registers(uint8_t startAddr, uint8_t* data, uint8_t numOfReads);
void _write_register(uint8_t regAddr, uint8_t data);

/* Initialise */
void RTC_init(); 

/* Time functions */
void RTC_set_time(uint8_t hour, uint8_t min, uint8_t sec);
void RTC_update_current_time();
uint8_t RTC_get_time_seconds_int();
uint8_t RTC_get_time_minutes_int();
uint8_t RTC_get_time_hours_int();
void RTC_get_time_string(char string[9]);

/* Weekday functions */
void RTC_set_weekday(uint8_t day);
uint8_t RTC_get_weekday_int();
const char* RTC_get_weekday_string();

/* Date functions */
void RTC_set_date_day(uint8_t dateDay);
uint8_t RTC_get_date_day_int();
void RTC_get_date_day_string(char string[3]);
void RTC_set_month(uint8_t month);
uint8_t RTC_get_month_int();
void RTC_get_month_num_string(char string[3]);
const char* RTC_get_month_name_string();
void RTC_set_year(uint8_t year);
uint8_t RTC_get_year_int();
void RTC_get_year_string(char string[3]);
void RTC_set_date(uint8_t dayDate, uint8_t month, uint8_t year);
void RTC_alarm_enable_disable(uint8_t value);
void RTC_set_alarm_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
uint8_t RTC_check_alarm_match();
void RTC_alarm_deactivate();
uint32_t RTC_get_alarm_time_hex();
uint32_t RTC_get_current_time_hex();
void RTC_get_date_string(char string[9]);
uint8_t RTC_get_alarm_time_seconds_int();
uint8_t RTC_get_alarm_time_minutes_int();
uint8_t RTC_get_alarm_time_hours_int();
void RTC_get_alarm_time_string(char string[9]);

#endif /* MCP7940M_H_ */