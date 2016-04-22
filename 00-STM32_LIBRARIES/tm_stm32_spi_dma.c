/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2016
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_spi_dma.h"

/* Private structure */
typedef struct {
	uint32_t TX_Channel;
	DMA_Stream_TypeDef* TX_Stream;
	uint32_t RX_Channel;
	DMA_Stream_TypeDef* RX_Stream;
	uint32_t Dummy32;
	uint16_t Dummy16;
} TM_SPI_DMA_INT_t;

/* Private variables */
#ifdef SPI1
static TM_SPI_DMA_INT_t SPI1_DMA_INT = {SPI1_DMA_TX_CHANNEL, SPI1_DMA_TX_STREAM, SPI1_DMA_RX_CHANNEL, SPI1_DMA_RX_STREAM};
#endif
#ifdef SPI2
static TM_SPI_DMA_INT_t SPI2_DMA_INT = {SPI2_DMA_TX_CHANNEL, SPI2_DMA_TX_STREAM, SPI2_DMA_RX_CHANNEL, SPI2_DMA_RX_STREAM};
#endif
#ifdef SPI3
static TM_SPI_DMA_INT_t SPI3_DMA_INT = {SPI3_DMA_TX_CHANNEL, SPI3_DMA_TX_STREAM, SPI3_DMA_RX_CHANNEL, SPI3_DMA_RX_STREAM};
#endif
#ifdef SPI4
static TM_SPI_DMA_INT_t SPI4_DMA_INT = {SPI4_DMA_TX_CHANNEL, SPI4_DMA_TX_STREAM, SPI4_DMA_RX_CHANNEL, SPI4_DMA_RX_STREAM};
#endif
#ifdef SPI5
static TM_SPI_DMA_INT_t SPI5_DMA_INT = {SPI5_DMA_TX_CHANNEL, SPI5_DMA_TX_STREAM, SPI5_DMA_RX_CHANNEL, SPI5_DMA_RX_STREAM};
#endif
#ifdef SPI6
static TM_SPI_DMA_INT_t SPI6_DMA_INT = {SPI6_DMA_TX_CHANNEL, SPI6_DMA_TX_STREAM, SPI6_DMA_RX_CHANNEL, SPI6_DMA_RX_STREAM};
#endif

/* Private functions */
static TM_SPI_DMA_INT_t* TM_SPI_DMA_INT_GetSettings(SPI_TypeDef* SPIx);
	
void TM_SPI_DMA_Init(SPI_TypeDef* SPIx) {
	/* Init DMA TX mode */
	/* Assuming SPI is already initialized and clock is enabled */
	
	/* Get USART settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Init both streams */
	TM_DMA_Init(Settings->TX_Stream, NULL);
	TM_DMA_Init(Settings->RX_Stream, NULL);
}

void TM_SPI_DMA_InitWithStreamAndChannel(SPI_TypeDef* SPIx, DMA_Stream_TypeDef* TX_Stream, uint32_t TX_Channel, DMA_Stream_TypeDef* RX_Stream, uint32_t RX_Channel) {
	/* Get USART settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Set values */
	Settings->RX_Channel = RX_Channel;
	Settings->RX_Stream = RX_Stream;
	Settings->TX_Channel = TX_Channel;
	Settings->TX_Stream = TX_Stream;
	
	/* Init SPI */
	TM_SPI_DMA_Init(SPIx);
}

void TM_SPI_DMA_Deinit(SPI_TypeDef* SPIx) {
	/* Get USART settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Deinit DMA Streams */
	TM_DMA_DeInit(Settings->TX_Stream);
	TM_DMA_DeInit(Settings->RX_Stream);
}

uint8_t TM_SPI_DMA_Transmit(SPI_TypeDef* SPIx, uint8_t* TX_Buffer, uint8_t* RX_Buffer, uint16_t count) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Get USART settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Check if DMA available */
	if (
		Settings->RX_Stream->NDTR || 
		Settings->TX_Stream->NDTR || 
		(TX_Buffer == NULL && RX_Buffer == NULL)
	) {
		return 0;
	}
	
#if defined(STM32F7xx)
    CLEAR_BIT(SPIx->CR2, SPI_CR2_LDMATX);
    CLEAR_BIT(SPIx->CR2, SPI_CR2_LDMARX);
