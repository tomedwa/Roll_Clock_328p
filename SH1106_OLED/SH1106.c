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

#include <avr/io.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "SH1106.h"
#include "../pFleury_i2c_stuff/i2cmaster.h"
#include "XBM_fonts/XBM_FONT_8.h"
#include "XBM_fonts/XBM_FONT_16.h"
#include "XBM_fonts/XBM_FONT_NUMBERS_20.h"
#include "XBM_fonts/XBM_FONT_NUMBERS_25.h"

/*
*	OLED_init()
*	-----------
*	Initialise the OLED display by sending a series of initialisation commands to the screen.
*/
void OLED_init() {
	i2c_init();
	uint8_t initCommands[] = {
		OLED_DISPLAY_OFF,
		OLED_MUX_RATIO, 0x3F,						// Multiplex ratio 1/64 duty cycle
		OLED_DISPLAY_OFFSET, 0x00,					// No offset
		OLED_DISPLAY_START_LINE | 0x00,				// Start at the first row of the display
		OLED_SEGMENT_REMAP_REVERSE,
		OLED_COM_SCAN_DIRECITON_DESCENDING,
		OLED_COM_HARDWARE_PIN_CONFIGURATION, 0x12,	// Alternative COM pin configuration
		OLED_SET_CONTRAST, 0xFF,					// Highest contrast
		OLED_PRE_CHARGE_PERIOD, 0xF1,				// Set pre-charge period, Phase 1 period = 15, Phase 2 period = 1
		OLED_VCOMH_DESELECT_LEVEL, 0x40,			// Set VCOMH deselect level, VCOMH deselect level = 0.77 * VCC
		OLED_ENTIRE_DISPLAY_NORMAL,					// Show all pixels not just the ones that are on
		OLED_NORMAL_DISPLAY_COMMAND,
		OLED_DISPLAY_ON	
	};
	OLED_multiple_command(initCommands, 19);
}

void OLED_display_invert(uint8_t invert) {
	switch(invert) {
		case 0:
			OLED_single_command(OLED_NORMAL_DISPLAY_COMMAND);
			break;
		case 1:
			OLED_single_command(OLED_INVERT_DISPLAY_COMMAND);
			break;
	}
}

/*	
*	OLED_single_command()
*	---------------------
*	Send a single command to the OLED display using i2c.
*
*	command: Command to be sent to the OLED display.
*
*	NOTE:	The command must be valid as there are no checks
			for an invalid command.
*/ 
void OLED_single_command(uint8_t command) {
	i2c_set_bitrate(OLED_I2C_BITRATE);
	i2c_start_wait(OLED_ADDR << 1);	
	i2c_write(OLED_COMMAND_MODE);	
	i2c_write(command);
	i2c_stop();
}

/*	
*	OLED_multiple_command()
*	-----------------------
*	Send multiple commands to the OLED display using i2c.
*
*	commands:	An array of commands that will be sent to the OLED display.
*				The commands will be sent in the same order that they appear
*				in the list.
*
*	numOfCommands:	The number of commands in the 'commands' array.
*	
*	NOTE:	The commands in the array must be valid as there are no checks
			for invalid commands.
*/
void OLED_multiple_command(uint8_t commands[], uint8_t numOfCommands) {
	i2c_set_bitrate(OLED_I2C_BITRATE);
	i2c_start_wait(OLED_ADDR << 1);	
	i2c_write(OLED_COMMAND_MODE);	
	for (uint8_t i = 0; i < numOfCommands; i++) {
		i2c_write(commands[i]);
	}
	i2c_stop();
}

/*
*	OLED_clear_buffer()
*	-------------------
*	Clear the OLED display buffer by setting all the elements of the array to 0x00.
*/
void OLED_clear_buffer() {
	for (uint8_t page = 0; page < OLED_HEIGHT / 8; page++) {
		for (uint8_t column = 0; column < OLED_WIDTH; column++) {
			OLED_BUFFER[page][column] = 0x00;
		}
	}
}

/*
*	OLED_send_byte()
*	----------------
*	Send a single byte of data to be displayed on the OLED screen.
*
*	byte: The byte of data to be sent to the OLED display.
*/
void OLED_send_byte(uint8_t byte) {
	i2c_set_bitrate(OLED_I2C_BITRATE);
	i2c_start_wait(OLED_ADDR << 1);
	i2c_write(OLED_DATA_MODE);		
	i2c_write(byte);
	i2c_stop();
}

