/*
 **************************************************************
 * SH1106_OLED.h
 * Author: Tom
 * Date: 10/11/2023
 * AVR Library for 128x64 SH1106 OLED Display Module. This lib
 * requires the Peter Fleury i2cmaster interface.
 * I have created 4 fonts for the screen so far.
 * This was written specifically for the atmega328p MCU. 
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * OLED_init() - Initialise the sh1106.
 * OLED_clear_buffer() - Clear the buffer that stores pixel
 * data for the screen.
 * OLED_display_buffer() - Display the pixel data buffer on the
 * sh1106.
 * OLED_draw_string() - Draw a string in the buffer.
 * OLED_set_pixel() - Set a pixel in the buffer at a position.
 * OLED_draw_xbm() - Draw an XBM image file to the screen.
 * OLED_screen_off()
 * OLED_screen_on()
 * OLED_invert_buffer() - Invert all data in the buffer.
 * OLED_clear_pixel() - clear a pixel in the buffer.
 * OLED_invert_horizontal() - Invert pixels in a horizontal
 * region of the buffer.
 * OLED_invert_vertical() - Invert pixels in a vertical region
 * of the buffer.
 * OLED_invert_rectangle() - Invert pixels on the buffer in a
 * rectangular region.
 * OLED_draw_horizontal_line()
 * OLED_draw_vertical_line()
 * OLED_draw_rectangle()
 * OLED_draw_circle()
 **************************************************************
*/

#ifndef SH1106_H_
#define SH1106_H_

#define OLED_WIDTH	128
#define OLED_HEIGHT	64

#define OLED_CLOCKWISE_0	0
#define OLED_CLOCKWISE_90	1
#define OLED_CLOCKWISE_180	2
#define OLED_CLOCKWISE_270	3

#define OLED_ADDR			0x3C
#define OLED_COMMAND_MODE	0x00
#define OLED_DATA_MODE		0x40

#define OLED_DISPLAY_ON						0xAF
#define OLED_DISPLAY_OFF					0xAE
#define OLED_MUX_RATIO						0xA8
#define OLED_DISPLAY_OFFSET					0xD3
#define OLED_DISPLAY_START_LINE				0x40
#define OLED_SEGMENT_REMAP_NORMAL			0xA0
#define OLED_SEGMENT_REMAP_REVERSE			0xA1
#define OLED_COM_SCAN_DIRECTION_ASCENDING	0xC0
#define OLED_COM_SCAN_DIRECITON_DESCENDING	0xC8
#define OLED_COM_HARDWARE_PIN_CONFIGURATION	0xDA
#define OLED_SET_CONTRAST					0x81
#define OLED_PRE_CHARGE_PERIOD				0xD9
#define OLED_VCOMH_DESELECT_LEVEL			0xDB
#define OLED_ENTIRE_DISPLAY_ON				0xA5
#define OLED_ENTIRE_DISPLAY_NORMAL			0xA4
#define OLED_SET_LOWER_COLUMN_ADDR			0x00
#define OLED_SET_HIGHER_COLUMN_ADDR			0x10
#define OLED_SET_PAGE_ADDR					0xB0

uint8_t OLED_BUFFER[OLED_HEIGHT / 8][OLED_WIDTH]; // Used to store the pixel data for the OLED display.

void OLED_init();
void OLED_single_command(uint8_t command);
void OLED_multiple_command(uint8_t commands[], uint8_t numOfCommands);
void OLED_clear_buffer();
void OLED_send_byte(uint8_t byte);
void OLED_display_buffer();
void OLED_draw_string(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t fontSize, uint8_t characterSpacing, uint8_t screenOrientation);
void OLED_set_pixel(uint8_t xCoordinate, uint8_t yCoordinate);
uint8_t bitread(uint8_t byte, uint8_t bit);
void OLED_xbm_font_8_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation);
void OLED_xbm_font_16_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation);
void OLED_xbm_font_20_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation);
void OLED_xbm_font_25_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation);
void OLED_draw_xbm(uint8_t xPosition, uint8_t yPosition, const uint8_t xbm[], uint8_t width, uint8_t height, uint8_t screenOrientation);
void OLED_screen_off();
void OLED_screen_on();
void OLED_invert_buffer();
void OLED_clear_pixel(uint8_t xCoordinate, uint8_t yCoordinate);
void OLED_invert_horizontal(uint8_t yTop, uint8_t yBottom);
void OLED_invert_vertical(uint8_t xLeft, uint8_t xRight);
void OLED_invert_rectangle(uint8_t xLeft, uint8_t xRight, uint8_t yTop, uint8_t yBottom);
void OLED_draw_horizontal_line(uint8_t xStart, uint8_t xEnd, uint8_t yPosition);
void OLED_draw_vertical_line(uint8_t yStart, uint8_t yEnd, uint8_t xPosition);
void OLED_draw_rectangle(uint8_t xPosition, uint8_t yPosition, uint8_t width, uint8_t height, uint8_t filled);
void OLED_draw_circle(uint8_t xCenter, uint8_t yCenter, uint8_t radius, uint8_t filled);

#endif /* SH1106_H_ */