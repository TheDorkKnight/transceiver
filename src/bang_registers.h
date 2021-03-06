#ifndef _BANG_REGISTERS_H_
#define _BANG_REGISTERS_H_

#include "error.h"

#define STANDARD_REGISTER_SPACE 0x2e
#define EXTENDED_REGISTER_SPACE 0xff

#define EXTENDED_REGISTER_SPACE_ADDRESS 0x2f

/*
	Names by which to access registers.

	Regular registers are in the form
	0x00**, where they are less than 0x002e.

	Extended register space is accessed
	from 0x2f, so every extended address
	starts with 0x2f, ie. in the form 0x2f**,
	where they are less than or equal to 
	0x2fff and greater than or equal to
	0x2f00.
*/
typedef uint16_t register_name;

#define FS_CFG       (register_name)0x0021
#define NUM_TX_BYTES (register_name)0x2fd6
#define NUM_RX_BYTES (register_name)0x2fd7

/*
	Bounds checking constants for valid registers.
	Make sure first is the first register_name in 
	enumeration above, and last is the last.
*/
#define FIRST_REGISTER_NAME FS_CFG
#define LAST_REGISTER_NAME  NUM_RX_BYTES

/*
	Writes an 8-bit value to the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
tcvr_error_t REGISTER_write(register_name rn, uint8_t data, uint8_t* status);
/*
	Reads an 8-bit value from the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
tcvr_error_t REGISTER_read(register_name rn, uint8_t* data, uint8_t* status);

/*
	Writes a 1-8 bit value to a bitfield in the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
tcvr_error_t REGISTER_write_bitfield(register_name rn, uint8_t data, bit_t ms_bit, bit_t ls_bit, uint8_t* status);
/*
	Reads a 1-8 bit value from a bitfield in the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
tcvr_error_t REGISTER_read_bitfield(register_name rn, bit_t ms_bit, bit_t ls_bit, uint8_t* data, uint8_t* status);


/*
	Writes a sequence of bytes to a sequence of registers, starting with
	the specified register, and reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
tcvr_error_t REGISTER_burst_write(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status);
/*
	Reads a sequence of bytes from a sequence of registers, starting with
	the specified register, and reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
tcvr_error_t REGISTER_burst_read(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status);

#endif
