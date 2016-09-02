/**
 * Keil project example for EXTI interrupts
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
#include "tm_stm32_exti.h"
#include "tm_stm32_disco.h"

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Attach EXTI pin, enable both edges because of different boards support */
	if (TM_EXTI_Attach(TM_DISCO_BUTTON_PORT, TM_DISCO_BUTTON_PIN, TM_EXTI_Trigger_Rising_Falling) == TM_EXTI_Result_Ok) {
		/* Turn on green LED */
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		/* Turn on RED led */
		TM_DISCO_LedOn(LED_RED);
	}
	
	while (1) {
		/* Do nothing, wait user to press button */
	}
}

/* Handle all EXTI lines */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Check proper line */
	if (GPIO_Pin == TM_DISCO_BUTTON_PIN) {
		/* Toggle pin only if button is pressed */
		if (TM_DISCO_ButtonPressed()) {
			/* Toggle LEDs if interrupt on button line happens */
			TM_DISCO_LedToggle(LED_ALL);
		}
	}
}

