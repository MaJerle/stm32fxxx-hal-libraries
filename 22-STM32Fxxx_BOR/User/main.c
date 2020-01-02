/**
 * Keil project example for setting brown-out level
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
#include "tm_stm32_bor.h"
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Try to set brown-out to level 3 */
	if (TM_BOR_Set(TM_BOR_Level_3) == TM_BOR_Result_Ok) {
		/* Brown-out level is set OK */
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		/* Problems with setting brown-out detection */
		TM_DISCO_LedOn(LED_RED);
	}
	
	while (1) {
		/* Do nothing */
	}
}

