#ifndef _TRANSCEIVER_SIM_INTERFACE_H_
#define _TRANSCEIVER_SIM_INTERFACE_H_

#include <stdint.h>

typedef void* sim_driver_handle;

void SIM_write_to_MOSI(uint8_t hiOrLo, sim_driver_handle dh);
uint8_t SIM_read_from_MISO(sim_driver_handle dh);
void SIM_write_to_SCLK(uint8_t hiOrLo, sim_driver_handle dh);
void SIM_write_to_SS(uint8_t hiOrLo, sim_driver_handle dh);

#endif
