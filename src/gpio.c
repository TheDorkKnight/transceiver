/*
	Software simulation of GPIO pins, non-threadsafe.
*/

#include <stdint.h>
#include "gpio.h"

static uint8_t MOSI = LOW;
static uint8_t MISO = LOW;
static uint8_t SCLK = LOW;
static uint8_t SS   = LOW;

uint8_t GPIO_read_MOSI() {
	return MOSI;
}

void GPIO_write_MOSI(uint8_t hiOrLo) {
	MOSI = (hiOrLo) ? HIGH : LOW;
}

uint8_t GPIO_read_MISO() {
	return MISO;
}

void GPIO_write_MISO(uint8_t hiOrLo) {
	MISO = (hiOrLo) ? HIGH : LOW;
}

uint8_t GPIO_read_SCLK() {
	return SCLK;
}

void GPIO_write_SCLK(uint8_t hiOrLo) {
	SCLK = (hiOrLo) ? HIGH : LOW;
}

uint8_t GPIO_read_SS() {
	return SS;
}

void GPIO_write_SS(uint8_t hiOrLo) {
	SS = (hiOrLo) ? HIGH : LOW;
}