/*
*	OLED_display_buffer()
*	---------------------
*	Display the content of the buffer on the OLED display.
*/
void OLED_display_buffer() {
	uint8_t horizontalOffset = 2;	// The horizontal offset will vary depending on the hardware
	
	for (uint8_t page = 0; page < OLED_HEIGHT / 8; page++) {
		OLED_single_command(OLED_SET_PAGE_ADDR + page);	// Set page address
		for (uint8_t column = horizontalOffset; column < OLED_WIDTH + horizontalOffset; column++) {
			OLED_single_command(OLED_SET_LOWER_COLUMN_ADDR + (column & 0x0F));
			OLED_single_command(OLED_SET_HIGHER_COLUMN_ADDR + ((column >> 4) & 0x0F));
			OLED_send_byte(OLED_BUFFER[page][column - horizontalOffset]);
		}
	}
}

/*
*	OLED_draw_string()
*	------------------
*	Draw a string on the buffer in the correct position and orientation.
*
*	string: The string to be represented in the buffer.
*
*	xPosition:	The x-coordinate of the starting pixel of the string.
*
*	yPosition:	The y-coordinate of the starting pixel of the string.
*
*	fontSize: The desired font size (Either 8 or 16 so far).
*
*	characterSpacing: The desired number of pixels between the characters stored in the buffer.
*
*	screenOrientation: The desired orientation of the screen:
*				0 = No rotation (normal orientation).
*				1 = 90 degrees clockwise rotation.
*				2 = 180 degrees clockwise rotation (upside down).
*				3 = 270 degrees clockwise rotation.
*
*	NOTE: 
*			- Not all characters can be represented in the buffer, only those defined in the font file.
*			- No checks for valid x and y coordinates.
*/
void OLED_draw_string(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t fontSize, uint8_t characterSpacing, uint8_t screenOrientation) {
	if (fontSize == 8) {
		OLED_xbm_font_8_to_buffer(string, xPosition, yPosition, characterSpacing, screenOrientation);
	} else if (fontSize == 16) {
		OLED_xbm_font_16_to_buffer(string, xPosition, yPosition, characterSpacing, screenOrientation);
	} else if (fontSize == 20) {
		OLED_xbm_font_20_to_buffer(string, xPosition, yPosition, characterSpacing, screenOrientation);
	} else if (fontSize == 25) {
		OLED_xbm_font_25_to_buffer(string, xPosition, yPosition, characterSpacing, screenOrientation);
	}
}

/*
*	OLED_set_pixel()
*	----------------
*	Sets a single pixel on the display buffer at the specified position.
*
*	xCoordinate:	The x-coordinate of the pixel.
*
*	yCoordinate:	The y-coordinate of the pixel.
*
*	NOTE: No checks for valid x and y coordinates.
*/
void OLED_set_pixel(uint8_t xCoordinate, uint8_t yCoordinate) {
	uint8_t page = yCoordinate / 8;
	uint8_t column = xCoordinate;
	uint8_t value = 1 << (yCoordinate - (yCoordinate / 8) * 8);
	OLED_BUFFER[page][column] |= value;
}

/*
*	bitread()
*	---------
*	Reads the specific value bit in a given byte and returns the bit.
*
*	byte:	The byte from which to read the bit.
*
*	bit:	The position of the bit to read.
*
*	Return:	The value of the specific bit (0 or 1)
*
*	REF:	This function is taken directly from an arduino library, do a quick google search and you will find it.
*/
uint8_t bitread(uint8_t byte, uint8_t bit) {
	if (bit > 7) {
		return 0;
	}
	
	uint8_t result = 0x00;
	
	result |= (byte & (1 << bit)) >> bit;
	return result;
}

