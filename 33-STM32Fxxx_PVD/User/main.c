/**
 * Keil project example for PVD Power Voltage Detector
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
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
#include "tm_stm32_pvd.h"
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init Power Voltage Detector with rising and falling interrupt */
	TM_PVD_Enable(TM_PVD_Level_3, TM_PVD_Trigger_Rising_Falling);
	
	while (1) {
		
	}
}

/* Handle interrupts from PVD */
void TM_PVD_Handler(uint8_t status) {
	/* Check status */
	if (status) {
		/* Power is below trigger voltage */
		TM_DISCO_LedOn(LED_ALL);
	} else {
		/* Power is above trigger voltage */
		TM_DISCO_LedOff(LED_ALL);
	}
}
