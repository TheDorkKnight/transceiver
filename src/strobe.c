
#include <stdint.h>

#include "gpio.h"
#include "spi.h"
#include "strobe.h"

static uint8_t s_get_address(strobe_name sn) {
	return (uint8_t)(sn);
}

static void s_delay() {
	// nothing
}

int STROBE_command_strobe(strobe_name sn, uint8_t* status) {
	uint8_t byt = 0;

	if (sn < SRES || sn > SNOP) {
		return 0;
	}

	if (sn == SRES) {
		// SRES is handled in a special way
	}

	// Set Read/Write bit to write
	byt |= SPI_WRITE;

	// Second bit should be 0, so let's make sure
	byt &= 0xbf; // 10111111b

	// Set strobe register address
	byt |= (s_get_address(sn) & 0x3f); // addr & 00111111b

	// Write the address of the strobe register over SPI, which signals strobe
	SPI_start_transaction();

	byt = SPI_transfer_byte(byt);
	if (status) {
		*status = byt;
	}

	if (sn == SRES) {
		// we must wait until SO goes low before releasing
		// CSn to high, ie. stopping transaction
		while (GPIO_read_MISO() == HIGH) {
			// wait
			s_delay();
		}
	}

	SPI_stop_transaction();
	return 1;
}
