/*
 **************************************************************
 * ADXL343.h
 * Author: Tom
 * Date: 16/11/2023
 * AVR Library for the ADXL343 Accelerometer. Communication can
 * be done using I2C or SPI. For I2C use the i2cmaster.h
 * library by Peter Fleury. For SPI use Atmega328p_SPI.h by me.
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * ADXL343_setup_axis_read() - Init for x, y, z axis reading.
 * ADXL343_update_axis_readings() - Read data in x, y, z data and
 * store the result in the adxl_axis_readings array.
 * ADXL343_get_x_axis_int() - return integer value of x-axis.
 * ADXL343_get_y_axis_int() - return integer value of y-axis.
 * ADXL343_get_z_axis_int() - return integer value of z-axis.
 * ADXL343_get_x_axis_string() - return string value of x-axis.
 * ADXL343_get_y_axis_string() - return string value of y-axis.
 * ADXL343_get_z_axis_string() - return string value of z-axis.
 **************************************************************
*/

#ifndef ADXL343_H_
#define ADXL343_H_


//#define ADXL343_SPI_MODE // defined = SPI, undefined = I2C

#define I2C_ADDR	0x53 // Alt address pin low.
#define I2C_READ_ADDR	(I2C_ADDR << 1) | 1
#define I2C_WRITE_ADDR	(I2C_ADDR << 1) | 0

#define SPI_READ		(1 << 7)
#define SPI_WRITE		(0 << 7)
#define SPI_MULTIBYTE	(1 << 6)
#define SPI_SINGLEBYTE	(0 << 6)

// define register addresses
#define X_DATA_0	0x32
#define X_DATA_1	0x33
#define Y_DATA_0	0x34
#define Y_DATA_1	0x35
#define Z_DATA_0	0x36
#define Z_DATA_1	0x36
#define BW_RATE		0x2C
#define DATA_FORMAT	0x31
#define POWER_CTL	0x2D
#define FIFO_CTL	0x38

int32_t adxl_axis_readings[3];

void ADXL343_setup_axis_read();
void ADXL343_update_axis_readings();
int32_t ADXL343_get_x_axis_int();
int32_t ADXL343_get_y_axis_int();
int32_t ADXL343_get_z_axis_int();

char* ADXL343_get_x_axis_string();
char* ADXL343_get_y_axis_string();
char* ADXL343_get_z_axis_string();

#endif /* ADXL343_H_ */