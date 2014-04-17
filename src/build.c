
#include <stdint.h>

#include "error.h"
#include "bits.h"
#include "gpio.h"
#include "spi.h"
#include "bang_registers.h"
#include "strobe.h"
#include "status_byte.h"
#include "freq_synth_config.h"
#include "chip_reset.h"


/*
	This program doesn't do anything, just exists to make
	sure that object files can be linked into a single
	executable.
*/
int main(int argc, char** argv) {

	return 0;
}
