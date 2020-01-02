/**
 * Keil project example for CPU load monitor
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
#include "tm_stm32_cpu_load.h"
#include "tm_stm32_usart.h"
	
/* CPU load structure */
TM_CPULOAD_t CPU_LOAD;
	
int main(void) {
	__IO uint32_t i;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init CPU load monitor */
	TM_CPULOAD_Init(&CPU_LOAD);
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init USART6, TX: PC6, RX: PC7, 921600 bauds */
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 921600);
	
	while (1) {
		/* Check if CPU LOAD variable is updated */
		if (CPU_LOAD.Updated) {
			/* Print to user */
			printf("W: %u; S: %u; Load: %5.2f\n", CPU_LOAD.WCNT, CPU_LOAD.SCNT, CPU_LOAD.Load);
		}
		
		/* Toggle leds */
		TM_DISCO_LedToggle(LED_ALL);
		
		/* If button pressed, do some useless counting */
		if (TM_DISCO_ButtonPressed()) {
			/* Count something to waste some time before entering to sleep mode */
			i = 0;
			while (i++ < 0x1FFF);
		}
		
		/* Go low power mode, sleep mode until next interrupt (Systick or anything else), measure CPU load */
		TM_CPULOAD_GoToSleepMode(&CPU_LOAD);
	}
}

/* Handle printf functionality */
int fputc(int ch, FILE* fil) {
	/* Send character over USART */
	TM_USART_Putc(USART6, ch);
	
	/* Return character */
	return ch;
}
