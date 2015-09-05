/**
 * Keil project example for UART peripheral
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
#include "tm_stm32_usart.h"

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
		/* Check if anything received */
		if (TM_USART_FindString(USART6, "OK")) {
			/* Send data back from buffer */
			while (!TM_USART_BufferEmpty(USART6)) {
				/* Send to computer */
				TM_USART_Putc(USART6, TM_USART_Getc(USART6));
			}
		}
	}
}

