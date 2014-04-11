#ifndef _TRANSCEIVER_RX_TX_H_
#define _TRANSCEIVER_RX_TX_H_

#include <stdint.h>

#define TRANSCEIVER_FIFO_SIZE 128

int RX_queue_len(uint8_t* len, uint8_t* status);
int TX_queue_len(uint8_t* len, uint8_t* status);

#endif
