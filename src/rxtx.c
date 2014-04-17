
#include "error.h"
#include "spi.h"
#include "bang_registers.h"
#include "rxtx.h"

#define RXTX_RX SPI_READ
#define RXTX_TX SPI_WRITE

#define STANDARD_FIFO_ADDRESS 0x3f
#define DIRECT_FIFO_ADDRESS 0x3e

tcvr_error_t RX_queue_len(uint8_t* len, uint8_t* status) {
	return REGISTER_read(NUM_RX_BYTES, len, status);
}

tcvr_error_t TX_queue_len(uint8_t* len, uint8_t* status) {
	return REGISTER_read(NUM_TX_BYTES, len, status);
}

tcvr_error_t RX_dequeue(uint8_t* data, uint8_t* status) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      addr = 0;
	uint8_t      byt;

	// Check if the RX FIFO is empty
	err = RX_queue_len(&byt, status);
	if (err != ERROR_NONE) {
		return err;
	}
	if (byt == 0) {
		return ERROR_RXTX_DEQUEUING_FROM_EMPTY_RX_FIFO;
	}

	// RX FIFO Address
	addr = (RXTX_RX | SPI_SINGLE) | STANDARD_FIFO_ADDRESS;

	SPI_start_transaction();

	// Transfer address
	byt = SPI_transfer_byte(addr);
	if (status) {
		*status = byt;
	}

	// Read dequeued byte
	byt = SPI_transfer_byte(0);
	if (data) {
		*data = byt;
	}

	SPI_stop_transaction();
	return ERROR_NONE;
}

tcvr_error_t TX_enqueue(uint8_t data, uint8_t* status) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      addr = 0;
	uint8_t      byt;

	// Check if the TX FIFO is full
	err = TX_queue_len(&byt, status);
	if (err != ERROR_NONE) {
		return err;
	}
	if (byt == TRANSCEIVER_FIFO_SIZE) {
		return ERROR_RXTX_ENQUEUING_TO_FULL_TX_FIFO;
	}

	// TX FIFO Address
	addr = (RXTX_TX | SPI_SINGLE) | STANDARD_FIFO_ADDRESS;

	SPI_start_transaction();

	// Transfer address
	byt = SPI_transfer_byte(addr);
	if (status) {
		*status = byt;
	}

	// Enqueue byte
	SPI_transfer_byte(data);

	SPI_stop_transaction();
	return ERROR_NONE;
}

tcvr_error_t RX_burst_dequeue(uint8_t* data_arr, uint8_t bytes_requested,
                              uint8_t* bytes_received, uint8_t* status) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      addr = 0;
	uint8_t      rx_fifo_len;
	uint8_t      i;

	// Check RX FIFO num items enqueued
	err = RX_queue_len(&rx_fifo_len, status);
	if (err != ERROR_NONE) {
		return err;
	}
	// Limit bytes_requested to amount actually in queue
	bytes_requested = (rx_fifo_len < bytes_requested) ? rx_fifo_len : bytes_requested;

	// RX FIFO Address
	addr = (RXTX_RX | SPI_BURST) | STANDARD_FIFO_ADDRESS;

	SPI_start_transaction();

	// Transfer address
	i = SPI_transfer_byte(addr);
	if (status) {
		*status = i;
	}

	// Read dequeued bytes
	if (data_arr) {
		for (i = 0; i < bytes_requested; i++) {
			data_arr[i] = SPI_transfer_byte(0);
		}	
	}
	else { // simply drain queue, without outputting data
		for (i = 0; i < bytes_requested; i++) {
			SPI_transfer_byte(0);
		}	
	}

	SPI_stop_transaction();
	return ERROR_NONE;
}

tcvr_error_t TX_burst_enqueue(uint8_t* data_arr, uint8_t data_len, uint8_t* status) {
	tcvr_error_t err = ERROR_NONE;
	uint8_t      addr = 0;
	uint8_t      tx_fifo_len;
	uint8_t      i;

	if (!data_arr) {
		return 0;
	}

	// Check TX FIFO num items enqueued
	err = TX_queue_len(&tx_fifo_len, status);
	if (err != ERROR_NONE) {
		return err;
	}
	if (tx_fifo_len + data_len > TRANSCEIVER_FIFO_SIZE) {
		return ERROR_RXTX_ENQUEUING_TO_FULL_TX_FIFO;
	}

	// TX FIFO Address
	addr = (RXTX_TX | SPI_BURST) | STANDARD_FIFO_ADDRESS;

	SPI_start_transaction();

	// Transfer address
	i = SPI_transfer_byte(addr);
	if (status) {
		*status = i;
	}

	// Enqueue bytes
	for (i = 0; i < data_len; i++) {
		SPI_transfer_byte(data_arr[i]);
	}

	SPI_stop_transaction();
	return ERROR_NONE;
}

