/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
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
#include "tm_stm32_dma.h"

/* Private defines for stream numbers */
#define GET_STREAM_NUMBER_DMA1(stream)    (((uint32_t)(stream) - (uint32_t)DMA1_Stream0) / (0x18))
#define GET_STREAM_NUMBER_DMA2(stream)    (((uint32_t)(stream) - (uint32_t)DMA2_Stream0) / (0x18))

/* Offsets for bits */
const static uint8_t DMA_Flags_Bit_Pos[4] = {
	0, 6, 16, 22
};
const static IRQn_Type DMA_IRQs[2][8] = {
	{
		DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn, DMA1_Stream3_IRQn,
		DMA1_Stream4_IRQn, DMA1_Stream5_IRQn, DMA1_Stream6_IRQn, DMA1_Stream7_IRQn
	},
	{
		DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn, DMA2_Stream3_IRQn,
		DMA2_Stream4_IRQn, DMA2_Stream5_IRQn, DMA2_Stream6_IRQn, DMA2_Stream7_IRQn
	}
};

void TM_DMA_ClearFlags(DMA_Stream_TypeDef* DMA_Stream) {
	/* Clear all flags */
	TM_DMA_ClearFlag(DMA_Stream, DMA_FLAG_ALL);
}

void TM_DMA_ClearFlag(DMA_Stream_TypeDef* DMA_Stream, uint32_t flag) {
	uint32_t location;
	uint32_t stream_number;

	/* Check stream value */
	if (DMA_Stream < DMA2_Stream0) {
		location = (uint32_t)&DMA1->LIFCR;
		stream_number = GET_STREAM_NUMBER_DMA1(DMA_Stream);
	} else {
		location = (uint32_t)&DMA2->LIFCR;
		stream_number = GET_STREAM_NUMBER_DMA2(DMA_Stream);
	}
	
	/* Get register offset */
	if (stream_number >= 4) {
		/* High registers for DMA clear */
		location += 4;
		
		/* Do offset for high DMA registers */
		stream_number -= 4;
	}
	
	/* Clear flags */
	*(__IO uint32_t *)location = (flag & DMA_FLAG_ALL) << DMA_Flags_Bit_Pos[stream_number];
}

uint32_t TM_DMA_GetFlags(DMA_Stream_TypeDef* DMA_Stream, uint32_t flag) {
	uint32_t stream_number = 0;
	uint32_t location = 0;
	uint32_t flags = 0;
	
	/* Check stream value */
	if (DMA_Stream < DMA2_Stream0) {
		location = (uint32_t)&DMA1->LISR;
		stream_number = GET_STREAM_NUMBER_DMA1(DMA_Stream);
	} else {
		location = (uint32_t)&DMA2->LISR;
		stream_number = GET_STREAM_NUMBER_DMA2(DMA_Stream);
	}
	
	/* Get register offset */
	if (stream_number >= 4) {
		/* High registers for DMA clear */
		location += 4;
		
		/* Do offset for high DMA registers */
		stream_number -= 4;
	}
	
	/* Get register value */
	flags =   *(__IO uint32_t *)location;
	flags >>= DMA_Flags_Bit_Pos[stream_number];
	flags &=  DMA_FLAG_ALL;
	
	/* Return value */
	return flags;
}

