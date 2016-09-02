/**
 * Keil project example for ONEWIRE protocol
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
#include "tm_stm32_onewire.h"

/* Onewire structure */
TM_OneWire_t OW;

int main(void) {	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init ONEWIRE port on PB4 pin */
	TM_OneWire_Init(&OW, GPIOB, GPIO_PIN_4);
	
	/* Check if any device is connected */
	if (TM_OneWire_First(&OW)) {
		/* Set LED GREEN */
		TM_DISCO_LedOn(LED_GREEN);
		
		/* Search for next devices */
		do {
			/* Read ROM from device */
			//TM_OneWire_GetFullROM(&OW, &array_8_bytes[0]);
		} while (TM_OneWire_Next(&OW));
	} else {
		/* Set LED RED */
		TM_DISCO_LedOn(LED_RED);
	}
	
	while (1) {

	}
}