#endif
	
	/* Set DMA default */
	DMA_InitStruct.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_InitStruct.Init.Mode = DMA_NORMAL;
	DMA_InitStruct.Init.Priority = DMA_PRIORITY_LOW;
	DMA_InitStruct.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DMA_InitStruct.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	DMA_InitStruct.Init.MemBurst = DMA_MBURST_SINGLE;
	DMA_InitStruct.Init.PeriphBurst = DMA_PBURST_SINGLE;

	/* Set dummy memory to default */
	Settings->Dummy16 = 0x12;
	
	/* Set memory size */
	DMA_InitStruct.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA_InitStruct.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;

	/*******************************************************/
	/*                       RX DMA                        */
	/*******************************************************/
	/* Set stream */
	DMA_InitStruct.Instance = Settings->RX_Stream;
	
	/* Configure RX DMA */
	DMA_InitStruct.Init.Channel = Settings->RX_Channel;
	DMA_InitStruct.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_InitStruct.Init.Priority = DMA_PRIORITY_HIGH;
	
	/* Deinit first RX stream */
	TM_DMA_ClearFlag(Settings->RX_Stream, DMA_FLAG_ALL);
	
	/* Set memory increase */
	if (RX_Buffer != NULL) {
		DMA_InitStruct.Init.MemInc = DMA_MINC_ENABLE;
	} else {
		DMA_InitStruct.Init.MemInc = DMA_MINC_DISABLE;
	}
	
	/* Start TX stream */
	TM_DMA_Init(Settings->RX_Stream, &DMA_InitStruct);
	
	/* Start DMA */
	if (RX_Buffer != NULL) {
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) &SPIx->DR, (uint32_t) RX_Buffer, count);
	} else {
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) &SPIx->DR, (uint32_t) &Settings->Dummy32, count);
	}
	
	/*******************************************************/
	/*                       TX DMA                        */
	/*******************************************************/
	/* Set stream */
	DMA_InitStruct.Instance = Settings->TX_Stream;
	
	/* Configure TX DMA */
	DMA_InitStruct.Init.Channel = Settings->TX_Channel;
	DMA_InitStruct.Init.Direction = DMA_MEMORY_TO_PERIPH;
	
	/* Deinit first TX stream */
	TM_DMA_ClearFlag(Settings->TX_Stream, DMA_FLAG_ALL);
	
	/* Set memory increase */
	if (TX_Buffer != NULL) {
		DMA_InitStruct.Init.MemInc = DMA_MINC_ENABLE;
	} else {
		DMA_InitStruct.Init.MemInc = DMA_MINC_DISABLE;
	}
	
	/* Start TX stream */
	TM_DMA_Init(Settings->TX_Stream, &DMA_InitStruct);
	
	/* Start DMA */
	if (TX_Buffer != NULL) {
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) TX_Buffer, (uint32_t) &SPIx->DR, count);
	} else {
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) &Settings->Dummy32, (uint32_t) &SPIx->DR, count);
	}
	
	/* Start stream */
	SPIx->CR2 |= SPI_CR2_RXDMAEN;
	SPIx->CR2 |= SPI_CR2_TXDMAEN;
	
	/* Return OK */
	return 1;
}

uint8_t TM_SPI_DMA_SendByte(SPI_TypeDef* SPIx, uint8_t value, uint16_t count) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Get USART settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Check if DMA available */
	if (Settings->TX_Stream->NDTR) {
		return 0;
	}
	
	/* Set DMA default */
	DMA_InitStruct.Instance = Settings->TX_Stream;
	DMA_InitStruct.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DMA_InitStruct.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA_InitStruct.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_InitStruct.Init.Mode = DMA_NORMAL;
	DMA_InitStruct.Init.Priority = DMA_PRIORITY_LOW;
	DMA_InitStruct.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DMA_InitStruct.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	DMA_InitStruct.Init.MemBurst = DMA_MBURST_SINGLE;
	DMA_InitStruct.Init.PeriphBurst = DMA_PBURST_SINGLE;
	DMA_InitStruct.Init.MemInc = DMA_MINC_DISABLE;
	DMA_InitStruct.Init.Channel = Settings->TX_Channel;
	DMA_InitStruct.Init.Direction = DMA_MEMORY_TO_PERIPH;
	
	/* Set dummy memory to value we specify */
	Settings->Dummy32 = value;
	
	/* Deinit first TX stream */
	TM_DMA_ClearFlag(Settings->TX_Stream, DMA_FLAG_ALL);
	
	/* Init TX stream */
	TM_DMA_Init(Settings->TX_Stream, &DMA_InitStruct);
	
	/* Start TX stream */
	TM_DMA_Start(&DMA_InitStruct, (uint32_t) &Settings->Dummy32, (uint32_t) &SPIx->DR, count);
	
	/* Enable SPI TX DMA */
	SPIx->CR2 |= SPI_CR2_TXDMAEN;
	
	/* Return OK */
	return 1;
}

