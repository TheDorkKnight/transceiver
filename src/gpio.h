#ifndef _TRANSCEIVER_GPIO_H_
#define _TRANSCEIVER_GPIO_H_

#include <stdint.h>

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

// Master
// =====================================

void GPIO_write_MOSI(uint8_t hiOrLo);
uint8_t GPIO_read_MISO();
void GPIO_write_SCLK(uint8_t hiOrLo);
void GPIO_write_SS(uint8_t hiOrLo);

// Slave
// =====================================

uint8_t GPIO_read_MOSI();
void GPIO_write_MISO(uint8_t hiOrLo);
uint8_t GPIO_read_SCLK();
uint8_t GPIO_read_SS();

#endif
