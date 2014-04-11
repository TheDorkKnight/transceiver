#ifndef _CHIP_RESET_H_
#define _CHIP_RESET_H_

#include <stdint.h>

/*
	The chip can be reset by:

		a) Power-cycling, causing an automatic sequence
		b) Strobing SRES (RESET_strobe_reset function provided below)
		c) Using the RESET_N pin
*/

/*
	Sends a strobe to reset the transceiver, and reads the
	chip status.
	Returns 1 if successful, 0 otherwise.
*/
int RESET_strobe_reset(uint8_t* status);

#endif

