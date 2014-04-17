CC=gcc
CFLAGS=-Wall -Werror -g -Wextra -Wno-unused-parameter

all: bits.o sim_gpio.o spi.o bang_registers.o strobe.o status_byte.o rxtx.o xosc.o freq_synth_config.o chip_reset.o sim.o simulate

simulate: ../error.h bits.o sim_gpio.o spi.o bang_registers.o strobe.o status_byte.o rxtx.o xosc.o freq_synth_config.o chip_reset.o sim.o main.c
	$(CC) -lpthread bits.o sim_gpio.o spi.o bang_registers.o strobe.o status_byte.o rxtx.o xosc.o freq_synth_config.o chip_reset.o sim.o main.c -o simulate

bits.o: ../bits.h ../bits.c
	$(CC) $(CFLAGS) -c ../bits.c

sim_gpio.o: ../gpio.h sim_iface.h sim.h sim_gpio.c
	$(CC) $(CFLAGS) -c sim_gpio.c

spi.o: ../gpio.h ../bits.h ../spi.h ../spi.c
	$(CC) $(CFLAGS) -c ../spi.c

bang_registers.o: ../error.h ../bits.h ../spi.h ../bang_registers.h ../bang_registers.c
	$(CC) $(CFLAGS) -c ../bang_registers.c

strobe.o: ../error.h ../bits.h ../spi.h ../strobe.h ../strobe.c
	$(CC) $(CFLAGS) -c ../strobe.c

status_byte.o: ../bits.h ../bang_registers.h ../status_byte.h ../status_byte.c
	$(CC) $(CFLAGS) -c ../status_byte.c

rxtx.o: ../error.h ../spi.h ../bang_registers.h ../rxtx.h ../rxtx.c
	$(CC) $(CFLAGS) -c ../rxtx.c

xosc.o: ../bits.h ../bang_registers.h ../xosc.h ../xosc.c
	$(CC) $(CFLAGS) -c ../xosc.c

freq_synth_config.o: ../error.h ../bits.h ../bang_registers.h ../freq_synth_config.h ../freq_synth_config.c
	$(CC) $(CFLAGS) -c ../freq_synth_config.c

chip_reset.o: ../error.h ../strobe.h ../chip_reset.h ../chip_reset.c
	$(CC) $(CFLAGS) -c ../chip_reset.c

sim.o: ../bits.h ../gpio.h sim_iface.h sim.h sim.c
	$(CC) $(CFLAGS) -c sim.c 

clean:
	rm -rf simulate bits.o sim_gpio.o spi.o bang_registers.o strobe.o status_byte.o rxtx.o xosc.o freq_synth_config.o chip_reset.o sim.o
