#ifndef _TRANSCEIVER_ERROR_H_
#define _TRANSCEIVER_ERROR_H_

/*
	These defines set the base error code value
	for the enums of each error source
*/
#define ERROR_NONE           0
#define ERROR_MISC           0x0100
#define ERROR_BITS           0x0200
#define ERROR_GPIO           0x0300
#define ERROR_SPI            0x0400
#define ERROR_STROBE         0x0500
#define ERROR_BANG_REGISTERS 0x0600
#define ERROR_RXTX           0x0700

typedef int tcvr_error_t;

enum miscellaneous_error_e {
	ERROR_NULL_POINTER = ERROR_MISC + 1,
	ERROR_PARAMETER_OUT_OF_RANGE,
	ERROR_OUT_OF_MEMORY
};

/* No bits errors defined yet */
/*enum bits_error_e {
	ERROR_BITS_??? = ERROR_BITS + 1
};*/

/* No bits errors defined yet */
/*enum gpio_error_e {
	ERROR_GPIO_??? = ERROR_GPIO + 1
};*/

/* No spi errors defined yet */
/*enum spi_error_e {
	ERROR_SPI_??? = ERROR_SPI + 1
};*/

enum strobe_error_e {
	ERROR_STROBE_INVALID_NAME = ERROR_STROBE + 1
};

enum register_error_e {
	ERROR_REGISTER_INVALID_NAME = ERROR_BANG_REGISTERS + 1
};

enum rxtx_error_e {
	ERROR_RXTX_DEQUEUING_FROM_EMPTY_RX_FIFO = ERROR_RXTX + 1,
	ERROR_RXTX_ENQUEUING_TO_FULL_TX_FIFO
};

#endif
