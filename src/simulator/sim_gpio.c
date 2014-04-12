
#include "../gpio.h"
#include "sim_iface.h"
#include "sim.h"

#define _DEBUG_SIM_GPIO_

#ifdef _DEBUG_SIM_GPIO_
#include <stdio.h>
#endif

static sim_driver_handle driver = NULL;

void GPIO_write_MOSI(uint8_t hiOrLo) {
	if (driver == NULL) {
		driver = (sim_driver_handle)SIM_create_sim_driver();
	}
#ifdef _DEBUG_SIM_GPIO_
	printf("Master writing %s to MOSI..\n", (hiOrLo == HIGH) ? "HIGH" : ((hiOrLo == LOW) ? "LOW" : ((hiOrLo) ? "HIGH" : "LOW")));
#endif
	SIM_write_to_MOSI(hiOrLo, driver);
}

uint8_t GPIO_read_MISO() {
	if (driver == NULL) {
		driver = (sim_driver_handle)SIM_create_sim_driver();
	}	
#ifdef _DEBUG_SIM_GPIO_
	{
		uint8_t hiOrLo = SIM_read_from_MISO(driver);
		printf("Master read %s from MISO..\n", (hiOrLo == HIGH) ? "HIGH" : ((hiOrLo == LOW) ? "LOW" : ((hiOrLo) ? "HIGH" : "LOW")));
		return hiOrLo;
	}
#else
	return SIM_read_from_MISO(driver);
#endif	
}

void GPIO_write_SCLK(uint8_t hiOrLo) {
	if (driver == NULL) {
		driver = (sim_driver_handle)SIM_create_sim_driver();
	}
#ifdef _DEBUG_SIM_GPIO_
	printf("Master writing %s to SCLK..\n", (hiOrLo == HIGH) ? "HIGH" : ((hiOrLo == LOW) ? "LOW" : ((hiOrLo) ? "HIGH" : "LOW")));
#endif
	SIM_write_to_SCLK(hiOrLo, driver);
}

void GPIO_write_SS(uint8_t hiOrLo) {
	if (driver == NULL) {
		driver = (sim_driver_handle)SIM_create_sim_driver();
	}
#ifdef _DEBUG_SIM_GPIO_
	printf("Master writing %s to SS..\n", (hiOrLo == HIGH) ? "HIGH" : ((hiOrLo == LOW) ? "LOW" : ((hiOrLo) ? "HIGH" : "LOW")));
#endif
	SIM_write_to_SS(hiOrLo, driver);
}

