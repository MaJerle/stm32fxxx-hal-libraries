/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen MAJERLE
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_spi.h"

/* Defines for alternate functions */
#if defined(STM32F4xx) || defined(STM32F7xx)
#define GPIO_AFx_SPI1    GPIO_AF5_SPI1
#define GPIO_AFx_SPI2    0x05
#define GPIO_AFx_SPI3    0x06
#define GPIO_AFx_SPI4    GPIO_AF5_SPI4
#define GPIO_AFx_SPI5    0x05
#define GPIO_AFx_SPI6    GPIO_AF5_SPI6
#endif
#if defined(STM32F0xx)
#define GPIO_AFx_SPI1    GPIO_AF0_SPI1
#define GPIO_AFx_SPI2    GPIO_AF0_SPI2
#endif

/* Private functions */
static void TM_SPIx_Init(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack, TM_SPI_Mode_t SPI_Mode, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_MasterSlave, uint16_t SPI_FirstBit);
void TM_SPI1_INT_InitPins(TM_SPI_PinsPack_t pinspack);
void TM_SPI2_INT_InitPins(TM_SPI_PinsPack_t pinspack);
void TM_SPI3_INT_InitPins(TM_SPI_PinsPack_t pinspack);
void TM_SPI4_INT_InitPins(TM_SPI_PinsPack_t pinspack);
void TM_SPI5_INT_InitPins(TM_SPI_PinsPack_t pinspack);
void TM_SPI6_INT_InitPins(TM_SPI_PinsPack_t pinspack);

void TM_SPI_Init(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack) {
	/* Init with default settings */
#ifdef SPI1
	if (SPIx == SPI1) {
		TM_SPIx_Init(SPI1, pinspack, TM_SPI1_MODE, TM_SPI1_PRESCALER, TM_SPI1_MASTERSLAVE, TM_SPI1_FIRSTBIT);
	}
#endif
#ifdef SPI2
	if (SPIx == SPI2) {
		TM_SPIx_Init(SPI2, pinspack, TM_SPI2_MODE, TM_SPI2_PRESCALER, TM_SPI2_MASTERSLAVE, TM_SPI2_FIRSTBIT);
	}
#endif
#ifdef SPI3
	if (SPIx == SPI3) {
		TM_SPIx_Init(SPI3, pinspack, TM_SPI3_MODE, TM_SPI3_PRESCALER, TM_SPI3_MASTERSLAVE, TM_SPI3_FIRSTBIT);
	}
#endif
#ifdef SPI4
	if (SPIx == SPI4) {
		TM_SPIx_Init(SPI4, pinspack, TM_SPI4_MODE, TM_SPI4_PRESCALER, TM_SPI4_MASTERSLAVE, TM_SPI4_FIRSTBIT);
	}
#endif
#ifdef SPI5
	if (SPIx == SPI5) {
		TM_SPIx_Init(SPI5, pinspack, TM_SPI5_MODE, TM_SPI5_PRESCALER, TM_SPI5_MASTERSLAVE, TM_SPI5_FIRSTBIT);
	}
#endif
#ifdef SPI6
	if (SPIx == SPI6) {
		TM_SPIx_Init(SPI6, pinspack, TM_SPI6_MODE, TM_SPI6_PRESCALER, TM_SPI6_MASTERSLAVE, TM_SPI6_FIRSTBIT);
	}
#endif
}

void TM_SPI_InitWithMode(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack, TM_SPI_Mode_t SPI_Mode) {
	/* Init with custom mode, 0, 1, 2, 3 */
#ifdef SPI1
	if (SPIx == SPI1) {
		TM_SPIx_Init(SPI1, pinspack, SPI_Mode, TM_SPI1_PRESCALER, TM_SPI1_MASTERSLAVE, TM_SPI1_FIRSTBIT);
	}
#endif
#ifdef SPI2
	if (SPIx == SPI2) {
		TM_SPIx_Init(SPI2, pinspack, SPI_Mode, TM_SPI2_PRESCALER, TM_SPI2_MASTERSLAVE, TM_SPI2_FIRSTBIT);
	}
#endif
#ifdef SPI3
	if (SPIx == SPI3) {
		TM_SPIx_Init(SPI3, pinspack, SPI_Mode, TM_SPI3_PRESCALER, TM_SPI3_MASTERSLAVE, TM_SPI3_FIRSTBIT);
	}
#endif
#ifdef SPI4
	if (SPIx == SPI4) {
		TM_SPIx_Init(SPI4, pinspack, SPI_Mode, TM_SPI4_PRESCALER, TM_SPI4_MASTERSLAVE, TM_SPI4_FIRSTBIT);
	}
#endif
#ifdef SPI5
	if (SPIx == SPI5) {
		TM_SPIx_Init(SPI5, pinspack, SPI_Mode, TM_SPI5_PRESCALER, TM_SPI5_MASTERSLAVE, TM_SPI5_FIRSTBIT);
	}
#endif
#ifdef SPI6
	if (SPIx == SPI6) {
		TM_SPIx_Init(SPI6, pinspack, SPI_Mode, TM_SPI6_PRESCALER, TM_SPI6_MASTERSLAVE, TM_SPI6_FIRSTBIT);
	}
#endif
}

