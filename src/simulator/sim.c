
#include <stdlib.h> // malloc, free
#include <stdint.h> // for uint8_t
#include <string.h> // memset
#include <stdio.h>
#include <pthread.h> // for pthreads

#include "../bits.h"
#include "../gpio.h" // for HIGH, LOW
#include "../spi.h" // for SPI_READ/WRITE, SPI_SINGLE/BURST
#include "../strobe.h" // for STROBE_ADDRESS_START/STOP
#include "sim_iface.h"
#include "sim.h"

#define _DEBUG_SIM_

sim_driver* SIM_create_sim_driver() {
	int failure;
	sim_driver* driver = (sim_driver*)malloc(sizeof(sim_driver));

#ifdef _DEBUG_SIM_
	printf("Creating sim driver\n");
#endif

	if (!driver) {
		return NULL;
	}

	driver->MOSI_bit = LOW;
	driver->MISO_bit = LOW;
	driver->SCLK_bit = LOW;
	driver->SS_bit   = LOW;
	driver->last_clock_value = LOW;

	memset(driver->tx_fifo, 0, TRANSCEIVER_FIFO_SIZE*sizeof(driver->tx_fifo[0]));
	memset(driver->rx_fifo, 0, TRANSCEIVER_FIFO_SIZE*sizeof(driver->rx_fifo[0]));	
	memset(driver->standard_registers, 0, STANDARD_REGISTER_SPACE*sizeof(driver->standard_registers[0]));
	memset(driver->extended_registers, 0, EXTENDED_REGISTER_SPACE*sizeof(driver->extended_registers[0]));

	failure = pthread_mutex_init(&driver->MOSI_mutex, NULL);
	if (failure) {
		free(driver);
		return NULL;
	}
	failure = pthread_mutex_init(&driver->MISO_mutex, NULL);
	if (failure) {
		free(driver);
		return NULL;
	}
	failure = pthread_mutex_init(&driver->SCLK_mutex, NULL);
	if (failure) {
		free(driver);
		return NULL;
	}
	failure = pthread_mutex_init(&driver->SS_mutex, NULL);
	if (failure) {
		free(driver);
		return NULL;
	}
	driver->currently_accessing_extended = 0;

	driver->chip_status = 0; // READY and IDLE
	driver->current_output_byte = driver->chip_status;
	driver->current_input_byte = 0;
	driver->current_address = 0;
	driver->current_bit = BIT_7;

	driver->current_command = SIM_IO_READY;


#ifdef _DEBUG_SIM_
	printf("Successfully created sim driver.\n");
#endif

	return driver;
}

void SIM_release_sim_driver(sim_driver** driver) {
	if (driver) {
		sim_driver* d = *driver;
		if (d) {
			pthread_mutex_destroy(&d->MOSI_mutex);
			pthread_mutex_destroy(&d->MISO_mutex);
			pthread_mutex_destroy(&d->SCLK_mutex);
			pthread_mutex_destroy(&d->SS_mutex);
			free(d);
			*driver = NULL;
		}
	}
}

// Slave-side functions to read GPIO lines
// =======================================

uint8_t SIM_read_from_MOSI(sim_driver* driver) {
	if (driver) {
		int     failure = pthread_mutex_lock(&driver->MOSI_mutex);
		uint8_t bit;
		if (failure) {
			return LOW;
		}
		bit = (driver->MOSI_bit == HIGH || driver->MOSI_bit == LOW) ? driver->MOSI_bit : ((driver->MOSI_bit) ? HIGH : LOW);
		pthread_mutex_unlock(&driver->MOSI_mutex);
#ifdef _DEBUG_SIM_
		printf("\tSlave read %u from MOSI\n", bit);
#endif
		return bit;
	}

	return LOW;
}

void SIM_write_to_MISO(uint8_t hiOrLo, sim_driver* driver) {
	if (driver) {
		int failure = pthread_mutex_lock(&driver->MISO_mutex);
		if (failure) {
			return;
		}
		driver->MISO_bit = hiOrLo;
#ifdef _DEBUG_SIM_
		printf("\tSlave wrote %u to MISO\n", hiOrLo);
#endif
		pthread_mutex_unlock(&driver->MISO_mutex);		
	}
}

