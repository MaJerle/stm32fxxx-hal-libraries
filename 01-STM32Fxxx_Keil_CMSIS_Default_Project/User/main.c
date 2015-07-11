/**
 * Keil project template
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx Keil packs version 2.4.0 or greater required
 * @stdperiph STM32F4xx Standard peripheral drivers version 1.5.0 or greater required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_exti.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"

int main(void) {
	__IO uint32_t i;
	
	/* Init system clock */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init delay functions */
	TM_DELAY_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	while (1) {
		/* If button pressed */
		if (TM_DISCO_ButtonPressed()) {
			/* Turn on ALL leds */
			TM_DISCO_LedOn(LED_ALL);
		} else {
			/* Turn off ALL leds */
			TM_DISCO_LedOff(LED_ALL);
		}
	}
}

