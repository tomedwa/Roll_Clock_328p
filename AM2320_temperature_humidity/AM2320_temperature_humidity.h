/*
 **************************************************************
 * AM2320_temperature_humidity.h
 * Author: Tom
 * Date: 18/01/2023
 * AVR Library for the AM2320 temperature and humidity
 * sensor. This lib requires the Peter Fleury i2cmaster 
 * interface.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * AM2320_init() - Initialise the AM2320.
 * AM2320_update_temperature_humidity() - Update the current
 * readings for temperature and humidity.
 * AM2320_get_temperature_float_celsius() - Get the current 
 * reading for temp as a float in degrees Celsius.
 * AM2320_get_temperature_float_fahrenheit() - Get the current
 * reading for temp as a float in degrees Fahrenheit.
 * AM2320_get_humidity_float() - Get current reading for
 * humidity as a float.
 * AM2320_get_temperature_string_celsius() - Get current reading 
 * for temperature as a string in degrees Celsius.
 * AM2320_get_temperature_string_fahrenheit() - Get current 
 * reading for temperature as a string in degrees Fahrenheit.
 * AM2320_get_humidity_string() - Get current reading for
 * humidity as a string.
 **************************************************************
*/

#ifndef AM2320_TEMPERATURE_HUMIDITY_H_
#define AM2320_TEMPERATURE_HUMIDITY_H_

#define AM2320_ADDR			0xB8
#define AM2320_I2C_READ			0x01
#define AM2320_I2C_WRITE		0x00
#define AM2320_COMMAND_READ_REG_DATA	0x03
#define AM2320_HUMIDITY_REG_HIGH	0x00
#define AM2320_HUMIDITY_REG_LOW		0x01
#define AM2320_TEMP_REG_HIGH		0x02
#define AM2320_TEMP_REG_LOW		0x03

// Index 0 = temperature, 1 = humidity.
float TEMPERATURE_HUMIDITY[2];

void AM2320_init();
void AM2320_update_temperature_humidity();
float AM2320_get_temperature_float_celsius();
float AM2320_get_temperature_float_fahrenheit();
float AM2320_get_humidity_float();
char* AM2320_get_temperature_string_celsius();
char* AM2320_get_temperature_string_fahrenheit();
char* AM2320_get_humidity_string();

#endif /* AM2320_TEMPERATURE_HUMIDITY_H_ */