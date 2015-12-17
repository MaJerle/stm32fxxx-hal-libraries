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
	
	/* Init USART, TX: PC6, RX: PC7, 921600 bauds */
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	
	/* Put test string */
	TM_USART_Puts(USART2, "Hello world\n");
	
	while (1) {
		/* Check if we have string "OK" in USART6 buffer */
		if (TM_USART_FindString(USART2, "OK")) {
			/* Send data back from buffer */
			while (!TM_USART_BufferEmpty(USART2)) {
				/* Send to computer */
				TM_USART_Putc(USART2, TM_USART_Getc(USART6));
			}
		}
	}
}

