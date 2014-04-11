
#include "spi.h"
#include "bang_registers.h"

#define EXTENDED_REGISTER_SPACE 0x2f

static int s_REGISTER_extended_space_write(register_name rn, uint8_t data, uint8_t* status) {
	/*
		In the extended register space, we must first send a command
		to address 0x3f, and then we can access the register.
	*/
	uint8_t byt = (SPI_WRITE | SPI_SINGLE) | EXTENDED_REGISTER_SPACE; // extended command

	SPI_start_transaction();

	// Signal write to extended register space
	SPI_transfer_byte(byt);

	// Write address of register within extended space
	byt = rn & 0x00ff;
	SPI_transfer_byte(byt);

	// Write data to register
	byt = SPI_transfer_byte(data);
	if (status) {
		*status = byt;
	}

	SPI_stop_transaction();
	return 1;
}

static int s_REGISTER_extended_space_read(register_name rn, uint8_t* data, uint8_t* status) {
	/*
		In the extended register space, we must first send a command
		to address 0x3f, and then we can access the register.
	*/
	uint8_t byt = (SPI_READ | SPI_SINGLE) | EXTENDED_REGISTER_SPACE; // extended command

	SPI_start_transaction();

	// Signal read from extended register space
	byt = SPI_transfer_byte(byt);
	if (status) {
		*status = byt;
	}

	// Write address of register within extended space
	byt = rn & 0x00ff;
	SPI_transfer_byte(byt);

	// Read data from register
	byt = SPI_transfer_byte(0);
	if (data) {
		*data = byt;
	}

	SPI_stop_transaction();
	return 1;
}

static int s_REGISTER_extended_space_burst_write(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status) {
	/*
		In the extended register space, we must first send a command
		to address 0x3f, and then we can access the register.
	*/
	uint8_t byt = (SPI_WRITE | SPI_BURST) | EXTENDED_REGISTER_SPACE; // extended command
	uint8_t i;
	if (data_len <= 1) {
		return 0;
	}
	if (!data_arr) {
		return 0;
	}

	SPI_start_transaction();

	// Signal write to extended register space
	byt = SPI_transfer_byte(byt);
	if (status) {
		*status = byt;
	}

	// Write address of register within extended space
	byt = rn & 0x00ff;
	SPI_transfer_byte(byt);

	for (i = 0; i < data_len; i++) {
		SPI_transfer_byte(data_arr[i]);
	}		

	SPI_stop_transaction();
	return 1;
}

static int s_REGISTER_extended_space_burst_read(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status) {
	/*
		In the extended register space, we must first send a command
		to address 0x3f, and then we can access the register.
	*/
	uint8_t byt = (SPI_READ | SPI_BURST) | EXTENDED_REGISTER_SPACE; // extended command
	uint8_t i;
	if (data_len <= 1) {
		return 0;
	}

	SPI_start_transaction();

	// Signal read from extended register space
	byt = SPI_transfer_byte(byt);
	if (status) {
		*status = byt;
	}

	// Write address of register within extended space
	byt = rn & 0x00ff;
	SPI_transfer_byte(byt);

	// Read data from registers
	if (data_arr) {
		for (i = 0; i < data_len; i++) {
			data_arr[i] = SPI_transfer_byte(0);
		}
	}
	else {
		for (i = 0; i < data_len; i++) {
			SPI_transfer_byte(0);
		}		
	}

	SPI_stop_transaction();
	return 1;
}

static int s_REGISTER_address_is_in_extended_space(register_name rn) {
	return (((rn & (EXTENDED_REGISTER_SPACE << 2)) >> 2) == EXTENDED_REGISTER_SPACE);
}

// Publicly Exported Functions
// ===========================

int REGISTER_write(register_name rn, uint8_t data, uint8_t* status) {
	uint8_t byt = 0;
	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		return s_REGISTER_extended_space_write(rn, data, status);
	}

	// 7th bit is write bit
	byt |= SPI_WRITE;

	// second bit is single-access bit
	byt |= SPI_SINGLE;

	// last six bits are address
	byt |= (rn & 0x3f);

	SPI_start_transaction();

	// Write the address byte over SPI
	SPI_transfer_byte(byt);

	// Write output byte to the register over SPI
	byt = SPI_transfer_byte(data);
	if (status) {
		*status = byt;
	}

	SPI_stop_transaction();
	return 1;
}

