
#include <stdint.h>
#include <string.h>

#include "bits.h"
#include "spi.h"
#include "bang_registers.h"
#include "strobe.h"
#include "status_byte.h"
#include "freq_synth_config.h"
#include "chip_reset.h"

#define FIFO_SIZE 128

static uint8_t s_tx_fifo[FIFO_SIZE];
static uint8_t s_rx_fifo[FIFO_SIZE];


/*
	This program doesn't do anything yet, but I'm hoping
	to simulate IO by providing an alternate implementation
	of gpio.h that would fill the rx and tx buffers,
	and a simulated register file, so that I could test
	that everything is working without having a physical chip.
*/
int main(int argc, char** argv) {

	memset(s_tx_fifo, 0, FIFO_SIZE*sizeof(uint8_t));
	memset(s_rx_fifo, 0, FIFO_SIZE*sizeof(uint8_t));

	return 0;
}
