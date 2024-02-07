/*
 **************************************************************
 * ADXL343.c
 * Author: Tom
 * Date: 16/11/2023
 * AVR Library for the ADXL343 Accelerometer. Communication can
 * be done using I2C or SPI. For I2C use the i2cmaster.h
 * library by Peter Fleury. For SPI use Atmega328p_SPI.h by me.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * ADXL343_setup_axis_read() - Init for x, y, z axis reading.
 * ADXL_343_update_axis_readings() - Read data in x, y, z data and
 * store the result in the adxl_axis_readings array.
 * ADXL343_get_x_axis_value() - return value of x-axis.
 * ADXL343_get_y_axis_value() - return value of y-axis.
 * ADXL343_get_z_axis_value() - return value of z-axis.
 * ADXL343_get_x_axis_string() - return string value of x-axis.
 * ADXL343_get_y_axis_string() - return string value of y-axis.
 * ADXL343_get_z_axis_string() - return string value of z-axis.
 **************************************************************
*/

#include <avr/io.h>
#include <stdio.h>
#include "ADXL343.h"

/* Current axis readings will be stored here. */
static int32_t _adxl_axis_readings[3];

#ifdef ADXL343_SPI_MODE
	#include "../Atmega328p_SPI/Atmega328p_SPI.h"
#else
	#include "../pFleury_i2c_stuff/i2cmaster.h"
#endif /* ADXL343_SPI_MODE */

/*
 * ADXL343_setup_axis_read()
 * -------------------------
 * External function that initialises the ADXL343 sensor to read data from the x, y, z - axis. The specific
 * initialisation steps depend on the communication protocol defined in the header (SPI or I2C).
*/	
void ADXL343_setup_axis_read() {
	_adxl_axis_readings[0] = 0;
	_adxl_axis_readings[1] = 0;
	_adxl_axis_readings[2] = 0;
	
	#ifdef ADXL343_SPI_MODE
		/* SPI Comms */
		A328p_SPI_transfer_data_to_reg(SPI_WRITE | SPI_SINGLEBYTE | BW_RATE,	 0x0D);
		A328p_SPI_transfer_data_to_reg(SPI_WRITE | SPI_SINGLEBYTE | DATA_FORMAT, 0x07);
		A328p_SPI_transfer_data_to_reg(SPI_WRITE | SPI_SINGLEBYTE | POWER_CTL,	 0x08);
		return;
	#else
		/* I2C Comms */
		i2c_init();
		i2c_set_bitrate(ADXL343_I2C_BITRATE);
		i2c_start_wait(I2C_WRITE_ADDR);
		i2c_write(BW_RATE);
		i2c_write(0x0D);
		i2c_stop();
		
		i2c_start_wait(I2C_WRITE_ADDR);
		i2c_write(POWER_CTL);
		i2c_write(0x08);
		i2c_stop();
		
		i2c_start_wait(I2C_WRITE_ADDR);
		i2c_write(DATA_FORMAT);
		i2c_write(0x07);
		i2c_stop();
		
		i2c_start_wait(I2C_WRITE_ADDR);
		i2c_write(FIFO_CTL);
		i2c_write(0x00);
		i2c_stop();
		return;
	#endif /* ADXL343_SPI_MODE */
}

/*
 * ADXL_343_update_axis_readings()
 * -----------------------------
 * External function that reads the x, y, and z axis data from the ADXL343 registers and stores the results
 * in the adxl_axis_readings array. The specific read procedure depends on the
 * communication protocol defined in the header file (SPI or I2C).
 */
void ADXL343_update_axis_readings() {
	uint8_t z0, z1;
	uint8_t x0, x1;
	uint8_t y0, y1;
	int32_t x, y, z;
	
	#ifdef ADXL343_SPI_MODE
		/* SPI Comms */
		SS_LOW;
		A328p_SPI_send_reg_only(SPI_READ | SPI_MULTIBYTE | X_DATA_0);
		x0 = A328p_SPI_receive_data_only();
		x1 = A328p_SPI_receive_data_only();
		y0 = A328p_SPI_receive_data_only();
		y1 = A328p_SPI_receive_data_only();
		z0 = A328p_SPI_receive_data_only();
		z1 = A328p_SPI_receive_data_only();
		SS_HIGH;
	
		/* Combine all accelerometer data into integers */
		x = (x1 << 8) | x0;
		y = (y1 << 8) | y0;
		z = (z1 << 8) | z0;
	
		_adxl_axis_readings[0] = x;
		_adxl_axis_readings[1] = y;
		_adxl_axis_readings[2] = z;
	
		return;
	#else
		/* I2C Comms */
		i2c_set_bitrate(ADXL343_I2C_BITRATE);
		i2c_start_wait(0xA6);
		i2c_write(X_DATA_0);
		i2c_rep_start(0xA7);
		x0 = i2c_readAck();
		x1 = i2c_readAck();
		y0 = i2c_readAck();
		y1 = i2c_readAck();
		z0 = i2c_readAck();
		z1 = i2c_readNak();
		i2c_stop();
		
		/* Combine all accelerometer data into integers */
 		x = (x1 << 8) | x0;
 		y = (y1 << 8) | y0;
 		z = (z1 << 8) | z0;
		
		_adxl_axis_readings[0] = x;
 		_adxl_axis_readings[1] = y;
 		_adxl_axis_readings[2] = z;
		return;
	#endif /* ADXL343_SPI_MODE */
}

/*
* ADXL343_get_x_axis_int()
* --------------------------
* External function that returns the last recorded value for the 
* x axis as a a base 10 integer.
*/
int32_t ADXL343_get_x_axis_int() {
	return _adxl_axis_readings[0];
}

/*
* ADXL343_get_y_axis_int()
* --------------------------
* External function that returns the last recorded value for the 
* y axis as a base 10 integer.
*/
int32_t ADXL343_get_y_axis_int() {
	return _adxl_axis_readings[1];
}

/*
* ADXL343_get_z_axis_int()
* --------------------------
* External function that returns the last recorded value for the 
* z axis as a base 10 integer.
*/
int32_t ADXL343_get_z_axis_int() {
	return _adxl_axis_readings[2];
}

/*
* ADXL343_get_x_axis_string()
* --------------------------
* External function that populates a given string with the last recorded 
* value for the x axis as a string.
*/
void ADXL343_get_x_axis_string(char string[6]) {
	snprintf(string, sizeof(string), "%ld", _adxl_axis_readings[0]);
}

/*
* ADXL343_get_y_axis_string()
* --------------------------
* External function that populates a given string with the last recorded 
* value for the y axis as a string.
*/
void ADXL343_get_y_axis_string(char string[6]) {
	snprintf(string, sizeof(string), "%ld", _adxl_axis_readings[1]);
}

/*
* ADXL343_get_z_axis_string()
* --------------------------
* External function that populates a given string with the last recorded 
* value for the z axis as a string.
*/
void ADXL343_get_z_axis_string(char string[6]) {
	snprintf(string, sizeof(string), "%ld", _adxl_axis_readings[2]);
}