uint8_t SIM_read_from_SCLK(sim_driver* driver) {
	if (driver) {
		int     failure = pthread_mutex_lock(&driver->SCLK_mutex);
		uint8_t bit;
		if (failure) {
			return LOW;
		}
		bit = (driver->SCLK_bit == HIGH || driver->SCLK_bit == LOW) ? driver->SCLK_bit : ((driver->SCLK_bit) ? HIGH : LOW);
		pthread_mutex_unlock(&driver->SCLK_mutex);
#ifdef _DEBUG_SIM_
		printf("\tSlave read %u from SCLK\n", bit);
#endif
		return bit;
	}

	return LOW;
}

uint8_t SIM_read_from_SS(sim_driver* driver) {
	if (driver) {
		int     failure = pthread_mutex_lock(&driver->SS_mutex);
		uint8_t bit;
		if (failure) {
			return LOW;
		}
		bit = (driver->SS_bit == HIGH || driver->SS_bit == LOW) ? driver->SS_bit : ((driver->SS_bit) ? HIGH : LOW);
		pthread_mutex_unlock(&driver->SS_mutex);
#ifdef _DEBUG_SIM_
		printf("\tSlave read %u from SS\n", bit);
#endif
		return bit;
	}

	return LOW;
}

// ==

static void s_SIM_reset_to_ready(sim_driver* driver) {
	if (driver) {
		driver->current_command = SIM_IO_READY;
		driver->current_address = 0;
		driver->currently_accessing_extended = 0;
		driver->current_output_byte = driver->chip_status;
		driver->current_input_byte = 0;
	}
}

