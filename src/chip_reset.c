
#include <stdint.h>
#include "strobe.h"
#include "chip_reset.h"

int RESET_strobe_reset(uint8_t *status) {
	return STROBE_command_strobe(SRES, status);
}
