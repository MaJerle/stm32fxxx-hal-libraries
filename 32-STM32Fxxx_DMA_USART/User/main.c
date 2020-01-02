/**
 * Keil project example for DMA library
 *
 * This example works for STM32F4xx and STM32F7xx devices only!
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
#include "tm_stm32_usart_dma.h"

/* Create USART working buffer */
char USART_Buffer[100] = "Hello via USART6 with TX DMA\n";

/* USART for serial port */
#if defined(STM32F7_DISCOVERY)
	#define USART             USART1 /* USART 1 on STM32F7-Discovery */
#else
	#define USART             USART2 /* USART 2 on Nucleo boards for F4 series */
#endif

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init USART, check USART lib description for pinout */
	TM_USART_Init(USART, TM_USART_PinsPack_1, 921600);
	
	/* Say string without DMA */
	TM_USART_Puts(USART, "Hello via USART without DMA\n");
	
	/* Init TX DMA for USART */
	TM_USART_DMA_Init(USART);
	
	/* Enable interrupts for TX DMA */
	TM_USART_DMA_EnableInterrupts(USART);
	
	/* Send data with DMA */
	TM_USART_DMA_Send(USART, (uint8_t *)USART_Buffer, strlen(USART_Buffer));
	
	/* Wait till DMA works */
	/* You can do other stuff here instead of waiting for DMA to end */
	while (TM_USART_DMA_Transmitting(USART));
	
	while (1) {
		/* If any string arrived over USART */
		/* Expecting "\n" at the end of string from USART terminal or any other source */
		if (TM_USART_Gets(USART, USART_Buffer, sizeof(USART_Buffer))) {
			/* Send it back over DMA */
			TM_USART_DMA_Send(USART, (uint8_t *)USART_Buffer, strlen(USART_Buffer));

			/* Wait till DMA works */
			/* You can do other stuff here instead of waiting for DMA to end */
			while (TM_USART_DMA_Transmitting(USART));
		}
	}
}

/* DMA transfer complete callback */
void TM_DMA_TransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* Check for which stream we were successful */
	if (DMA_Stream == TM_USART_DMA_GetStreamTX(USART)) {
		/* DMA transfer has finished */
		/* We also have to wait for USART to finish transmitting last byte from DMA */
		while (TM_USART_DMA_Transmitting(USART));
		
		/* Send notification to the same USART port with polling mode */
		TM_USART_Puts(USART, "DMA has sent data!\n");
	}
}
