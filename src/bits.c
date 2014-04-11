
#include "bits.h"

uint8_t BITS_bitfield_mask(bit_t ms_bit, bit_t ls_bit) {
	uint8_t mask = 0;
	bit_t   bit;

	if (ls_bit > ms_bit) {
		bit_t tmp = ls_bit;
		ls_bit = ms_bit;
		ms_bit = tmp;
	}

	bit = ms_bit;
	while (ms_bit >= ls_bit) {
		mask |= bit;
		bit >>= 1;
	}

	return mask;
}

