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
#include "tm_stm32_i2c_dma.h"

/* Private structure */
typedef struct {
	uint32_t TX_Channel;
	DMA_Stream_TypeDef* TX_Stream;
	uint32_t RX_Channel;
	DMA_Stream_TypeDef* RX_Stream;
	uint32_t Dummy32;
	uint16_t Dummy16;
	I2C_HandleTypeDef Handle;
} TM_I2C_DMA_INT_t;

/* Private variables */
#ifdef I2C1
static TM_I2C_DMA_INT_t I2C1_DMA_INT = {I2C1_DMA_TX_CHANNEL, I2C1_DMA_TX_STREAM, I2C1_DMA_RX_CHANNEL, I2C1_DMA_RX_STREAM};
#endif
#ifdef I2C2
static TM_I2C_DMA_INT_t I2C2_DMA_INT = {I2C2_DMA_TX_CHANNEL, I2C2_DMA_TX_STREAM, I2C2_DMA_RX_CHANNEL, I2C2_DMA_RX_STREAM};
#endif
#ifdef I2C3
static TM_I2C_DMA_INT_t I2C3_DMA_INT = {I2C3_DMA_TX_CHANNEL, I2C3_DMA_TX_STREAM, I2C3_DMA_RX_CHANNEL, I2C3_DMA_RX_STREAM};
#endif

/* Private functions */
static TM_I2C_DMA_INT_t* TM_I2C_DMA_INT_GetSettings(I2C_TypeDef* I2Cx);
	
void TM_I2C_DMA_Init(I2C_TypeDef* I2Cx) {
	/* Init DMA TX mode */
	/* Assuming I2C is already initialized and clock is enabled */
	
	/* Get USART settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Init both streams */
	TM_DMA_Init(Settings->TX_Stream, NULL);
	TM_DMA_Init(Settings->RX_Stream, NULL);
}

void TM_I2C_DMA_InitWithStreamAndChannel(I2C_TypeDef* I2Cx, DMA_Stream_TypeDef* TX_Stream, uint32_t TX_Channel, DMA_Stream_TypeDef* RX_Stream, uint32_t RX_Channel) {
	/* Get USART settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Set values */
	Settings->RX_Channel = RX_Channel;
	Settings->RX_Stream = RX_Stream;
	Settings->TX_Channel = TX_Channel;
	Settings->TX_Stream = TX_Stream;
	
	/* Init I2C */
	TM_I2C_DMA_Init(I2Cx);
}

void TM_I2C_DMA_Deinit(I2C_TypeDef* I2Cx) {
	/* Get USART settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Deinit DMA Streams */
	TM_DMA_DeInit(Settings->TX_Stream);
	TM_DMA_DeInit(Settings->RX_Stream);
}

uint8_t TM_I2C_DMA_Transmit(I2C_TypeDef* I2Cx, uint8_t* TX_Buffer, uint8_t* RX_Buffer, uint16_t count) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Get USART settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Check if DMA available */
	if (
		Settings->RX_Stream->NDTR || 
		Settings->TX_Stream->NDTR || 
		(TX_Buffer == NULL && RX_Buffer == NULL)
	) {
		return 0;
	}
	
#if defined(STM32F7xx)
    CLEAR_BIT(I2Cx->CR2, I2C_CR2_LDMATX);
    CLEAR_BIT(I2Cx->CR2, I2C_CR2_LDMARX);
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
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) &I2Cx->DR, (uint32_t) RX_Buffer, count);
	} else {
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) &I2Cx->DR, (uint32_t) &Settings->Dummy32, count);
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
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) TX_Buffer, (uint32_t) &I2Cx->DR, count);
	} else {
		TM_DMA_Start(&DMA_InitStruct, (uint32_t) &Settings->Dummy32, (uint32_t) &I2Cx->DR, count);
	}
	
	/* Start stream */
	I2Cx->CR2 |= I2C_CR2_DMAEN;
	
	/* Return OK */
	return 1;
}

uint8_t TM_I2C_DMA_SendByte(I2C_TypeDef* I2Cx, uint8_t value, uint16_t count) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Get USART settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
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
	TM_DMA_Start(&DMA_InitStruct, (uint32_t) &Settings->Dummy32, (uint32_t) &I2Cx->DR, count);
	
	/* Enable I2C TX DMA */
	I2Cx->CR2 |= I2C_CR2_TXDMAEN;
	
	/* Return OK */
	return 1;
}

uint8_t TM_I2C_DMA_SendHalfWord(I2C_TypeDef* I2Cx, uint16_t value, uint16_t count) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Get USART settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
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
	TM_DMA_Start(&DMA_InitStruct, (uint32_t) &Settings->Dummy16, (uint32_t) &I2Cx->DR, count);
	
	/* Enable I2C TX DMA */
	I2Cx->CR2 |= I2C_CR2_TXDMAEN;
	
	/* Return OK */
	return 1;
}

uint8_t TM_I2C_DMA_Transmitting(I2C_TypeDef* I2Cx) {
	/* Get I2C settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Check if TX or RX DMA are working */
	return (
		Settings->RX_Stream->NDTR || /*!< RX is working */
		Settings->TX_Stream->NDTR || /*!< TX is working */
		I2C_IS_BUSY(I2Cx)            /*!< I2C is busy */
	);
}

DMA_Stream_TypeDef* TM_I2C_DMA_GetStreamTX(I2C_TypeDef* I2Cx) {
	/* Return pointer to TX stream */
	return TM_I2C_DMA_INT_GetSettings(I2Cx)->TX_Stream;
}

DMA_Stream_TypeDef* TM_I2C_DMA_GetStreamRX(I2C_TypeDef* I2Cx) {
	/* Return pointer to TX stream */
	return TM_I2C_DMA_INT_GetSettings(I2Cx)->RX_Stream;
}

void TM_I2C_DMA_EnableInterrupts(I2C_TypeDef* I2Cx) {
	/* Get I2C settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Enable interrupts for TX and RX streams */
	TM_DMA_EnableInterrupts(Settings->TX_Stream);
	TM_DMA_EnableInterrupts(Settings->RX_Stream);
}

void TM_I2C_DMA_DisableInterrupts(I2C_TypeDef* I2Cx) {
	/* Get I2C settings */
	TM_I2C_DMA_INT_t* Settings = TM_I2C_DMA_INT_GetSettings(I2Cx);
	
	/* Enable interrupts for TX and RX streams */
	TM_DMA_DisableInterrupts(Settings->TX_Stream);
	TM_DMA_DisableInterrupts(Settings->RX_Stream);
}

/* Private functions */
static TM_I2C_DMA_INT_t* TM_I2C_DMA_INT_GetSettings(I2C_TypeDef* I2Cx) {
	TM_I2C_DMA_INT_t* result;
#ifdef I2C1
	if (I2Cx == I2C1) {
		result = &I2C1_DMA_INT;
	}
#endif
#ifdef I2C2
	if (I2Cx == I2C2) {
		result = &I2C2_DMA_INT;
	}
#endif
#ifdef I2C3
	if (I2Cx == I2C3) {
		result = &I2C3_DMA_INT;
	}
#endif
#ifdef I2C4
	if (I2Cx == I2C4) {
		result = &I2C4_DMA_INT;
	}
#endif
#ifdef I2C5
	if (I2Cx == I2C5) {
		result = &I2C5_DMA_INT;
	}
#endif
#ifdef I2C6
	if (I2Cx == I2C6) {
		result = &I2C6_DMA_INT;
	}
#endif

	/* Return */
	return result;
}
