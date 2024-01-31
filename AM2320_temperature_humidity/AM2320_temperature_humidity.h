/*
 **************************************************************
 * AM2320_temperature_humidity.c
 * Author: Tom
 * Date: 18/01/2023
 * AVR Library for the AM2320 temperature and humidity
 * sensor. This lib requires the Peter Fleury i2cmaster 
 * interface.
 * NOTE: Any variable or function starting with an underscore is
 * private use inside this library.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * AM2320_wake_up() - Wake the AM2320.
 * AM2320_update_temperature_humidity() - Update the current
 * readings for temperature and humidity.
 * AM2320_get_temperature_float_celsius() - Get the current
 * reading for temp as a float in degrees Celsius.
 * AM2320_get_temperature_float_fahrenheit() - Get the current
 * reading for temp as a float in degrees Fahrenheit.
 * AM2320_get_temperature_string_celsius() - Get current reading
 * for temperature as a string in degrees Celsius.
 * AM2320_get_temperature_string_fahrenheit() - Get current
 * reading for temperature as a string in degrees Fahrenheit.
 * AM2320_get_humidity_float() - Get current reading for
 * humidity as a float.
 * AM2320_get_humidity_string() - Get current reading for
 * humidity as a string.
 **************************************************************
*/

#ifndef AM2320_TEMPERATURE_HUMIDITY_H_
#define AM2320_TEMPERATURE_HUMIDITY_H_

/* I2C */
#define AM2320_ADDR			0xB8
#define AM2320_I2C_READ		0x01
#define AM2320_I2C_WRITE	0x00
#define AM2320_I2C_BITRATE	80000L /* 80KHz SCL clock line */

/* AM2320 Commands */
#define AM2320_WAKE_UP_COMMAND				0x00
#define AM2320_COMMAND_READ_REG_DATA		0x03

/* AM2320 Register Addresses */
#define AM2320_HUMIDITY_REG_HIGH			0x00
#define AM2320_HUMIDITY_REG_LOW				0x01
#define AM2320_TEMP_REG_HIGH				0x02
#define AM2320_TEMP_REG_LOW					0x03

/* Index 0 = temperature, 1 = humidity */
extern float _temperatureHumidity[2];

/* Raw data from reading temp and humidity.
Index:
  0 = Function Code.
  1 = Number of Bytes read from registers.
  2 = High byte of Humidity reading. 
  3 = Low byte of Humidity reading.
  4 = High byte of Temperature reading.
  5 = Low byte of Temperature reading.
  6 = Low byte of CRC code.
  7 = High byte of CRC code. */
uint8_t _rawData[8];

/* Store the string representation of temp and humidity readings */
char _temperatureString[6];
char _humidityString[5];

void AM2320_wake_up();
void AM2320_update_temperature_humidity();

float AM2320_get_temperature_float_celsius();
void AM2320_get_temperature_string_celsius(char string[7]);

float AM2320_get_humidity_float();
void AM2320_get_humidity_string(char string[5]);

#endif /* AM2320_TEMPERATURE_HUMIDITY_H_ */