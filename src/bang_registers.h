#ifndef _BANG_REGISTERS_H_
#define _BANG_REGISTERS_H_

/*
	Names by which to access registers.
*/
typedef enum register_name_e {
	FS_CFG = 0x21
} register_name;

/*
	Bounds checking constants for valid registers.
	Make sure first is the first register_name in 
	enumeration above, and last is the last.
*/
#define FIRST_REGISTER_NAME FS_CFG
#define LAST_REGISTER_NAME FS_CFG

/*
	Writes an 8-bit value to the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
int REGISTER_write(register_name rn, uint8_t data, uint8_t* status);
/*
	Reads an 8-bit value from the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
int REGISTER_read(register_name rn, uint8_t* data, uint8_t* status);

/*
	Writes a 1-8 bit value to a bitfield in the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
int REGISTER_write_bitfield(register_name rn, uint8_t data, bit_t ms_bit, bit_t ls_bit, uint8_t* status);
/*
	Reads a 1-8 bit value from a bitfield in the specified register, and
	reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
int REGISTER_read_bitfield(register_name rn, bit_t ms_bit, bit_t ls_bit, uint8_t* data, uint8_t* status);


/*
	Writes a sequence of bytes to a sequence of registers, starting with
	the specified register, and reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
int REGISTER_burst_write(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status);
/*
	Reads a sequence of bytes from a sequence of registers, starting with
	the specified register, and reads the chip status.
	Returns 1 if successful, 0 otherwise.
*/
int REGISTER_burst_read(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status);

#endif