void TM_SPI_InitFull(
	SPI_TypeDef* SPIx,              \
	TM_SPI_PinsPack_t pinspack,     \
	uint16_t SPI_BaudRatePrescaler, \
	TM_SPI_Mode_t SPI_Mode_t,       \
	uint16_t SPI_Mode,              \
	uint16_t SPI_FirstBit           \
) {
	/* Init FULL SPI settings by user */
#ifdef SPI1
	if (SPIx == SPI1) {
		TM_SPIx_Init(SPI1, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
	}
#endif
#ifdef SPI2
	if (SPIx == SPI2) {
		TM_SPIx_Init(SPI2, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
	}
#endif
#ifdef SPI3
	if (SPIx == SPI3) {
		TM_SPIx_Init(SPI3, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
	}
#endif
#ifdef SPI4
	if (SPIx == SPI4) {
		TM_SPIx_Init(SPI4, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
	}
#endif
#ifdef SPI5
	if (SPIx == SPI5) {
		TM_SPIx_Init(SPI5, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
	}
#endif
#ifdef SPI6
	if (SPIx == SPI6) {
		TM_SPIx_Init(SPI6, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
	}
#endif
}

uint16_t TM_SPI_GetPrescalerFromMaxFrequency(SPI_TypeDef* SPIx, uint32_t MAX_SPI_Frequency) {
	uint32_t APB_Frequency;
	uint8_t i;
	
	/* Prevent false input */
	if (MAX_SPI_Frequency == 0) {
		return SPI_BAUDRATEPRESCALER_256;
	}
	
	/* Calculate max SPI clock */
	if (0
#ifdef SPI1	
		|| SPIx == SPI1
#endif
#ifdef SPI4
		|| SPIx == SPI4
#endif
#ifdef SPI5
		|| SPIx == SPI5
#endif
#ifdef SPI6
		|| SPIx == SPI6
#endif
	) {
		APB_Frequency = HAL_RCC_GetPCLK2Freq();
	} else {
		APB_Frequency = HAL_RCC_GetPCLK1Freq();
	}
	
	/* Calculate prescaler value */
	/* Bits 5:3 in CR1 SPI registers are prescalers */
	/* 000 = 2, 001 = 4, 002 = 8, ..., 111 = 256 */
	for (i = 0; i < 8; i++) {
		if (APB_Frequency / (1 << (i + 1)) <= MAX_SPI_Frequency) {
			/* Bits for BP are 5:3 in CR1 register */
			return (i << 3);
		}
	}
	
	/* Use max prescaler possible */
	return SPI_BAUDRATEPRESCALER_256;
}

TM_SPI_DataSize_t TM_SPI_SetDataSize(SPI_TypeDef* SPIx, TM_SPI_DataSize_t DataSize) {
	TM_SPI_DataSize_t status;
	
	/* Disable SPI first */
	SPIx->CR1 &= ~SPI_CR1_SPE;
	
#if defined(STM32F7xx)
	/* Read current SPI status */
	status = ((SPIx->CR2 & SPI_CR2_DS) == SPI_CR2_DS) ? TM_SPI_DataSize_16b : TM_SPI_DataSize_8b;
	
	/* Set proper value */
	if (DataSize == TM_SPI_DataSize_16b) {
		/* Set bit for frame */
		SPIx->CR2 |= SPI_CR2_DS;
	} else {
		/* Clear bit for frame */
		SPIx->CR2 = (SPIx->CR2 & ~SPI_CR2_DS) | (SPI_CR2_DS_3);
	}
#else
	/* Read current SPI status */
	status = (SPIx->CR1 & SPI_CR1_DFF) ? TM_SPI_DataSize_16b : TM_SPI_DataSize_8b;

	/* Set proper value */
	if (DataSize == TM_SPI_DataSize_16b) {
		/* Set bit for frame */
		SPIx->CR1 |= SPI_CR1_DFF;
	} else {
		/* Clear bit for frame */
		SPIx->CR1 &= ~SPI_CR1_DFF;
	}
#endif
	
	/* Enable SPI back */
	SPIx->CR1 |= SPI_CR1_SPE;
	
	/* Return status */
	return status;	
}

void TM_SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint8_t* dataIn, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint8_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		*dataIn++ = *(__IO uint8_t *)&SPIx->DR;
	}
}

void TM_SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint8_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		(void)*(__IO uint16_t *)&SPIx->DR;
	}
}

void TM_SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t* dataIn, uint8_t dummy, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint8_t *)&SPIx->DR = dummy;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Save data to buffer */
		*dataIn++ = *(__IO uint8_t *)&SPIx->DR;
	}
}

