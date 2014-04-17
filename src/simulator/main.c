
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../error.h"
#include "../bits.h"
#include "../spi.h"
#include "../bang_registers.h"
#include "../strobe.h"
#include "../status_byte.h"
#include "../freq_synth_config.h"
#include "../chip_reset.h"

#define FIFO_SIZE 128

/*
	This program doesn't do anything yet, but I'm hoping
	to simulate IO by providing an alternate implementation
	of gpio.h that would fill the rx and tx buffers,
	and a simulated register file, so that I could test
	that everything is working without having a physical chip.
*/
int main(int argc, char** argv) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      byt = 0xc4;
	uint8_t      test = 0;
	uint8_t      status = 0xff;

	printf("Beginning register test...\n");

	err = REGISTER_write(FS_CFG, byt, &status);
	if (err != ERROR_NONE) {
		printf("REGISTER_write was not a success\n");
	}

	err = REGISTER_read(FS_CFG, &test, &status);
	if (err != ERROR_NONE) {
		printf("REGISTER_read was not a success\n");
	}

	if (byt == test) {
		printf("Values written and read were the same\n");
	}
	else {
		printf("Value written: %u. Value read: %u\n", byt, test);
	}

	return 0;
}
