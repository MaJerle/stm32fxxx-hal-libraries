/**
 * Keil project example for DMA library
 *
 * This example works for STM32F4xx and STM32F7xx devices only!
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
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_usart_dma.h"
#include "tm_stm32_spi.h"
#include "tm_stm32_spi_dma.h"

/* Create USART working buffer */
uint8_t SPI_TX[15], SPI_RX[15];

/* Get TX buffer size */
#define TX_SIZE    (sizeof(SPI_TX) / sizeof(SPI_TX[0]))

/* Set SPI */
#if defined(STM32F7_DISCOVERY)
	#define SPI               SPI2 /* SPI2 is on Arduino header */
	#define SPI_PP            TM_SPI_PinsPack_4
#else
	#define SPI               SPI1 /* SPI1 is on Arduino header */
	#define SPI_PP            TM_SPI_PinsPack_1
#endif

/* USART for serial port */
#if defined(STM32F7_DISCOVERY)
	#define USART             USART1 /* USART 1 on STM32F7-Discovery */
#else
	#define USART             USART2 /* USART 2 on Nucleo boards for F4 series */
#endif

int main(void) {
	uint8_t i;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init USART, check USART lib description for pinout */
	TM_USART_Init(USART, TM_USART_PinsPack_1, 921600);
	
	/* Send notification */
	TM_USART_Puts(USART, "This is SPI DMA demo. It sends data on MOSI and reads the same data on MISO.\r\n");
	TM_USART_Puts(USART, "Connect MOSI on MISO pins together for testing.\r\n");
	
	/* Init SPI */
	TM_SPI_Init(SPI, SPI_PP);
	
	/* Init SPI DMA */
	TM_SPI_DMA_Init(SPI);
	
	/* Enable interrupts */
	TM_SPI_DMA_EnableInterrupts(SPI);
	
	/* Set some values for TX */
	for (i = 0; i < TX_SIZE; i++) {
		SPI_TX[i] = i << 3;
	}
	
	/* Send via SPI with DMA */
	TM_SPI_DMA_Transmit(SPI, SPI_TX, SPI_RX, TX_SIZE);
	
	/* Wait till SPI sends data */
	while (TM_SPI_DMA_Transmitting(SPI));
	
	/* Check memory result */
	if (memcmp(SPI_TX, SPI_RX, TX_SIZE) == 0) {
		TM_USART_Puts(USART, "SPI memory OK\r\n");
	} else {
		TM_USART_Puts(USART, "SPI memory FAIL\r\n");
	}
	
	/* Do nothing */
	while (1) {
		
	}
}

/* DMA transfer complete callback */
void TM_DMA_TransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* Check for which stream we were successful */
	if (DMA_Stream == TM_SPI_DMA_GetStreamTX(SPI)) {
		
	}
	/* Check for which stream we were successful */
	if (DMA_Stream == TM_SPI_DMA_GetStreamRX(SPI)) {
		TM_USART_Puts(USART, "DMA received data!\n");
	}
}
