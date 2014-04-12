#ifndef _TRANSCEIVER_SIM_H_
#define _TRANSCEIVER_SIM_H_

#include <stdint.h> // uint types
#include <pthread.h> // pthreads

#include "../bits.h"
#include "../bang_registers.h"
#include "../rxtx.h"

typedef enum sim_io_command_e {
	SIM_IO_READY,
	SIM_IO_SINGLE_REGISTER_READ,
	SIM_IO_SINGLE_REGISTER_WRITE,
	SIM_IO_BURST_REGISTER_READ,
	SIM_IO_BURST_REGISTER_WRITE,
	SIM_IO_EXTENDED_SPACE,
	SIM_IO_SINGLE_RX_FIFO,
	SIM_IO_SINGLE_TX_FIFO,
	SIM_IO_BURST_RX_FIFO,
	SIM_IO_BURST_TX_FIFO
} sim_io_command;

typedef struct sim_driver_s {
	pthread_mutex_t MOSI_mutex;
	pthread_mutex_t MISO_mutex;
	pthread_mutex_t SCLK_mutex;
	pthread_mutex_t SS_mutex;
	uint8_t MOSI_bit;
	uint8_t MISO_bit;
	uint8_t SCLK_bit;
	uint8_t SS_bit;
	uint8_t last_clock_value;
	uint8_t tx_fifo[TRANSCEIVER_FIFO_SIZE];
	uint8_t rx_fifo[TRANSCEIVER_FIFO_SIZE];
	uint8_t standard_registers[STANDARD_REGISTER_SPACE];
	uint8_t extended_registers[EXTENDED_REGISTER_SPACE];
	uint8_t currently_accessing_extended;
	uint8_t chip_status;
	uint8_t current_output_byte;
	uint8_t current_address;
	uint8_t current_input_byte;
	bit_t   current_bit;
	sim_io_command current_command;
} sim_driver;

sim_driver* SIM_create_sim_driver();
void SIM_release_sim_driver(sim_driver** driver);

#endif
