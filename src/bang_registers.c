
#include "spi.h"
#include "bang_registers.h"

#define _DEBUG_BANG_REGISTERS_

#ifdef _DEBUG_BANG_REGISTERS_
#include <stdio.h>
#endif

static int s_REGISTER_address_is_in_extended_space(register_name rn) {
	return (((rn & (EXTENDED_REGISTER_SPACE << 2)) >> 2) == EXTENDED_REGISTER_SPACE_ADDRESS);
}

static uint8_t s_REGISTER_extract_address(register_name rn) {
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		return (uint8_t)(rn & 0xff);
	}
	else {
		return (uint8_t)(rn & 0x2f);	
	}
}

// Publicly Exported Functions
// ===========================

int REGISTER_write(register_name rn, uint8_t data, uint8_t* status) {
	uint8_t byt = 0;
	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}

#ifdef _DEBUG_BANG_REGISTERS_
	printf("Beginning REGISTER_write to rn: %x, with data: %x...\n", rn, data);
#endif

	SPI_start_transaction();

#ifdef _DEBUG_BANG_REGISTERS_
	printf("\t\tStarted SPI transaction\n");
#endif

	// Signal extended space address if necessary
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		byt = (SPI_WRITE | SPI_SINGLE) | EXTENDED_REGISTER_SPACE_ADDRESS;
		SPI_transfer_byte(byt);
	}

	// OR together single-write command and register address
	byt = (SPI_WRITE | SPI_SINGLE) | s_REGISTER_extract_address(rn);
	SPI_transfer_byte(byt);

	// Write output byte to the register over SPI
	byt = SPI_transfer_byte(data);
	if (status) { // output chip status byte
		*status = byt;
	}

	SPI_stop_transaction();
#ifdef _DEBUG_BANG_REGISTERS_
	printf("\t\tStopped SPI transaction\n");
#endif
	return 1;
}

int REGISTER_read(register_name rn, uint8_t* data, uint8_t* status) {
	uint8_t byt = 0;
	if (rn < FIRST_REGISTER_NAME || rn > LAST_REGISTER_NAME) {
		return 0;
	}

#ifdef _DEBUG_BANG_REGISTERS_
	printf("Beginning REGISTER_read from rn: %x...\n", rn);
#endif

	SPI_start_transaction();

#ifdef _DEBUG_BANG_REGISTERS_
	printf("\t\tStarted SPI transaction\n");
#endif

	if (s_REGISTER_address_is_in_extended_space(rn)) {
		byt = (SPI_READ | SPI_SINGLE) | EXTENDED_REGISTER_SPACE_ADDRESS;
		SPI_transfer_byte(byt);
	}

	// transfer register address
	byt = (SPI_READ | SPI_SINGLE) | s_REGISTER_extract_address(rn);
	byt = SPI_transfer_byte(byt);
	if (status) { // output chip status
		*status = byt;
	}

	// read input byte to the register over SPI
	byt = SPI_transfer_byte(0); // byte transferred is ignored
	if (data) {
		*data = byt;
	}

	SPI_stop_transaction();

#ifdef _DEBUG_BANG_REGISTERS_
	printf("\t\tStopped SPI transaction\n");
#endif
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

	SPI_start_transaction();

	// Signal if address is extended address if necessary
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		byt = (SPI_WRITE | SPI_BURST) | EXTENDED_REGISTER_SPACE_ADDRESS;
		SPI_transfer_byte(byt);
	}

	// Signal register address
	byt = (SPI_WRITE | SPI_BURST) | s_REGISTER_extract_address(rn);
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

	SPI_start_transaction();

	// Signal that registers are in extended space, if necessary
	if (s_REGISTER_address_is_in_extended_space(rn)) {
		byt = (SPI_READ | SPI_BURST) | EXTENDED_REGISTER_SPACE_ADDRESS;
		SPI_transfer_byte(byt);
	}

	// Signal starting register address
	byt = (SPI_READ | SPI_BURST) | s_REGISTER_extract_address(rn);
	byt = SPI_transfer_byte(byt);
	if (status) { // output chip status byte
		*status = byt;
	}

	// Read bytes into array
	for (i = 0; i < data_len; i++) {
		data_arr[i] = SPI_transfer_byte(0);		
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