/*
*	OLED_xbm_font_8_to_buffer()
*	---------------------------
*	Draw a string in the buffer using an xbm font 8 pixels high and various widths. This function is
*	designed to use the font designed in XBM_FONT_8.h
*
*	string: The string to be represented in the buffer.
*
*	xPosition:	The x-coordinate of the starting pixel of the string.
*
*	yPosition:	The y-coordinate of the starting pixel of the string.
*
*	characterSpacing: The desired number of pixels between the characters stored in the buffer.
*	
*	screenOrientation: The desired orientation of the string:
*				0 = No rotation (normal orientation).
*				1 = 90 degrees clockwise rotation.
*				2 = 180 degrees clockwise rotation (upside down).
*				3 = 270 degrees clockwise rotation.
*
*	NOTE: 
*			- No checks for valid x and y coordinates.
*			- XBM_FONT_8.h must be included
*/
void OLED_xbm_font_8_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation) {
	uint8_t indexNumSize = (uint8_t)strlen(string);
	uint8_t indexNums[indexNumSize];
	uint8_t cursorPosition = xPosition;
	
	// This loop is used to map the index of each character in the string to the corresponding entry in the font array.
	for (uint8_t i = 0; i < indexNumSize; i++) {
		if ((string[i] >= 'A') && (string[i] <= 'Z')) {
			indexNums[i] = (uint8_t)(string[i] - 65);
		} else if ((string[i] >= 'a') && (string[i] <= 'z')) {
			indexNums[i] = (uint8_t)(string[i] - 71);
		} else if ((string[i] >= '0') && (string[i] <= '9')) {
			indexNums[i] = (uint8_t)(string[i] + 4);
		} else if ((string[i] >= '!') && (string[i] <= '%')) {
			indexNums[i] = (uint8_t)(string[i] + 29);
		} else if ((string[i] >= 39) && (string[i] <= ')')) {
			indexNums[i] = (uint8_t)(string[i] + 28);
		} else if ((string[i] >= '+') && (string[i] <= '/')) {
			indexNums[i] = (uint8_t)(string[i] + 27);
		} else if ((string[i] >= ':') && (string[i] <= ';')) {
			indexNums[i] = (uint8_t)(string[i] + 17);
		} else if (string[i] == '=') {
			indexNums[i] = 77;
		} else if (string[i] == '?') {
			indexNums[i] = 78;
		} else if (string[i] == 254) {
			indexNums[i] = 79;
		} else if (string[i] == 255) {
			indexNums[i] = 80;
		}
	}
	
	for (uint8_t i = 0; i < indexNumSize; i++) {
		
		if (string[i] == ' ') {
			cursorPosition += 5;
			continue;
		}
		
		uint8_t* word = (uint8_t*)pgm_read_word(&(FONT_8[indexNums[i]]));
		uint8_t characterWidth = pgm_read_byte(&(FONT_8_WIDTHS[indexNums[i]]));
		uint8_t characterOffset = pgm_read_byte(&(FONT_8_Y_OFFSET[indexNums[i]]));
		
		OLED_draw_xbm(cursorPosition, yPosition + characterOffset, word, characterWidth, 8, screenOrientation);
		cursorPosition += characterWidth + characterSpacing;
	}
}

/*
*	OLED_xbm_font_16_to_buffer()
*	---------------------------
*	Draw a string in the buffer using an xbm font 16 pixels high and various widths. This function is
*	designed to use the font designed in XBM_FONT_16.h
*
*	string: The string to be represented in the buffer.
*
*	xPosition:	The x-coordinate of the starting pixel of the image.
*
*	yPosition:	The y-coordinate of the starting pixel of the image.
*
*	characterSpacing: The desired number of pixels between the characters stored in the buffer.
*
*	screenOrientation: The desired orientation of the string:
*				0 = No rotation (normal orientation).
*				1 = 90 degrees clockwise rotation.
*				2 = 180 degrees clockwise rotation (upside down).
*				3 = 270 degrees clockwise rotation.
*
*	NOTE:
*			- No checks for valid x and y coordinates.
*			- XBM_FONT_16.h must be included
*/
void OLED_xbm_font_16_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation) {
	uint8_t indexNumSize = (uint8_t)strlen(string);
	uint8_t indexNums[indexNumSize];
	uint8_t cursorPosition = xPosition;
	
	// This loop is used to map the index of each character in the string to the corresponding entry in the font array.
	for (uint8_t i = 0; i < indexNumSize; i++) {
		if ((string[i] >= 'A') && (string[i] <= 'Z')) {
			indexNums[i] = (uint8_t)(string[i] - 65);
		} else if ((string[i] >= 'a') && (string[i] <= 'z')) {
			indexNums[i] = (uint8_t)(string[i] - 71);
		} else if ((string[i] >= '0') && (string[i] <= '9')) {
			indexNums[i] = (uint8_t)(string[i] + 4);
		} else if ((string[i] >= '!') && (string[i] <= '%')) {
			indexNums[i] = (uint8_t)(string[i] + 29);
		} else if ((string[i] >= 39) && (string[i] <= ')')) {
			indexNums[i] = (uint8_t)(string[i] + 28);
		} else if ((string[i] >= '+') && (string[i] <= '/')) {
			indexNums[i] = (uint8_t)(string[i] + 27);
		} else if ((string[i] >= ':') && (string[i] <= ';')) {
			indexNums[i] = (uint8_t)(string[i] + 17);
		} else if (string[i] == '=') {
			indexNums[i] = 77;
		} else if (string[i] == '?') {
			indexNums[i] = 78;
		} else if (string[i] == 254) {
			indexNums[i] = 79;
		} else if (string[i] == 255) {
			indexNums[i] = 80;
	}
	}
	
	for (uint8_t i = 0; i < indexNumSize; i++) {
		
		if (string[i] == ' ') {
			cursorPosition += 10;
			continue;
		}
		
		uint8_t* word = (uint8_t*)pgm_read_word(&(FONT_16[indexNums[i]]));
		uint8_t characterWidth = pgm_read_byte(&(FONT_16_WIDTHS[indexNums[i]]));
		uint8_t characterOffset = pgm_read_byte(&(FONT_16_Y_OFFSET[indexNums[i]]));
		
		OLED_draw_xbm(cursorPosition, yPosition + characterOffset, word, characterWidth, 16, screenOrientation);
		cursorPosition += characterWidth + characterSpacing;
	}
}