void SIM_do_command(sim_driver* driver) {
	if (driver) {
		uint8_t command_portion;
		uint8_t address_portion;

		switch (driver->current_command) {
		case SIM_IO_READY:
			// interpret input byte as command
			address_portion = driver->current_input_byte & 0x3f;
			command_portion = driver->current_input_byte & 0xc0;
			if (address_portion < STANDARD_REGISTER_SPACE) {
				// This is a standard register read or write
				driver->current_address = address_portion;

				if ((command_portion & BIT_7) == SPI_READ) {
					// Current output byte should be the register to be read
					if (address_portion <= STANDARD_REGISTER_SPACE) {
						driver->current_output_byte = driver->standard_registers[address_portion];
#ifdef _DEBUG_SIM_
						printf("\t\tCurrent output byte set to standard_registers[0x%x]=0x%x\n", address_portion, driver->current_output_byte);
#endif
					}
					else {
						driver->current_output_byte = 0;
					}

					// Update state
					if ((command_portion & BIT_6) == SPI_SINGLE) {
						// Single-Read
						driver->current_command = SIM_IO_SINGLE_REGISTER_READ;
					}
					else { // SPI_BURST
						driver->current_command = SIM_IO_BURST_REGISTER_READ;
					}
				}
				else { // SPI_WRITE
					driver->current_output_byte = driver->chip_status;
					// Update state
					if ((command_portion & BIT_6) == SPI_SINGLE) {
						driver->current_command = SIM_IO_SINGLE_REGISTER_WRITE;
					}
					else { // SPI_BURST
						driver->current_command = SIM_IO_BURST_REGISTER_WRITE;
					}
				}
			}
			else if (address_portion == EXTENDED_REGISTER_SPACE_ADDRESS) {
				driver->current_command = SIM_IO_EXTENDED_SPACE;
				driver->currently_accessing_extended = 1;
			}
			else if (address_portion >= STROBE_ADDRESS_START && command_portion <= STROBE_ADDRESS_END) {
				// do probe effects, and go back to ready
				s_SIM_reset_to_ready(driver);
			}
			else if (address_portion == DIRECT_FIFO_ADDRESS) {

			}
			else /* if (command_portion == STANDARD_FIFO_ADDRESS)*/ {

			}
			break;
		case SIM_IO_SINGLE_REGISTER_READ:
			// ignore input byte, and go back to ready
			s_SIM_reset_to_ready(driver);
			break;
		case SIM_IO_SINGLE_REGISTER_WRITE:
			// write input byte to register pointed to
			// by last input byte
			if (driver->currently_accessing_extended) {
				if (driver->current_address < EXTENDED_REGISTER_SPACE) {
					driver->extended_registers[driver->current_address] = driver->current_input_byte;
#ifdef _DEBUG_SIM_
					printf("\t\tWrote 0x%x to extended_registers[0x%x]\n", driver->current_input_byte, driver->current_address);
#endif
				}
			}
			else {
				if (driver->current_address < STANDARD_REGISTER_SPACE) {
					driver->standard_registers[driver->current_address] = driver->current_input_byte;
#ifdef _DEBUG_SIM_
					printf("\t\tWrote 0x%x to standard_registers[0x%x]\n", driver->current_input_byte, driver->current_address);
#endif
				}
			}
			// go back to ready
			s_SIM_reset_to_ready(driver);
			break;
		case SIM_IO_BURST_REGISTER_READ:
			// ignore input byte and change output to next register
			if (driver->currently_accessing_extended) {
				if (driver->current_address < EXTENDED_REGISTER_SPACE - 1) {
					driver->current_address++;
				}
				if (driver->current_address < EXTENDED_REGISTER_SPACE) {
					driver->current_output_byte = driver->extended_registers[driver->current_address];	
				}
			}
			else {
				if (driver->current_address < STANDARD_REGISTER_SPACE - 1) {
					driver->current_address++;
				}
				if (driver->current_address < STANDARD_REGISTER_SPACE) {
					driver->current_output_byte = driver->standard_registers[driver->current_address];
				}
			}
			break;
		case SIM_IO_BURST_REGISTER_WRITE:
			// write input byte to register and change address to next register
			if (driver->currently_accessing_extended) {
				if (driver->current_address < EXTENDED_REGISTER_SPACE) {
					driver->extended_registers[driver->current_address] = driver->current_input_byte;
					if (driver->current_address < EXTENDED_REGISTER_SPACE - 1) {
						driver->current_address++;
					}
				}
			}
			else {
				if (driver->current_address < STANDARD_REGISTER_SPACE) {
					driver->standard_registers[driver->current_address] = driver->current_input_byte;
					if (driver->current_address < STANDARD_REGISTER_SPACE - 1) {
						driver->current_address++;
					}
				}				
			}
			break;
		case SIM_IO_EXTENDED_SPACE:
			// input byte is
			command_portion = driver->current_input_byte & 0xd0;
			address_portion = driver->current_input_byte & 0x3f;
			// Set address
			driver->current_address = address_portion;
			if ((command_portion & BIT_7) == SPI_READ) {
				// Output register contents
				/*if (address_portion <= EXTENDED_REGISTER_SPACE) { */
					driver->current_output_byte = driver->extended_registers[address_portion];
#ifdef _DEBUG_SIM_
					printf("\t\tCurrent output byte set to extended_registers[0x%x]=0x%x\n", address_portion, driver->current_output_byte);
#endif					
				/*}
				else {
					driver->current_output_byte = 0;
				}*/
				// Update command
				if ((command_portion & BIT_6) == SPI_SINGLE) {
					driver->current_command = SIM_IO_SINGLE_REGISTER_READ;
				}
				else { // SPI_BURST
					driver->current_command = SIM_IO_BURST_REGISTER_READ;
				}
			}
			else { // SPI_WRITE
				driver->current_output_byte = driver->chip_status;
				if ((command_portion & BIT_6) == SPI_SINGLE) {
					driver->current_command = SIM_IO_SINGLE_REGISTER_WRITE;
				}
				else { // SPI_BURST
					driver->current_command = SIM_IO_BURST_REGISTER_WRITE;
				}
			}
			break;
		case SIM_IO_SINGLE_RX_FIFO:
			// not implemented yet
			break;
		case SIM_IO_SINGLE_TX_FIFO:
			break;
		case SIM_IO_BURST_RX_FIFO:
			break;
		case SIM_IO_BURST_TX_FIFO:
			break;
		default:
			s_SIM_reset_to_ready(driver);
			break;
		}
	}
}

