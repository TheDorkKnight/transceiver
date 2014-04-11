#ifndef _FREQUENCY_SYTHESIZER_CONFIGURATION_H_
#define _FREQUENCY_SYTHESIZER_CONFIGURATION_H_

#include <stdint.h>

/*
	Frequency band in MHz
*/
typedef enum freq_band_e {
	FREQ_BAND_820_960 = 0x2,
	FREQ_BAND_420_480 = 0x4, // Used in our application
	FREQ_BAND_273_320 = 0x6,
	FREQ_BAND_205_240 = 0x8,
	FREQ_BAND_164_192 = 0xa,
	FREQ_BAND_136_160 = 0xb
} freq_band;

/*
	Reads current frequency band and chip status.
	Returns 1 if successful, 0 otherwise.
*/
int FREQCONFIG_read_band(freq_band *fb, uint8_t* status);

/*
	Sets frequency band and reads chip status.
	Returns 1 if successful, 0 otherwise.
*/
int FREQCONFIG_set_band(freq_band fb, uint8_t* status);

/*
	Reads whether out-of-lock detector is enabled, and reads chip status.
	Returns 1 if successful, 0 otherwise.
*/
int FREQCONFIG_read_out_of_lock_detector_enabled(int* enabled, uint8_t* status);

/*
	Sets whether out-of-lock detector is enabled, and reads chip status.
	Returns 1 if successful, 0 otherwise.
*/
int FREQCONFIG_set_out_of_lock_detector_enabled(int enable, uint8_t* status);

#endif
