/*
 **************************************************************
 * Atmega328p_SPI.h
 * Author: Tom
 * Date: 16/11/2023
 * This is a generic SPI library for the Atmega328p MCU. 
 **************************************************************
 * EXTERNAL FUNCTIONS
 **************************************************************
 * A328p_set_SS() - Set the SS pin high or low.
 * A328p_SPI_init() - Init the Atmega328p for SPI comms.
 * A328p_SPI_transfer_data_to_reg() - Transmit some data to a 
 * register on the peripheral device.
 * A328p_SPI_transfer_data_only() - Transmit only data.
 * A328p_SPI_receive_from_reg() - Receive data from a register
 * on the peripheral slave device.
 * A328p_SPI_send_reg_only() - Send only the register to the
 * slave device (used for multi byte read/write).
 * A328p_SPI_receive_data_only() - Receive data from the slave
 * device (used for multi byte read).
 **************************************************************
*/

#ifndef ATMEGA328P_SPI_H_
#define ATMEGA328P_SPI_H_

#define SS		2
#define MOSI	3
#define MISO	4
#define SCK		5

// Macros for easily setting the SS pin high or low
#define SS_HIGH A328p_set_SS(1)
#define SS_LOW	A328p_set_SS(0)

void A328p_set_SS(uint8_t value);
void A328p_SPI_init();
void A328p_SPI_transfer_data_to_reg(uint8_t reg, uint8_t data);
void A328p_SPI_transfer_data_only(uint8_t data);
uint8_t A328p_SPI_receive_from_reg(uint8_t reg);
void A328p_SPI_send_reg_only(uint8_t reg);
uint8_t A328p_SPI_receive_data_only();

#endif /* ATMEGA328P_SPI_H_ */