/**
 * Keil project example for Random Number Generator
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
#include "tm_stm32_delay.h"
#include "tm_stm32_rng.h"
#include "tm_stm32_usart.h"

/* Random value */
uint32_t randval;

/* Format string */
char buff[100];

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init delay */
	TM_DELAY_Init();
	
	/* Init USART6, TX: PC6, 921600 bauds */
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 921600);
	
	/* Init Random Number generator */
	TM_RNG_Init();
	
	while (1) {
		/* Get 32-bit random value */
		randval = TM_RNG_Get();
		
		/* Format string */
		sprintf(buff, "Value: 0x%08X", randval);
		
		/* Send via USART */
		TM_USART_Puts(USART6, buff);
		
		/* Delay */
		Delayms(500);
	}
}