void OLED_xbm_font_20_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation) {
	uint8_t indexNumSize = (uint8_t)strlen(string);
	uint8_t indexNums[indexNumSize];
	uint8_t cursorPosition = xPosition;
	
	// This loop is used to map the index of each character in the string to the corresponding entry in the font array.
	for (uint8_t i = 0; i < indexNumSize; i++) {
		if ((string[i] >= '0') && (string[i] <= '9')) {
			indexNums[i] = (uint8_t)(string[i] - 48);
		}
	}
	
	for (uint8_t i = 0; i < indexNumSize; i++) {
		
		if (string[i] == ' ') {
			cursorPosition += 14;
			continue;
		}
		
		uint8_t* word = (uint8_t*)pgm_read_word(&(FONT_20[indexNums[i]]));
		uint8_t characterWidth = pgm_read_byte(&(FONT_20_WIDTHS[indexNums[i]]));
		uint8_t characterOffset = pgm_read_byte(&(FONT_20_Y_OFFSET[indexNums[i]]));
		
		OLED_draw_xbm(cursorPosition, yPosition + characterOffset, word, characterWidth, 20, screenOrientation);
		cursorPosition += characterWidth + characterSpacing;
	}
}

void OLED_xbm_font_25_to_buffer(char* string, uint8_t xPosition, uint8_t yPosition, uint8_t characterSpacing, uint8_t screenOrientation) {
	uint8_t indexNumSize = (uint8_t)strlen(string);
	uint8_t indexNums[indexNumSize];
	uint8_t cursorPosition = xPosition;
	
	// This loop is used to map the index of each character in the string to the corresponding entry in the font array.
	for (uint8_t i = 0; i < indexNumSize; i++) {
		if ((string[i] >= '0') && (string[i] <= '9')) {
			indexNums[i] = (uint8_t)(string[i] - 48);
		} else if (string[i] == ':') {
			indexNums[i] = 10;
		}
	}
	
	for (uint8_t i = 0; i < indexNumSize; i++) {
		
		if (string[i] == ' ') {
			cursorPosition += 14;
			continue;
		}
		
		uint8_t* word = (uint8_t*)pgm_read_word(&(FONT_25[indexNums[i]]));
		uint8_t characterWidth = pgm_read_byte(&(FONT_25_WIDTHS[indexNums[i]]));
		uint8_t characterOffset = pgm_read_byte(&(FONT_25_Y_OFFSET[indexNums[i]]));
		
		OLED_draw_xbm(cursorPosition, yPosition + characterOffset, word, characterWidth, 25, screenOrientation);
		cursorPosition += characterWidth + characterSpacing;
	}
}

