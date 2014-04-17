
#include <stdint.h>
#include "error.h"
#include "strobe.h"
#include "chip_reset.h"

tcvr_error_t RESET_strobe_reset(uint8_t *status) {
	return STROBE_command_strobe(SRES, status);
}
