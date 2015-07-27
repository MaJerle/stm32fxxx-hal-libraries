/**
 * Keil project example for custom pins initialization
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
	
	/* Init USART, Pins not initialized yet, 921600 bauds */
	TM_USART_Init(USART6, TM_USART_PinsPack_Custom, 921600);
	
	/* Put test string */
	TM_USART_Puts(USART6, "Hello world\n");
	
	while (1) {
		/* Check if anything received */
		while (!TM_USART_BufferEmpty(USART6)) {
			/* Send data back from buffer */
			TM_USART_Putc(USART6, TM_USART_Getc(USART6));
		}
	}
}

/* USART Custom pins callback function */
void TM_USART_InitCustomPinsCallback(USART_TypeDef* USARTx, uint16_t AlternateFunction) { 
	/* Check for proper USART */
	if (USARTx == USART6) {
		/* Init pins for USART 6 */
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_6 | GPIO_PIN_7, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Fast, AlternateFunction);
	}
}

