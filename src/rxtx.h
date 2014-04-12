#ifndef _TRANSCEIVER_RX_TX_H_
#define _TRANSCEIVER_RX_TX_H_

#include <stdint.h>

/*
	There are two FIFOs on the transceiver chip,
	and RX FIFO and a TX FIFO. Each can contain
	TRANSCEIVER_FIFO_SIZE bytes.
*/
#define TRANSCEIVER_FIFO_SIZE 128

#define DIRECT_FIFO_ADDRESS 0x3e
#define STANDARD_FIFO_ADDRESS 0x3f

/*
	Outputs number of bytes in RX FIFO and reads chip
	status.
	Returns 1 if successful, 0 otherwise.
*/
int RX_queue_len(uint8_t* len, uint8_t* status);

/*
	Outputs number of bytes in TX FIFO and reads chip
	status.
	Returns 1 if successful, 0 otherwise.
*/
int TX_queue_len(uint8_t* len, uint8_t* status);

/*
	Dequeues a single byte from the RX FIFO and reads
	chip status.
	Returns 1 if byte is dequeued, returns 0 if
	no byte existed.
*/
int RX_dequeue(uint8_t* data, uint8_t* status);
/*
	Enqueues a single byte into the TX FIFO and reads
	chip status.
	Returns 1 if byte is enqueued, returns 0 if
	FIFO is full.
*/
int TX_enqueue(uint8_t data, uint8_t* status);

/*
	Dequeues a series of contiguous bytes from the RX FIFO, and
	reads chip status. Outputs number of bytes actually dequeued.
	Returns 1 if successful, 0 otherwise.

	Required: bytes_requested must be <= data_arr length

	Errors:   bytes_received must be non-null
*/
int RX_burst_dequeue(uint8_t* data_arr, uint8_t bytes_requested, uint8_t* bytes_received, uint8_t* status);
/*
	Enqueues a series of contiguous bytes into the TX FIFO, and
	reads chip status.
	Returns 1 if successful, 0 otherwise.

	Required: data_len must be <= data_arr length,
	
	Errors:   data_len must be <= TRANSCEIVER_FIFO_SIZE - current TX queue length.
*/
int TX_burst_enqueue(uint8_t* data_arr, uint8_t data_len, uint8_t* status);

#endif