/*
*	OLED_draw_xbm()
*	---------------
*	Draw an xbm image on the display buffer in the correct position.
*
*	xPosition:	The x-coordinate of the starting pixel of the image.
*
*	yPositioin:	The y-coordinate of the starting pixel of the image.
*
*	xbm:	The xbm image data stored as a PROGMEM array.
*
*	width:	The width of the xbm image.
*
*	height:	The height of the xbm image.
*
*	screenOrientation: The desired orientation of the image:
*				0 = No rotation (normal orientation).
*				1 = 90 degrees clockwise rotation.
*				2 = 180 degrees clockwise rotation (upside down).
*				3 = 270 degrees clockwise rotation.
*
*	NOTE:	
*			-	The correct width and height must be given or the image will not be processed correctly. Also the xbm image must be
*				stored as a PROGMEM array.
			-	No checks for valid x and y coordinated.
*/
void OLED_draw_xbm(uint8_t xPosition, uint8_t yPosition, const uint8_t xbm[], uint8_t width, uint8_t height, uint8_t screenOrientation) {
	uint8_t originalWidth = width;
	
	//	The XBM image needs to be drawn in horizontal sections of 8 pixels. If the width of an XBM image is not a multiple of 8 then the 
	//	irrelevant pixels must be trimmed from the image. This is what the adjustedWidth variable below is used for.
	uint8_t adjustedWidth = width; 
	
	// The number of bytes used horizontally in each row of the image.
	// This variable is used to determine which bits need to be trimmed (if any) to display the image.
	uint8_t multipleBytes;	
	
	uint8_t raw_X_calclulation;	// x-coordinate of a pixel in the image before orientation adjustments.
	uint8_t raw_Y_calclulation;	// y-coordinate of a pixel in the image before orientation adjustments.
	uint8_t targetX;			// x-coordinate of a pixel in the image after orientation adjustments.
	uint8_t targetY;			// y-coordinate of a pixel in the image after orientation adjustments.
	
	if (width == 8) {
		multipleBytes = 0;	// Means only one byte needed per row.
	} else {
		 multipleBytes = width / 8;	// Multiple bytes needed per row.
	}
	
	if ((width % 8) != 0) {
		width = ((width / 8 + 1)) * 8;	// If the width is not a multiple of 8 then adjust the width variable so that it is.
		// eg, a width of 28 would correspond to an actual width of 32.
	}
	
	uint8_t trim = 0;
	for (uint16_t i = 0; i < width * height / 8; i++) {
		uint8_t byteColumn = pgm_read_byte(&(xbm[i]));
		if (multipleBytes > 0) {
			// If trim == multiple bytes then calculate how many bits need to be trimmed off the byte if the width is not a multiple of 8.
			if (trim == multipleBytes) {	
				trim = -1;	// set trim to -1 because it will be incremented back to zero at the end of the if statement.
				adjustedWidth = originalWidth - (multipleBytes * 8);
			} else {
				adjustedWidth = 8;	// Otherwise no bits need to be trimmed and display the entire byte.
			}
		}
		trim++;
		
		for (uint8_t j = 0; j < adjustedWidth; j++) {
			if (bitread(byteColumn, j)) {
				
				raw_X_calclulation = (i * 8 + j) % width + xPosition;
				raw_Y_calclulation = (8 * i / (width)) + yPosition;
				
				if (screenOrientation == 0) {
					// Normal orientation.
					targetX = raw_X_calclulation;
					targetY = raw_Y_calclulation;
					
				} else if (screenOrientation == 1) {				
					// 90 degrees
					targetX = 127 - raw_Y_calclulation;
					targetY = raw_X_calclulation;
					
				} else if (screenOrientation == 2) {
					// 180 degrees
					targetX = 127 - raw_X_calclulation;
					targetY = 63 - raw_Y_calclulation;
					
				} else if (screenOrientation == 3) {
					// 270 degrees
					targetX = raw_Y_calclulation;
					targetY = 63 - raw_X_calclulation;
					
				} else {
					// Invalid orientation used.
					targetX = 0;
					targetY = 0;
					
				}
				OLED_set_pixel(targetX, targetY);
			}

		}
	}
}



/*
*	OLED_screen_off()
*	-----------------
*	Turn the OLED display off.
*/
void OLED_screen_off() {
	OLED_single_command(OLED_DISPLAY_OFF);
}

/*
*	OLED_screen_on()
*	-----------------
*	Turn the OLED display on.
*/
void OLED_screen_on() {
	OLED_single_command(OLED_DISPLAY_ON);
}