void TM_SPI_SendMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint16_t* dataIn, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint16_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		*dataIn++ = *(__IO uint16_t *)&SPIx->DR;
	}
}

void TM_SPI_WriteMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint16_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		(void)*(__IO uint16_t *)&SPIx->DR;
	}
}

void TM_SPI_ReadMulti16(SPI_TypeDef* SPIx, uint16_t* dataIn, uint16_t dummy, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	/* Wait for previous transmissions to complete if DMA TX enabled for SPI */
	SPI_WAIT_TX(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		SPIx->DR = dummy;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Save data to buffer */
		*dataIn++ = SPIx->DR;
	}
}

__weak void TM_SPI_InitCustomPinsCallback(SPI_TypeDef* SPIx, uint16_t AlternateFunction) { 
	/* NOTE: This function Should not be modified, when the callback is needed,
           the TM_SPI_InitCustomPinsCallback could be implemented in the user file
   */
}

/* Private functions */
static void TM_SPIx_Init(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack, TM_SPI_Mode_t SPI_Mode, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_MasterSlave, uint16_t SPI_FirstBit) {
	SPI_HandleTypeDef SPIHandle;
	
	/* Save instance */
	SPIHandle.Instance = SPIx;
	
#ifdef SPI1	
	if (SPIx == SPI1) {
		/* Enable SPI clock */
		__HAL_RCC_SPI1_CLK_ENABLE();
		
		/* Init pins */
		TM_SPI1_INT_InitPins(pinspack);
		
		/* Set options */
		SPIHandle.Init.DataSize = TM_SPI1_DATASIZE;
	}
#endif
#ifdef SPI2
	if (SPIx == SPI2) {
		/* Enable SPI clock */
		__HAL_RCC_SPI2_CLK_ENABLE();
		
		/* Init pins */
		TM_SPI2_INT_InitPins(pinspack);
		
		/* Set options */
		SPIHandle.Init.DataSize = TM_SPI2_DATASIZE;
	}
#endif
#ifdef SPI3
	if (SPIx == SPI3) {
		/* Enable SPI clock */
		__HAL_RCC_SPI3_CLK_ENABLE();
		
		/* Init pins */
		TM_SPI3_INT_InitPins(pinspack);
		
		/* Set options */
		SPIHandle.Init.DataSize = TM_SPI3_DATASIZE;
	
	}
#endif
#ifdef SPI4
	if (SPIx == SPI4) {
		/* Enable SPI clock */
		__HAL_RCC_SPI4_CLK_ENABLE();
		
		/* Init pins */
		TM_SPI4_INT_InitPins(pinspack);
		
		/* Set options */
		SPIHandle.Init.DataSize = TM_SPI4_DATASIZE;
	}
#endif
#ifdef SPI5
	if (SPIx == SPI5) {
		/* Enable SPI clock */
		__HAL_RCC_SPI5_CLK_ENABLE();
		
		/* Init pins */
		TM_SPI5_INT_InitPins(pinspack);
		
		/* Set options */
		SPIHandle.Init.DataSize = TM_SPI5_DATASIZE;
	}
#endif
#ifdef SPI6
	if (SPIx == SPI6) {
		/* Enable SPI clock */
		__HAL_RCC_SPI6_CLK_ENABLE();
		
		/* Init pins */
		TM_SPI6_INT_InitPins(pinspack);
		
		/* Set options */
		SPIHandle.Init.DataSize = TM_SPI6_DATASIZE;
	}
#endif

	/* Fill SPI settings */
	SPIHandle.Init.BaudRatePrescaler = SPI_BaudRatePrescaler;
	SPIHandle.Init.FirstBit = SPI_FirstBit;
	SPIHandle.Init.Mode = SPI_MasterSlave;
	
	SPIHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPIHandle.Init.CRCPolynomial = 7;
	SPIHandle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPIHandle.Init.NSS = SPI_NSS_SOFT;
	SPIHandle.Init.Direction = SPI_DIRECTION_2LINES;
	
    
#if defined(STM32F7xx)
	SPIHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	SPIHandle.Init.CRCLength = SPI_CRC_LENGTH_8BIT;
    SPIHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPIHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#endif
	
	/* SPI mode */
	if (SPI_Mode == TM_SPI_Mode_0) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
		SPIHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
	} else if (SPI_Mode == TM_SPI_Mode_1) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
		SPIHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
	} else if (SPI_Mode == TM_SPI_Mode_2) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
		SPIHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
	} else if (SPI_Mode == TM_SPI_Mode_3) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
		SPIHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
	}
	
	/* Disable first */
	__HAL_SPI_DISABLE(&SPIHandle);
	
	/* Init SPI */
	HAL_SPI_Init(&SPIHandle);
	
	/* Enable SPI */
	__HAL_SPI_ENABLE(&SPIHandle);
}

