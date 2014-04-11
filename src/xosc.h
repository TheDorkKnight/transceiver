#ifndef _XOSC_H_
#define _XOSC_H_

#include <stdint.h>

/*
	Crystal Oscillator Frequency
*/
typedef enum XOSC_frequency_e {
	XOSC_FREQUENCY_32_MHZ = 32000000,
	XOSC_FREQUENCY_40_MHZ = 40000000
} XOSC_frequency;

#endif
