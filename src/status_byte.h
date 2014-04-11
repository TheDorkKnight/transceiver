#ifndef _STATUS_BYTE_H_
#define _STATUS_BYTE_H_

#include <stdint.h>

/*
	Current state of the transceiver chip.
*/
typedef enum chip_status_e {
	STATUS_IDLE = 0x00,
	STATUS_RX = 0x01,
	STATUS_TX = 0x02,
	STATUS_FSTXON = 0x03,
	STATUS_CALIBRATE = 0x04,
	STATUS_SETTLING = 0x05,
	STATUS_RXFIFOERROR = 0x06,
	STATUS_TXFIFOERROR = 0x07
} chip_status;

/*
	Returns 1 if status_byte indicates that oscillator
	has settled. Otherwise returns 0.
*/
int STATUS_chip_is_ready(uint8_t status_byte);

/*
	Extracts chip_status from status_byte.
*/
chip_status STATUS_get_chip_status(uint8_t status_byte);

#endif