/* Private functions */
#ifdef SPI1
void TM_SPI1_INT_InitPins(TM_SPI_PinsPack_t pinspack) {
	/* Init SPI pins */
#if defined(GPIOA)
	if (pinspack == TM_SPI_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI1);
	}
#endif
#if defined(GPIOB)
	if (pinspack == TM_SPI_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI1);
	}
#endif
	if (pinspack == TM_SPI_PinsPack_Custom) {
		/* Call user function */
		TM_SPI_InitCustomPinsCallback(SPI1, GPIO_AFx_SPI1);
	}
}
#endif

#ifdef SPI2
void TM_SPI2_INT_InitPins(TM_SPI_PinsPack_t pinspack) {
	/* Init SPI pins */
#if defined(GPIOB) && defined(GPIOC)
	if (pinspack == TM_SPI_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_10, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI2);
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_2 | GPIO_PIN_3, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
#if defined(GPIOB)
	if (pinspack == TM_SPI_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
#if defined(GPIOI)
	if (pinspack == TM_SPI_PinsPack_3) {
		TM_GPIO_InitAlternate(GPIOI, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
#if defined(GPIOB) && defined(GPIOI)
	if (pinspack == TM_SPI_PinsPack_4) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI2);
		TM_GPIO_InitAlternate(GPIOI, GPIO_PIN_1, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
	if (pinspack == TM_SPI_PinsPack_Custom) {
		/* Call user function */
		TM_SPI_InitCustomPinsCallback(SPI2, GPIO_AFx_SPI2);
	}
}
#endif

#ifdef SPI3
void TM_SPI3_INT_InitPins(TM_SPI_PinsPack_t pinspack) {
	/* Enable SPI pins */
#if defined(GPIOB)
	if (pinspack == TM_SPI_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI3);
	}
#endif
#if defined(GPIOC)
	if (pinspack == TM_SPI_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI3);
	}
#endif
	if (pinspack == TM_SPI_PinsPack_Custom) {
		/* Call user function */
		TM_SPI_InitCustomPinsCallback(SPI3, GPIO_AFx_SPI3);
	}
}
#endif

#ifdef SPI4
void TM_SPI4_INT_InitPins(TM_SPI_PinsPack_t pinspack) {
	/* Init SPI pins */
#if defined(GPIOE)
	if (pinspack == TM_SPI_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI4);
	}
#endif
#if defined(GPIOE)
	if (pinspack == TM_SPI_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI4);
	}
#endif
	if (pinspack == TM_SPI_PinsPack_Custom) {
		/* Call user function */
		TM_SPI_InitCustomPinsCallback(SPI4, GPIO_AFx_SPI4);
	}
}
#endif

#ifdef SPI5
void TM_SPI5_INT_InitPins(TM_SPI_PinsPack_t pinspack) {
	/* Init SPI pins */
#if defined(GPIOF)
	if (pinspack == TM_SPI_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI5);
	}
#endif
#if defined(GPIOF) && defined(GPIOH)
	if (pinspack == TM_SPI_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_11, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI5);
		TM_GPIO_InitAlternate(GPIOH, GPIO_PIN_6 | GPIO_PIN_7, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI5);
	}
#endif
	if (pinspack == TM_SPI_PinsPack_Custom) {
		/* Call user function */
		TM_SPI_InitCustomPinsCallback(SPI5, GPIO_AFx_SPI5);
	}
}
#endif

#ifdef SPI6
void TM_SPI6_INT_InitPins(TM_SPI_PinsPack_t pinspack) {
#if defined(GPIOG)
	if (pinspack == TM_SPI_PinsPack_1) {
		/* Init SPI pins */
		TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AFx_SPI6);
	}
#endif
	if (pinspack == TM_SPI_PinsPack_Custom) {
		/* Call user function */
		TM_SPI_InitCustomPinsCallback(SPI6, GPIO_AFx_SPI6);
	}
}
#endif