void TM_DMA_EnableInterrupts(DMA_Stream_TypeDef* DMA_Stream) {
	uint32_t stream_number;
	IRQn_Type type;
	uint32_t preemption;
	
	/* Clear flags first */
	TM_DMA_ClearFlag(DMA_Stream, DMA_FLAG_ALL);

	/* Check stream value */
	if (DMA_Stream < DMA2_Stream0) {
		stream_number = GET_STREAM_NUMBER_DMA1(DMA_Stream);
		preemption = DMA1_NVIC_PREEMPTION_PRIORITY;
		type = DMA_IRQs[0][stream_number];
	} else {
		stream_number = GET_STREAM_NUMBER_DMA2(DMA_Stream);
		preemption = DMA2_NVIC_PREEMPTION_PRIORITY;
		type = DMA_IRQs[1][stream_number];
	}
	
	/* Disable IRQ */
	HAL_NVIC_DisableIRQ(type);
	
	/* Set priorities */
	HAL_NVIC_SetPriority(type, preemption, stream_number);
	
	/* Enable IRQ */
	HAL_NVIC_EnableIRQ(type);
	
	/* Enable DMA stream interrupts */
	DMA_Stream->CR |= DMA_SxCR_TCIE | DMA_SxCR_HTIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE;
	DMA_Stream->FCR |= DMA_SxFCR_FEIE;
}

void TM_DMA_DisableInterrupts(DMA_Stream_TypeDef* DMA_Stream) {
	IRQn_Type IRQValue;
	
	/* Clear flags first */
	TM_DMA_ClearFlag(DMA_Stream, DMA_FLAG_ALL);

	/* Check stream value */
	if (DMA_Stream < DMA2_Stream0) {
		IRQValue = DMA_IRQs[0][GET_STREAM_NUMBER_DMA1(DMA_Stream)];
	} else {
		IRQValue = DMA_IRQs[0][GET_STREAM_NUMBER_DMA2(DMA_Stream)];
	}
	
	/* Disable NVIC */
	HAL_NVIC_DisableIRQ(IRQValue);
	
	/* Disable DMA stream interrupts */
	DMA_Stream->CR &= ~(DMA_SxCR_TCIE  | DMA_SxCR_HTIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE);
	DMA_Stream->FCR &= DMA_SxFCR_FEIE;
}

void TM_DMA_Init(DMA_Stream_TypeDef* Stream, DMA_HandleTypeDef* HDMA) {	
	/* Init DMA stream */
	if (HDMA) {
		/* Unlock DMA */
		__HAL_UNLOCK(HDMA);
		
		/* Init DMA */
		HDMA->Instance = Stream;
		HAL_DMA_Init(HDMA);
	} else {
		/* Enable DMA clock */
		if (Stream >= DMA2_Stream0) {
			/* Enable DMA2 clock */	
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
		} else {
			/* Enable DMA1 clock */
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		}
	}
}

void TM_DMA_DeInit(DMA_Stream_TypeDef* Stream) {
	DMA_HandleTypeDef DMA_InitStruct;
	
	/* Fill data and deinit DMA stream */
	DMA_InitStruct.Instance = Stream;
	HAL_DMA_DeInit(&DMA_InitStruct);
}

void TM_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t Source, uint32_t Destination, uint16_t Length) {
	/* Unlock DMA */
	__HAL_UNLOCK(hdma);
	
	/* Start DMA transfer */
	HAL_DMA_Start(hdma, Source, Destination, Length);
}

/*****************************************************************/
/*                 DMA INTERRUPT USER CALLBACKS                  */
/*****************************************************************/
__weak void TM_DMA_TransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_DMA_TransferCompleteHandler should be implemented in the user file
	*/
}

__weak void TM_DMA_HalfTransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_DMA_HalfTransferCompleteHandler should be implemented in the user file
	*/
}

__weak void TM_DMA_TransferErrorHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_DMA_TransferErrorHandler should be implemented in the user file
	*/
}

__weak void TM_DMA_DirectModeErrorHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_DMA_DirectModeErrorHandler should be implemented in the user file
	*/
}

__weak void TM_DMA_FIFOErrorHandler(DMA_Stream_TypeDef* DMA_Stream) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_DMA_FIFOErrorHandler should be implemented in the user file
	*/
}

