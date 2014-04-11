#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>
#include "bits.h"

/*
	Read/Write bit for register/strobe/fifo addresses.
*/
#define SPI_READ_WRITE_BIT BIT_7
#define SPI_READ (SPI_READ_WRITE_BIT & 0xff)
#define SPI_WRITE (SPI_READ_WRITE_BIT & 0x00)

/*
	Single-Access/Burst bit for register/fifo addresses.
*/
#define SPI_SINGLE_BURST_BIT BIT_6
#define SPI_BURST (SPI_SINGLE_BURST_BIT & 0xff)
#define SPI_SINGLE (SPI_SINGLE_BURST_BIT & 0x00)

/*
	Starts SPI transaction by pulling CSn line low.
*/
void SPI_start_transaction(void);

/*
	Stops SPI transaction by pulling CSn line high.
*/
void SPI_stop_transaction(void);

/*
	Before transferring 1 or 2 bytes as per SPI protocol,
	caller must call SPI_start_transaction. After completing,
	call SPI_stop_transaction.
*/
uint8_t SPI_transfer_byte(uint8_t byte_out);

#endif
