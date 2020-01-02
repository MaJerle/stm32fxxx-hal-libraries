/**
 * Keil project example for software timers with delay library
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

/* Create pointers for 2 timers */
TM_DELAY_Timer_t* SWTIM1;
TM_DELAY_Timer_t* SWTIM2;

/* References for function callbacks */
void SWTIM1_Callback(TM_DELAY_Timer_t* SWTIM, void* UserParameters);
void SWTIM2_Callback(TM_DELAY_Timer_t* SWTIM, void* UserParameters);

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
	
	/* Create software timers */
	
	/* 100ms with autoreload enabled, and start after it is created */
	SWTIM1 = TM_DELAY_TimerCreate(100, 1, 1, SWTIM1_Callback, NULL);
	
	/* 150ms with autoreload enabled, and start after it is created */
	SWTIM2 = TM_DELAY_TimerCreate(150, 1, 1, SWTIM2_Callback, NULL);
	
	while (1) {
		/* Do nothing */
		/* Everything is performed in stm32fxxx_it.c function where callback functions are called for software timers */
	}
}

/* Callback function for SWTIM1 */
void SWTIM1_Callback(TM_DELAY_Timer_t* SWTIM, void* UserParameters) {
	/* Toggle green led */
	TM_DISCO_LedToggle(LED_GREEN);
}

/* Callback function for SWTIM2 */
void SWTIM2_Callback(TM_DELAY_Timer_t* SWTIM, void* UserParameters) {
	/* Toggle red led */
	TM_DISCO_LedToggle(LED_RED);
}