/*
*	OLED_invert_buffer()
*	--------------------
*	Invert every pixel in the display buffer.
*/
void OLED_invert_buffer() {
	for (uint8_t i = 0; i < OLED_HEIGHT / 8; i++) {
		for (uint8_t j = 0; j < OLED_WIDTH; j++) {
			OLED_BUFFER[i][j] ^= 0xFF;
		}
	}
}

/*
*	OLED_clear_pixel()
*	----------------
*	Clears a single pixel on the display buffer at the specified position.
*
*	xCoordinate:	The x-coordinate of the pixel.
*
*	yCoordinate:	The y-coordinate of the pixel.
*/
void OLED_clear_pixel(uint8_t xCoordinate, uint8_t yCoordinate) {
	uint8_t page = yCoordinate / 8;
	uint8_t column = xCoordinate;
	uint8_t value = ~(1 << (yCoordinate - (yCoordinate / 8) * 8));
	OLED_BUFFER[page][column] &= value;
}

/*
*	OLED_invert_horizontal()
*	------------------------
*	Inverts the pixels in a horizontal region of the OLED display.
*
*	yTop:		The top y-coordinate of the horizontal region to be inverted.
*
*	yBottom:	The bottom y-coordinate of the horizontal region to be inverted.
*/
void OLED_invert_horizontal(uint8_t yTop, uint8_t yBottom) {
	uint8_t xPos;
	uint8_t yPos;
	
	for (uint8_t i = 0; i < OLED_HEIGHT / 8; i++) {
		for (uint8_t j = 0; j < OLED_WIDTH; j++) {
			for (uint8_t k = 0; k < 8; k++) {
				xPos = j;
				yPos = (i * 8) + k;
				
				if ((yPos >= yTop) && (yPos < yBottom)) {
					if (bitread(OLED_BUFFER[i][j], k)) {
						OLED_clear_pixel(xPos, yPos);
					} else {
						OLED_set_pixel(xPos, yPos);
					}
				}
				
			}
		}
	}
}

/*
*	OLED_invert_vertical()
*	------------------------
*	Inverts the pixels in a vertical region of the OLED display.
*
*	xLeft:		The left x-coordinate of the vertical region to be inverted.
*
*	xRight:		The right x-coordinate of the vertical region to be inverted.
*/
void OLED_invert_vertical(uint8_t xLeft, uint8_t xRight) {
	uint8_t xPos;
	uint8_t yPos;
	
	for (uint8_t i = 0; i < OLED_HEIGHT / 8; i++) {
		for (uint8_t j = 0; j < OLED_WIDTH; j++) {
			for (uint8_t k = 0; k < 8; k++) {
				xPos = j;
				yPos = (i * 8) + k;
				
				if ((xPos >= xLeft) && (xPos < xRight)) {
					if (bitread(OLED_BUFFER[i][j], k)) {
						OLED_clear_pixel(xPos, yPos);
					} else {
						OLED_set_pixel(xPos, yPos);
					}
				}
			}
		}
	}
}

/*
*	OLED_invert_rectangle()
*	-----------------------
*	Invert the pixels within a rectangular region of the OLED display.
*
*	xLeft:		The left x-coordinate of the rectangular region to be inverted.
*
*	xRight:		The right x-coordinate of the rectangular region to be inverted.
*
*	yTop:		The top y-coordinate of the rectangular region to be inverted.
*
*	yBottom:	The bottom y-coordinate of the rectangular region to be inverted.
*/
void OLED_invert_rectangle(uint8_t xLeft, uint8_t xRight, uint8_t yTop, uint8_t yBottom) {
	uint8_t xPos;
	uint8_t yPos;
	
	for (uint8_t i = 0; i < OLED_HEIGHT / 8; i++) {
		for (uint8_t j = 0; j < OLED_WIDTH; j++) {
			for (uint8_t k = 0; k < 8; k++) {
				xPos = j;
				yPos = (i * 8) + k;
				
				if ((xPos >= xLeft) && (xPos < xRight) && (yPos >= yTop) && (yPos < yBottom)) {
					if (bitread(OLED_BUFFER[i][j], k)) {
						OLED_clear_pixel(xPos, yPos);
					} else {
						OLED_set_pixel(xPos, yPos);
					}
				}
			}
		}
	}
}

