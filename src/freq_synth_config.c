
#include <stdint.h>

#include "error.h"
#include "bits.h"
#include "bang_registers.h"
//#include "xosc.h"
#include "freq_synth_config.h"

#define FREQCONFIG_LOCK_ENABLED_MS_BIT BIT_4
#define FREQCONFIG_LOCK_ENABLED_LS_BIT BIT_4

#define FREQCONFIG_BAND_MS_BIT BIT_3
#define FREQCONFIG_BAND_LS_BIT BIT_0

/*
	radio frequency = (VCO frequency / LO Divider) Hz

	VCO frequency = (((FREQ/2^16) * XOSC frequency) + ((FREQOFF/2^18) * XOSC frequency)) Hz
*/
/*
static uint32_t s_FREQCONFIG_vcof(uint32_t freq, uint16_t freq_off, XOSC_frequency xosc) {
	uint32_t freq_part = freq * ((uint32_t)xosc / (2 << 15));
	uint32_t freq_off_part = (uint32_t)freq_off * ((uint32_t)xosc / (2 << 17));
	return freq_part + freq_off_part; 
}

static uint32_t s_FREQCONFIG_rf(uint32_t freq, uint16_t freq_off, XOSC_frequency xosc, freq_band fb) {
	return (s_FREQCONFIG_vcof(freq, freq_off, xosc) / (uint32_t)fb);
}
*/

tcvr_error_t FREQCONFIG_read_band(freq_band* fb, uint8_t* status) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      data;

	// read bitfield
	err = REGISTER_read_bitfield(FS_CFG, FREQCONFIG_BAND_MS_BIT, FREQCONFIG_BAND_LS_BIT, &data, status);
	if (err != ERROR_NONE) {
		return err;
	}

	// output bitfield data as frequency band
	if (fb) {
		*fb = (freq_band)data;
	}
	return ERROR_NONE;
}

tcvr_error_t FREQCONFIG_read_out_of_lock_detector_enabled(int* enabled, uint8_t* status) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      data;

	// read bitfield
	err = REGISTER_read_bitfield(FS_CFG, FREQCONFIG_LOCK_ENABLED_MS_BIT, FREQCONFIG_LOCK_ENABLED_LS_BIT, &data, status);
	if (err != ERROR_NONE) {
		return err;
	}

	// output bitfield data as frequency band
	if (enabled) {
		*enabled = (int)data;
	}
	return ERROR_NONE;
}

tcvr_error_t FREQCONFIG_set_band(freq_band fb, uint8_t* status) {
	uint8_t data = (uint8_t)fb;

	return REGISTER_write_bitfield(FS_CFG, data, FREQCONFIG_BAND_MS_BIT, FREQCONFIG_BAND_LS_BIT, status);
}

tcvr_error_t FREQCONFIG_set_out_of_lock_detector_enabled(int enable, uint8_t* status) {
	uint8_t data = (enable) ? 1 : 0;

	return REGISTER_write_bitfield(FS_CFG, data, FREQCONFIG_LOCK_ENABLED_MS_BIT, FREQCONFIG_LOCK_ENABLED_LS_BIT, status);
}

