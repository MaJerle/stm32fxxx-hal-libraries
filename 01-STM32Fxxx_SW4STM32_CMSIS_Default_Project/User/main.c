/**
 * SW4STM32 project template
 *
 * Before you start, select your target
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in target options
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();

	/* Init HAL layer */
	HAL_Init();

	/* Init leds */
	TM_DISCO_LedInit();

	/* Init button */
	TM_DISCO_ButtonInit();

	/* Init delay */
	TM_DELAY_Init();
	
	while (1) {
		/* Toggle leds */
		TM_DISCO_LedToggle(LED_ALL);

		/* Delay 500ms */
		Delayms(500);
	}
}