/*
*	OLED_draw_horizontal_line()
*	---------------------------
*	Draw a horizontal line on the OLED display buffer.
*
*	xStart:		The starting x-coordinate of the horizontal line.
*
*	xEnd:		The ending x-coordinate of the horizontal line.
*
*	yPosition:	The y-coordinate at which the horizontal line will be drawn.
*
*	NOTE: No checks for valid positioning of the line.
*/
void OLED_draw_horizontal_line(uint8_t xStart, uint8_t xEnd, uint8_t yPosition) {
	for (uint8_t i = xStart; i < xEnd; i++) {
		OLED_set_pixel(i, yPosition);
	}
}

/*
*	OLED_draw_vertical_line()
*	---------------------------
*	Draw a vertical line on the OLED display buffer.
*
*	yStart:		The starting y-coordinate of the vertical line.
*
*	yEnd:		The ending y-coordinate of the vertical line.
*
*	xPosition:	The x-coordinate at which the vertical line will be drawn.
*
*
*	NOTE: No checks for valid positioning of the line.
*/
void OLED_draw_vertical_line(uint8_t yStart, uint8_t yEnd, uint8_t xPosition) {
	for (uint8_t i = yStart; i < yEnd; i++) {
		OLED_set_pixel(xPosition, i);
	}
}

/*
*	OLED_draw_rectangle()
*	---------------------
*	Draw a rectangle on the display buffer. You can choose whether the rectangle is solid or an outline.
*
*	xPosition:	The x-coordinate of the top left corner of the rectangle.
*
*	yPosition:	The y-coordinate of the top left corner of the rectangle.
*
*	width:		The width of the rectangle.
*
*	height:		The height of the rectangle.
*
*	filled:
*			-	0 = Not filled (outline).
*			-	1 = Filled (solid).
*
*
*	NOTE: No checks for valid positioning of the rectangle.
*/
void OLED_draw_rectangle(uint8_t xPosition, uint8_t yPosition, uint8_t width, uint8_t height, uint8_t filled) {
	OLED_draw_horizontal_line(xPosition, xPosition + width, yPosition);
	OLED_draw_horizontal_line(xPosition, xPosition + width, yPosition + height - 1);
	
	OLED_draw_vertical_line(yPosition, yPosition + height, xPosition);
	OLED_draw_vertical_line(yPosition, yPosition + height, xPosition + width);
	
	if (filled) {
		for (uint8_t i = yPosition + 1; i < yPosition + height - 1; i++) {
			OLED_draw_horizontal_line(xPosition, xPosition + width, i);
		}
	}
	
}

/*
*	OLED_draw_circle()
*	------------------
*	Draw a circle on the display buffer, either an outline or solid shape.
*	The circle is drawn using the Midpoint Circle Algorithm.
*
*	xCenter:	The x-coordinate of the center of the circle.
*
*	yCenter:	The y-coordinate of the center of the circle.
*
*	radius:		The radius of the circle.
*
*	filled:
*			-	0 = Not filled (outline).
*			-	1 = Filled (solid).
*
*	NOTE: No checks for valid positioning of the circle.
*/
void OLED_draw_circle(uint8_t xCenter, uint8_t yCenter, uint8_t radius, uint8_t filled) {
	int8_t x = radius;	
	int8_t y = 0;		
	int16_t p = 1 - x;

	if (filled) {
		while (x >= y) {
			// Draw horizontal lines for each scan line within the circle
			for (uint8_t i = xCenter - x; i <= xCenter + x; i++) {
				OLED_set_pixel(i, yCenter + y);
				OLED_set_pixel(i, yCenter - y);
			}

			for (uint8_t i = xCenter - y; i <= xCenter + y; i++) {
				OLED_set_pixel(i, yCenter + x);
				OLED_set_pixel(i, yCenter - x);
			}

			y++;
			if (p < 0) {
				p += 2 * y + 1;
			} else {
				x--;
				p += 2 * (y - x) + 1;
			}
		}
	} else {
		while (x >= y) {
			OLED_set_pixel(xCenter + x, yCenter + y);
			OLED_set_pixel(xCenter + y, yCenter + x);
			OLED_set_pixel(xCenter - y, yCenter + x);
			OLED_set_pixel(xCenter - x, yCenter + y);
			OLED_set_pixel(xCenter - x, yCenter - y);
			OLED_set_pixel(xCenter - y, yCenter - x);
			OLED_set_pixel(xCenter + y, yCenter - x);
			OLED_set_pixel(xCenter + x, yCenter - y);

			y++;
			
			if (p < 0) {
				p += 2 * y + 1;
			} else {
				x--;
				p += 2 * (y - x) + 1;
			}
		}
	}	
}