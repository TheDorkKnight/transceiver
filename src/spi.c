
#include <stdint.h>
#include "bits.h"
#include "gpio.h"
#include "spi.h"

#define SPI_write_to_SI(A) (GPIO_write_MOSI(A))
#define SPI_read_from_SO GPIO_read_MISO
#define SPI_write_to_SCLK(A) (GPIO_write_SCLK(A))
#define SPI_write_to_CSn(A) (GPIO_write_SS(A))

#define SPI_NUM_DELAY_CYCLES 10

static void s_SPI_delay(int multiplier) {
	int i;
	int delay = multiplier * SPI_NUM_DELAY_CYCLES;
	if (delay <= 0) {
		delay = 50;
	}
	for (i = 0; i < delay; i++) {
		// do nothing
	}
}

void SPI_start_transaction(void) {
	SPI_write_to_CSn(LOW);
	s_SPI_delay(1);
}

void SPI_stop_transaction(void) {
	SPI_write_to_CSn(HIGH);
	s_SPI_delay(1);
}

/*
	Required: use SPI_write_to_CSn(LOW) before using,
	          SPI_write_to_CSn(HIGH) after all done.
*/
uint8_t SPI_transfer_byte(uint8_t byte_out) {
	uint8_t byte_in;
	uint8_t bit;

	for (bit = 0; bit < 8; bit++) {
		/* write a bit */
		if (byte_out & 0x80) {
			SPI_write_to_SI(HIGH);
		}
		else {
			SPI_write_to_SI(LOW);
		}
		byte_out <<= 1;

		/* Delay for at least the peer's setup time */
		// !!! Find out how long
		s_SPI_delay(1);

		/* Pull the clock line high */
		SPI_write_to_SCLK(HIGH);

		/* Read a bit from the slave */
		byte_in <<= 1; // pull in a bit of space
		if (SPI_read_from_SO()) {
			byte_in |= 0x01;
		}

		/* Delay for at least the peer's hold time */
		// !!! Find out how long
		s_SPI_delay(1);

		/* Pull the clock line low */
		SPI_write_to_SCLK(LOW);
	}

	s_SPI_delay(1);

	return byte_in;
}