void SIM_do_on_SCLK(uint8_t hiOrLo, sim_driver* driver) {
	if (driver) {
		if (hiOrLo == HIGH && driver->last_clock_value == LOW) {
			uint8_t ss = SIM_read_from_SS(driver);
			if (ss == LOW) {
#ifdef _DEBUG_SIM_
				printf("\t\tHit rising clock edge and IO is active\n");
#endif
				// then io is active, so write MISO and read MOSI
				hiOrLo = (driver->current_output_byte & driver->current_bit) ? HIGH : LOW;
				SIM_write_to_MISO(hiOrLo, driver);
				hiOrLo = SIM_read_from_MOSI(driver);
				if (hiOrLo == HIGH) {
					driver->current_input_byte |= driver->current_bit;
				}
#ifdef _DEBUG_SIM_
				printf("\t\tSlave input byte is currently 0x%x\n", driver->current_input_byte);
#endif

				// move the current bit
				if (driver->current_bit == BIT_0) {
					// then do an action and go back to BIT_7
					SIM_do_command(driver);
					driver->current_bit = BIT_7;
					driver->current_input_byte = 0;
				}
				else {
					driver->current_bit >>= 1;
				}
			} 
			// else do nothing
		}
		driver->last_clock_value = hiOrLo;
	}
}

void SIM_do_on_SS(uint8_t hiOrLo, sim_driver* driver) {
	if (driver) {
		if (hiOrLo == HIGH) {
			// io is complete, so reset output byte and bit
			s_SIM_reset_to_ready(driver);
			driver->current_bit = BIT_7;
		}
	}
}

// sim_iface.h : exported functions
// ================================

void SIM_write_to_MOSI(uint8_t hiOrLo, sim_driver_handle dh) {
	sim_driver* driver = (sim_driver*)dh;

	if (driver) {
		int failure = pthread_mutex_lock(&driver->MOSI_mutex);
		if (failure) {
			return;
		}
		driver->MOSI_bit = hiOrLo;
		pthread_mutex_unlock(&driver->MOSI_mutex);
	}
}

uint8_t SIM_read_from_MISO(sim_driver_handle dh) {
	sim_driver* driver = (sim_driver*)dh;

	if (driver) {
		int     failure = pthread_mutex_lock(&driver->MISO_mutex);
		uint8_t bit;
		if (failure) {
			return LOW;
		}
		bit = (driver->MISO_bit == HIGH || driver->MISO_bit == LOW) ? driver->MISO_bit : ((driver->MISO_bit) ? HIGH : LOW);
		pthread_mutex_unlock(&driver->MISO_mutex);
		return bit;
	}

	return LOW;
}

void SIM_write_to_SCLK(uint8_t hiOrLo, sim_driver_handle dh) {
	sim_driver* driver = (sim_driver*)dh;

	if (driver) {
		int failure = pthread_mutex_lock(&driver->SCLK_mutex);
		if (failure) {
			return;
		}
		driver->SCLK_bit = hiOrLo;

		SIM_do_on_SCLK(hiOrLo, driver);

		pthread_mutex_unlock(&driver->SCLK_mutex);
	}	
}

void SIM_write_to_SS(uint8_t hiOrLo, sim_driver_handle dh) {
	sim_driver* driver = (sim_driver*)dh;

	if (driver) {
		int failure = pthread_mutex_lock(&driver->SS_mutex);
		if (failure) {
			return;
		}
		driver->SS_bit = hiOrLo;

		SIM_do_on_SS(hiOrLo, driver);

		pthread_mutex_unlock(&driver->SS_mutex);
	}	
}

