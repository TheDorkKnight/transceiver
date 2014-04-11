
#include "bits.h"
#include "bang_registers.h"
#include "status_byte.h"

#define CHIP_RDYn_MS_BIT BIT_7
#define CHIP_RDYn_LS_BIT BIT_7

#define CHIP_STATE_MS_BIT BIT_6
#define CHIP_STATE_LS_BIT BIT_4

#define CHIP_STATE_RESERVED_MS_BIT BIT_3
#define CHIP_STATE_RESERVED_LS_BIT BIT_0


int STATUS_chip_is_ready(uint8_t status_byte) {
	uint8_t mask = BITS_bitfield_mask(CHIP_RDYn_MS_BIT, CHIP_RDYn_LS_BIT);
	status_byte &= mask;
	return (status_byte) ? 1 : 0;
}

chip_status STATUS_get_chip_status(uint8_t status_byte) {
	uint8_t mask = BITS_bitfield_mask(CHIP_STATE_MS_BIT, CHIP_STATE_LS_BIT);
	bit_t   bit;

	// mask away unused data
	status_byte &= mask;

	// right shift until ls_bit becomes bit 0
	bit = CHIP_STATE_LS_BIT;
	while (bit > BIT_0) {
		status_byte >>= 1;
		bit >>= 1;
	}

	return (chip_status)status_byte;
}

// static const char* s_descriptions[] = {
// 	"IDLE State",
// 	"Receive mode",
// 	"Transmit mode",
// 	"Fast TX ready",
// 	"Frequency synthesizer calibration is running",
// 	"PLL is settling",
// 	"RX FIFO has over/underflowed. Read out any "
// 	"useful data, then flush the FIFO with an "
// 	"SFRX strobe",
// 	"TX FIFO has over/underflowed. Flush the FIFO "
// 	"with an SFTX strobe"
// };

// const char* status_get_description(chip_status cs) {
// 	if cs < STATUS_IDLE || cs > STATUS_TXFIFOERROR {
// 		return "Unknown chip status";
// 	}
// 	return s_descriptions[cs];
// }

