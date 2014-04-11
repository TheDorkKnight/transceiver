#ifndef _BITS_H_
#define _BITS_H_

#include <stdint.h>

typedef uint8_t bit_t;

#define BIT_0 (bit_t)0x01
#define BIT_1 (bit_t)0x02
#define BIT_2 (bit_t)0x04
#define BIT_3 (bit_t)0x08

#define BIT_4 (bit_t)0x10
#define BIT_5 (bit_t)0x20
#define BIT_6 (bit_t)0x40
#define BIT_7 (bit_t)0x80

/*
	Returns bitmask with 1's between most significant bit
	and least significant bit, and 0's outside of them.
*/
uint8_t BITS_bitfield_mask(bit_t ms_bit, bit_t ls_bit);

#endif
