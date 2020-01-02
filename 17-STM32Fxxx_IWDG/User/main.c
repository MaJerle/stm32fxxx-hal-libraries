/**
 * Keil project example for IWDG - Independent Watchdog timer
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
#include "tm_stm32_iwdg.h"
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Check if system reset because of IWDG */
	if (TM_IWDG_Init(TM_IWDG_Timeout_4s)) {
		/* System reset was done because of IWDG timer */
		TM_DISCO_LedOn(LED_RED);
	} else {
		/* No IWDG */
		TM_DISCO_LedOn(LED_GREEN);
	}
	
	/* If there is no LED active (F7-Discovery, Nucleo boards), */
	/* then system reset occurred because of IWDG */
	
	while (1) {
		/* Check for button */
		if (TM_DISCO_ButtonPressed()) {
			/* Wait till pressed */
			/* If pressed more than 4 seconds in a row, system will reset because of IWDG timer */
			while (TM_DISCO_ButtonPressed());
		}
		
		/* Reset watchdog */
		TM_IWDG_Reset();
	}
}

