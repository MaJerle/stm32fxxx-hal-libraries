/**
 * Keil project example for USART string receive and send
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
#include "tm_stm32_usart.h"

/* Buffer array */
char mybuffer[100];

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init USART, TX: PC6, RX: PC7, 921600 bauds */
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 921600);
	
	/* Put test string */
	TM_USART_Puts(USART6, "Hello world\n");
	
	while (1) {
		/* Check if string received */
		/* Waiting for \n at the end of string */
		if (TM_USART_Gets(USART6, mybuffer, sizeof(mybuffer))) {
			/* Send string back */
			TM_USART_Puts(USART6, mybuffer);
		}
	}
}