uint8_t TM_SPI_DMA_SendHalfWord(SPI_TypeDef* SPIx, uint16_t value, uint16_t count) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Get USART settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Check if DMA available */
	if (Settings->TX_Stream->NDTR) {
		return 0;
	}
	
	/* Set DMA default */
	DMA_InitStruct.Instance = Settings->TX_Stream;
	DMA_InitStruct.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DMA_InitStruct.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DMA_InitStruct.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_InitStruct.Init.Mode = DMA_NORMAL;
	DMA_InitStruct.Init.Priority = DMA_PRIORITY_LOW;
	DMA_InitStruct.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DMA_InitStruct.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	DMA_InitStruct.Init.MemBurst = DMA_MBURST_SINGLE;
	DMA_InitStruct.Init.PeriphBurst = DMA_PBURST_SINGLE;
	DMA_InitStruct.Init.MemInc = DMA_MINC_DISABLE;
	DMA_InitStruct.Init.Channel = Settings->TX_Channel;
	DMA_InitStruct.Init.Direction = DMA_MEMORY_TO_PERIPH;
	
	/* Set dummy memory to value we specify */
	Settings->Dummy16 = value;
	
	/* Deinit first TX stream */
	TM_DMA_ClearFlag(Settings->TX_Stream, DMA_FLAG_ALL);
	
	/* Init TX stream */
	TM_DMA_Init(Settings->TX_Stream, &DMA_InitStruct);
	
	/* Start TX stream */
	TM_DMA_Start(&DMA_InitStruct, (uint32_t) &Settings->Dummy16, (uint32_t) &SPIx->DR, count);
	
	/* Enable SPI TX DMA */
	SPIx->CR2 |= SPI_CR2_TXDMAEN;
	
	/* Return OK */
	return 1;
}

uint8_t TM_SPI_DMA_Transmitting(SPI_TypeDef* SPIx) {
	/* Get SPI settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Check if TX or RX DMA are working */
	return (
		Settings->RX_Stream->NDTR || /*!< RX is working */
		Settings->TX_Stream->NDTR || /*!< TX is working */
		SPI_IS_BUSY(SPIx)            /*!< SPI is busy */
	);
}

DMA_Stream_TypeDef* TM_SPI_DMA_GetStreamTX(SPI_TypeDef* SPIx) {
	/* Return pointer to TX stream */
	return TM_SPI_DMA_INT_GetSettings(SPIx)->TX_Stream;
}

DMA_Stream_TypeDef* TM_SPI_DMA_GetStreamRX(SPI_TypeDef* SPIx) {
	/* Return pointer to TX stream */
	return TM_SPI_DMA_INT_GetSettings(SPIx)->RX_Stream;
}

void TM_SPI_DMA_EnableInterrupts(SPI_TypeDef* SPIx) {
	/* Get SPI settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Enable interrupts for TX and RX streams */
	TM_DMA_EnableInterrupts(Settings->TX_Stream);
	TM_DMA_EnableInterrupts(Settings->RX_Stream);
}

void TM_SPI_DMA_DisableInterrupts(SPI_TypeDef* SPIx) {
	/* Get SPI settings */
	TM_SPI_DMA_INT_t* Settings = TM_SPI_DMA_INT_GetSettings(SPIx);
	
	/* Enable interrupts for TX and RX streams */
	TM_DMA_DisableInterrupts(Settings->TX_Stream);
	TM_DMA_DisableInterrupts(Settings->RX_Stream);
}

/* Private functions */
static TM_SPI_DMA_INT_t* TM_SPI_DMA_INT_GetSettings(SPI_TypeDef* SPIx) {
	TM_SPI_DMA_INT_t* result;
#ifdef SPI1
	if (SPIx == SPI1) {
		result = &SPI1_DMA_INT;
	}
#endif
#ifdef SPI2
	if (SPIx == SPI2) {
		result = &SPI2_DMA_INT;
	}
#endif
#ifdef SPI3
	if (SPIx == SPI3) {
		result = &SPI3_DMA_INT;
	}
#endif
#ifdef SPI4
	if (SPIx == SPI4) {
		result = &SPI4_DMA_INT;
	}
#endif
#ifdef SPI5
	if (SPIx == SPI5) {
		result = &SPI5_DMA_INT;
	}
#endif
#ifdef SPI6
	if (SPIx == SPI6) {
		result = &SPI6_DMA_INT;
	}
#endif

	/* Return */
	return result;
}
