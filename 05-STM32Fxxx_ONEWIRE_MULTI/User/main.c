/**
 * Keil project example for ONEWIRE protocol on multiple GPIO pins
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen MAJERLE
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
#include "tm_stm32_onewire.h"

/* Onewire structures */
TM_OneWire_t OW1;
TM_OneWire_t OW2;

int main(void) {	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init ONEWIRE port on PB4 and PC6 pins */
	TM_OneWire_Init(&OW1, GPIOB, GPIO_PIN_4);
	TM_OneWire_Init(&OW2, GPIOC, GPIO_PIN_6);
	
	/* Check if any device is connected on OW port 1 */
	if (TM_OneWire_First(&OW1)) {
		/* Set LED green */
		TM_DISCO_LedOn(LED_GREEN);
		
		/* Search for next devices */
		do {
			/* Read rom from device */
			//TM_OneWire_GetFullROM(&OW1, &array_8_bytes[0]);
		} while (TM_OneWire_Next(&OW1));
	}
	
	/* Check if any device is connected on OW port 2 */
	if (TM_OneWire_First(&OW2)) {
		/* Set LED red */
		TM_DISCO_LedOn(LED_RED);
		
		/* Search for next devices */
		do {
			/* Read rom from device */
			//TM_OneWire_GetFullROM(&OW2, &array_8_bytes[0]);
		} while (TM_OneWire_Next(&OW2));
	}
	
	/* Wait and do nothing */
	while (1) {

	}
}

