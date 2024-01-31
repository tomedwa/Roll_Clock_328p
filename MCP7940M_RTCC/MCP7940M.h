/*
 **************************************************************
 * MCP7940M.h
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

#ifndef MCP7940M_H_
#define MCP7940M_H_

#define RTC_ADDR	0xDE
#define RTC_READ	1
#define RTC_WRITE	0
#define RTC_OSCILLATOR_ENABLE	0x80
#define RTC_SECONDS_REGISTER	0x00
#define RTC_MINUTES_REGISTER	0x01
#define RTC_HOURS_REGISTER		0x02
#define RTC_WEEKDAY_REGISTER	0x03
#define RTC_DATE_DAY_REGISTER	0x04
#define RTC_MONTH_REGISTER		0x05
#define RTC_YEAR_REGISTER		0x06

#define RTC_MONDAY		0x01
#define RTC_TUESDAY		0x02
#define RTC_WEDNESDAY	0x03
#define RTC_THURSDAY	0x04
#define RTC_FRIDAY		0x05
#define RTC_SATURDAY	0x06
#define RTC_SUNDAY		0x07

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

// sec tens, sec ones, min tens, min ones, hour tens, hours ones
extern uint8_t RTC_TIME[6]; 

void RTC_init(); 
uint8_t RTC_read_register(uint8_t regAddr);
void RTC_read_multiple_register(uint8_t startAddr, uint8_t* data, uint8_t numOfReads);
void RTC_write_register(uint8_t regAddr, uint8_t data);
void RTC_set_time(uint8_t hour, uint8_t min, uint8_t sec);
void RTC_update_current_time();

uint8_t RTC_get_time_hour_tens_int();
uint8_t RTC_get_time_hour_ones_int();
uint8_t RTC_get_time_min_tens_int();
uint8_t RTC_get_time_min_ones_int();
uint8_t RTC_get_time_sec_tens_int();
uint8_t RTC_get_time_sec_ones_int();

char* RTC_get_time_hour_string();
char* RTC_get_time_min_string();
char* RTC_get_time_sec_string();

void RTC_set_weekday(uint8_t day);
void RTC_set_date_day(uint8_t dateDay);
void RTC_set_month(uint8_t month);
void RTC_set_year(uint8_t year);

uint8_t RTC_get_weekday_int();
char* RTC_get_weekday_string();
char* RTC_get_date_day_string();
uint8_t RTC_get_month_int();
char* RTC_get_month_num_string();
char* RTC_get_month_name_string();
uint8_t RTC_get_year_int();
char* RTC_get_year_string();

#endif /* MCP7940M_H_ */