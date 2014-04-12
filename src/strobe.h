#ifndef _STROBE_H_
#define _STROBE_H_

#include <stdint.h>

#define STROBE_ADDRESS_START 0x30
#define STROBE_ADDRESS_END   0x3d

typedef enum strobe_name_e {
	SRES    = 0x30, // Reset Chip
	SFSTXON = 0x31, // Enable and calibrate frequency synthesizer
	SXOFF   = 0x32, // Enter XOFF state when CSn is de-asserted
	SCAL    = 0x33, // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without setting manual calibration mode (SETTLING_CFG.FS_AUTOCAL == 0).
	SRX     = 0x34, // Enable RX. Perform calibration first if coming from IDLE and SETTLING_CFG.FS_AUTOCAL == 1.
	STX     = 0x35, // In IDLE state: Enable TX. Perform  calibration first if SETTLING_CFG.FS_AUTOCAL == 1. If in RX state and PKT_CFG2.CCA_MODE != 0: Only go to TX channel if clear.
	SIDLE   = 0x36, // Exit RX/TX, turn off frequency synthesizer and exit eWOR mode if applicable.
	SAFC    = 0x37, // Automatic Frequency Compensation
	SWOR    = 0x38, // Start automatic RX polling sequence if WOR_CFG0.RC_PD == 0
	SPWD    = 0x39, // Enter SLEEP mode when CSn is de-asserted.
	SFRX    = 0x3a, // Flush the RX FIFO. Only issue SFRX in IDLE or RX_FIFO_ERR states.
	SFTX    = 0x3b, // Flush the TX FIFO. Only issue SFTX in IDLE or TX_FIFO_ERR states.
	SWORRST = 0x3c, // Reset the eWOR timer to the Event1 value.
	SNOP    = 0x3d  // No operation. May be used to get access to the chip status byte.
} strobe_name;

int STROBE_command_strobe(strobe_name sn, uint8_t* status);

#endif