int REGISTER_read(register_name rn, uint8_t* data, uint8_t* status) {
	uint8_t byt = 0;
	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		return s_REGISTER_extended_space_read(rn, data, status);
	}

	// 7th bit is read-write bit, set to read
	byt |= SPI_READ;

	// second bit is single-access bit
	byt |= SPI_SINGLE;

	// last six bits are address
	byt |= (rn & 0x3f);

	SPI_start_transaction();

	// Write the address byte over SPI
	byt = SPI_transfer_byte(byt);
	if (status) {
		*status = byt;
	}

	// read input byte to the register over SPI
	byt = SPI_transfer_byte(0); // byte transferred is ignored
	if (data) {
		*data = byt;
	}

	SPI_stop_transaction();
	return 1;
}

int REGISTER_burst_write(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status) {
	uint8_t byt = 0;
	uint8_t i;
	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}
	if (!data_arr) {
		return 0;
	}
	if (data_len <= 1) {
		return 0;
	}
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		return s_REGISTER_extended_space_burst_write(rn, data_arr, data_len, status);
	}

	// 7th bit is write bit
	byt |= SPI_WRITE;

	// second bit is burst-access bit
	byt |= SPI_BURST;

	// last six bits are address
	byt |= (rn & 0x3f);

	SPI_start_transaction();

	// Write the address byte over SPI
	SPI_transfer_byte(byt);

	// Write output bytes to the registers over SPI
	for (i = 0; i < data_len; i++) {
		byt = SPI_transfer_byte(data_arr[i]);		
	}

	if (status) {
		*status = byt;
	}

	SPI_stop_transaction();
	return 1;
}

int REGISTER_burst_read(register_name rn, uint8_t* data_arr, uint8_t data_len, uint8_t* status) {
	uint8_t byt = 0;
	uint8_t i;
	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}
	if (!data_arr) {
		return 0;
	}
	if (data_len <= 1) {
		return 0;
	}
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		return s_REGISTER_extended_space_burst_read(rn, data_arr, data_len, status);
	}

	// 7th bit is read bit
	byt |= SPI_READ;

	// second bit is burst-access bit
	byt |= SPI_BURST;

	// last six bits are address
	byt |= (rn & 0x3f);

	SPI_start_transaction();

	// Write the address byte over SPI
	byt = SPI_transfer_byte(byt);

	// Read bytes over SPI
	for (i = 0; i < data_len; i++) {
		data_arr[i] = SPI_transfer_byte(0);		
	}

	if (status) {
		*status = byt;
	}

	SPI_stop_transaction();
	return 1;
}

int REGISTER_write_bitfield(register_name rn, uint8_t data,
                             bit_t ms_bit, bit_t ls_bit,
                             uint8_t* status) {
	bit_t bit = 0;
	uint8_t old_data = 0;
	uint8_t mask = 0;

	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}

	// read the old data
	if (REGISTER_read(rn, &old_data, status) == 0) {
		return 0;
	}

	// make sure bits are in appropriate significance order
	if (ls_bit > ms_bit) {
		bit_t tmp = ls_bit;
		ls_bit = ms_bit;
		ms_bit = tmp;
	}

	// shift the new data by appropriate amount
	bit = BIT_0;
	while (bit < ls_bit) {
		data <<= 1;
		bit <<=1;
	}

	// make a mask for the bitfield
	mask = BITS_bitfield_mask(ms_bit, ls_bit);

	// use that mask on the new data
	data &= mask;

	// invert the mask to use on the old data
	mask = ~mask;

	// mask away the bitfield from the old data
	old_data &= mask;

	// combine the new and old data
	data |= old_data;

	// write the data to the register
	return REGISTER_write(rn, data, status);
}

int REGISTER_read_bitfield(register_name rn, bit_t ms_bit, bit_t ls_bit,
                           uint8_t* data, uint8_t* status) {
	bit_t bit = 0;
	uint8_t mask = 0;

	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}

	// read the data
	if (REGISTER_read(rn, data, status) == 0) {
		return 0;
	}

	// make sure bits are in appropriate significance order
	if (ls_bit > ms_bit) {
		bit_t tmp = ls_bit;
		ls_bit = ms_bit;
		ms_bit = tmp;
	}

	// make a mask for the bitfield
	mask = BITS_bitfield_mask(ms_bit, ls_bit);

	// use that mask on the new data
	*data &= mask;

	// shift the data by appropriate amount
	bit = ls_bit;
	while (bit > BIT_0) {
		*data >>= 1;
		bit >>=1;
	}

	return 1;
}
