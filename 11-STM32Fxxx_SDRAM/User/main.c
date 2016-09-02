/**
 * Keil project example for SDRAM
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_sdram.h"
#include "string.h"

/* Data to write */
uint32_t Write[32];
uint32_t Read[32];

int main(void) {
	uint8_t i = 0;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init SDRAM */
	TM_SDRAM_Init();
	
	/* Fill write data */
	for (i = 0; i < 32; i++) {
		Write[i] = 1 << i;
		
		/* Write to SDRAM */
		TM_SDRAM_Write32(4 * i, Write[i]);
	}

	/* Read data from SDRAM */
	for (i = 0; i < 32; i++) {
		Read[i] = TM_SDRAM_Read32(4 * i);
	}
	
	/* Memory compare */
	if (memcmp((uint8_t *)Read, (uint8_t *)Write, sizeof(Read)) == 0) {
		/* SDRAM is OK */
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		/* SDRAM failed */
		TM_DISCO_LedOn(LED_RED);
	}
	
	while (1) {
		
	}
}