/*****************************************************************/
/*                    DMA INTERNAL FUNCTIONS                     */
/*****************************************************************/
static void TM_DMA_INT_ProcessInterrupt(DMA_Stream_TypeDef* DMA_Stream) {
	/* Get DMA interrupt status flags */
	uint16_t flags = TM_DMA_GetFlags(DMA_Stream, DMA_FLAG_ALL);
	
	/* Clear flags */
	TM_DMA_ClearFlag(DMA_Stream, DMA_FLAG_ALL);
	
	/* Call user callback function */
	
	/* Check transfer complete flag */
	if ((flags & DMA_FLAG_TCIF) && (DMA_Stream->CR & DMA_SxCR_TCIE)) {
		TM_DMA_TransferCompleteHandler(DMA_Stream);
	}
	/* Check half-transfer complete flag */
	if ((flags & DMA_FLAG_HTIF) && (DMA_Stream->CR & DMA_SxCR_HTIE)) {
		TM_DMA_HalfTransferCompleteHandler(DMA_Stream);
	}
	/* Check transfer error flag */
	if ((flags & DMA_FLAG_TEIF) && (DMA_Stream->CR & DMA_SxCR_TEIE)) {
		TM_DMA_TransferErrorHandler(DMA_Stream);
	}
	/* Check direct error flag */
	if ((flags & DMA_FLAG_DMEIF) && (DMA_Stream->CR & DMA_SxCR_DMEIE)) {
		TM_DMA_DirectModeErrorHandler(DMA_Stream);
	}
	/* Check FIFO error flag */
	if ((flags & DMA_FLAG_FEIF) && (DMA_Stream->FCR & DMA_SxFCR_FEIE)) {
		TM_DMA_FIFOErrorHandler(DMA_Stream);
	}
}

/* Handle all DMA interrupt handlers possible */
#ifndef DMA1_STREAM0_DISABLE_IRQHANDLER
void DMA1_Stream0_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream0);
}
#endif
#ifndef DMA1_STREAM1_DISABLE_IRQHANDLER
void DMA1_Stream1_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream1);
}
#endif
#ifndef DMA1_STREAM2_DISABLE_IRQHANDLER
void DMA1_Stream2_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream2);
}
#endif
#ifndef DMA1_STREAM3_DISABLE_IRQHANDLER
void DMA1_Stream3_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream3);
}
#endif
#ifndef DMA1_STREAM4_DISABLE_IRQHANDLER
void DMA1_Stream4_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream4);
}
#endif
#ifndef DMA1_STREAM5_DISABLE_IRQHANDLER
void DMA1_Stream5_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream5);
}
#endif
#ifndef DMA1_STREAM6_DISABLE_IRQHANDLER
void DMA1_Stream6_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream6);
}
#endif
#ifndef DMA1_STREAM7_DISABLE_IRQHANDLER
void DMA1_Stream7_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA1_Stream7);
}
#endif
#ifndef DMA2_STREAM0_DISABLE_IRQHANDLER
void DMA2_Stream0_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream0);
}
#endif
#ifndef DMA2_STREAM1_DISABLE_IRQHANDLER
void DMA2_Stream1_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream1);
}
#endif
#ifndef DMA2_STREAM2_DISABLE_IRQHANDLER
void DMA2_Stream2_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream2);
}
#endif
#ifndef DMA2_STREAM3_DISABLE_IRQHANDLER
void DMA2_Stream3_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream3);
}
#endif
#ifndef DMA2_STREAM4_DISABLE_IRQHANDLER
void DMA2_Stream4_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream4);
}
#endif
#ifndef DMA2_STREAM5_DISABLE_IRQHANDLER
void DMA2_Stream5_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream5);
}
#endif
#ifndef DMA2_STREAM6_DISABLE_IRQHANDLER
void DMA2_Stream6_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream6);
}
#endif
#ifndef DMA2_STREAM7_DISABLE_IRQHANDLER
void DMA2_Stream7_IRQHandler(void) {
	/* Call user function */
	TM_DMA_INT_ProcessInterrupt(DMA2_Stream7);
}
#endif
