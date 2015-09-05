/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
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
#include "tm_stm32_usart.h"

/* Set variables for buffers */
#ifdef USART1
uint8_t USART1_Buffer[TM_USART1_BUFFER_SIZE];
#endif
#ifdef USART2
uint8_t USART2_Buffer[TM_USART2_BUFFER_SIZE];
#endif
#ifdef USART3
uint8_t USART3_Buffer[TM_USART3_BUFFER_SIZE];
#endif
#ifdef UART4
uint8_t UART4_Buffer[TM_UART4_BUFFER_SIZE];
#endif
#ifdef UART5
uint8_t UART5_Buffer[TM_UART5_BUFFER_SIZE];
#endif
#ifdef USART6
uint8_t USART6_Buffer[TM_USART6_BUFFER_SIZE];
#endif
#ifdef UART7
uint8_t UART7_Buffer[TM_UART7_BUFFER_SIZE];
#endif
#ifdef UART8
uint8_t UART8_Buffer[TM_UART8_BUFFER_SIZE];
#endif

#ifdef USART1
TM_BUFFER_t TM_USART1 = {TM_USART1_BUFFER_SIZE, 0, 0, USART1_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef USART2
TM_BUFFER_t TM_USART2 = {TM_USART2_BUFFER_SIZE, 0, 0, USART2_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef USART3
TM_BUFFER_t TM_USART3 = {TM_USART3_BUFFER_SIZE, 0, 0, USART3_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef UART4
TM_BUFFER_t TM_UART4 = {TM_UART4_BUFFER_SIZE, 0, 0, UART4_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef UART5
TM_BUFFER_t TM_UART5 = {TM_UART5_BUFFER_SIZE, 0, 0, UART5_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef USART6
TM_BUFFER_t TM_USART6 = {TM_USART6_BUFFER_SIZE, 0, 0, USART6_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef UART7
TM_BUFFER_t TM_UART7 = {TM_UART7_BUFFER_SIZE, 0, 0, UART7_Buffer, 0, USART_STRING_DELIMITER};
#endif
#ifdef UART8
TM_BUFFER_t TM_UART8 = {TM_UART8_BUFFER_SIZE, 0, 0, UART8_Buffer, 0, USART_STRING_DELIMITER};
#endif

/* Private functions */
void TM_USART1_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART2_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART3_InitPins(TM_USART_PinsPack_t pinspack);
void TM_UART4_InitPins(TM_USART_PinsPack_t pinspack);
void TM_UART5_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART6_InitPins(TM_USART_PinsPack_t pinspack);
void TM_UART7_InitPins(TM_USART_PinsPack_t pinspack);
void TM_UART8_InitPins(TM_USART_PinsPack_t pinspack);
void TM_USART_INT_InsertToBuffer(TM_BUFFER_t* u, uint8_t c);
static void TM_USART_INT_ClearAllFlags(USART_TypeDef* USARTx);
static TM_BUFFER_t* TM_USART_INT_GetUSARTBuffer(USART_TypeDef* USARTx);
static uint8_t TM_USART_INT_GetSubPriority(USART_TypeDef* USARTx);
uint8_t TM_USART_BufferFull(USART_TypeDef* USARTx);

/* Private initializator function */
static void TM_USART_INT_Init(
	USART_TypeDef* USARTx,
	TM_USART_PinsPack_t pinspack,
	uint32_t baudrate,
	TM_USART_HardwareFlowControl_t FlowControl,
	uint32_t Mode,
	uint32_t Parity,
	uint32_t StopBits,
	uint32_t WordLength
);

void TM_USART_Init(USART_TypeDef* USARTx, TM_USART_PinsPack_t pinspack, uint32_t baudrate) {
#ifdef USART1
	if (USARTx == USART1) {
		TM_USART_INT_Init(USART1, pinspack, baudrate, TM_USART1_HARDWARE_FLOW_CONTROL, TM_USART1_MODE, TM_USART1_PARITY, TM_USART1_STOP_BITS, TM_USART1_WORD_LENGTH);
	}
#endif
#ifdef USART2
	if (USARTx == USART2) {
		TM_USART_INT_Init(USART2, pinspack, baudrate, TM_USART2_HARDWARE_FLOW_CONTROL, TM_USART2_MODE, TM_USART2_PARITY, TM_USART2_STOP_BITS, TM_USART2_WORD_LENGTH);
	}
#endif
#ifdef USART3
	if (USARTx == USART3) {
		TM_USART_INT_Init(USART3, pinspack, baudrate, TM_USART3_HARDWARE_FLOW_CONTROL, TM_USART3_MODE, TM_USART3_PARITY, TM_USART3_STOP_BITS, TM_USART3_WORD_LENGTH);
	}
#endif
#ifdef UART4
	if (USARTx == UART4) {
		TM_USART_INT_Init(UART4, pinspack, baudrate, TM_UART4_HARDWARE_FLOW_CONTROL, TM_UART4_MODE, TM_UART4_PARITY, TM_UART4_STOP_BITS, TM_UART4_WORD_LENGTH);
	}
#endif
#ifdef UART5
	if (USARTx == UART5) {
		TM_USART_INT_Init(UART5, pinspack, baudrate, TM_UART5_HARDWARE_FLOW_CONTROL, TM_UART5_MODE, TM_UART5_PARITY, TM_UART5_STOP_BITS, TM_UART5_WORD_LENGTH);
	}
#endif
#ifdef USART6
	if (USARTx == USART6) {
		TM_USART_INT_Init(USART6, pinspack, baudrate, TM_USART6_HARDWARE_FLOW_CONTROL, TM_USART6_MODE, TM_USART6_PARITY, TM_USART6_STOP_BITS, TM_USART6_WORD_LENGTH);
	}
#endif
#ifdef UART7
	if (USARTx == UART7) {
		TM_USART_INT_Init(UART7, pinspack, baudrate, TM_UART7_HARDWARE_FLOW_CONTROL, TM_UART7_MODE, TM_UART7_PARITY, TM_UART7_STOP_BITS, TM_UART7_WORD_LENGTH);
	}
#endif
#ifdef UART8
	if (USARTx == UART8) {
		TM_USART_INT_Init(UART8, pinspack, baudrate, TM_UART8_HARDWARE_FLOW_CONTROL, TM_UART8_MODE, TM_UART8_PARITY, TM_UART8_STOP_BITS, TM_UART8_WORD_LENGTH);
	}
#endif
}

void TM_USART_InitWithFlowControl(USART_TypeDef* USARTx, TM_USART_PinsPack_t pinspack, uint32_t baudrate, TM_USART_HardwareFlowControl_t FlowControl) {
#ifdef USART1
	if (USARTx == USART1) {
		TM_USART_INT_Init(USART1, pinspack, baudrate, FlowControl, TM_USART1_MODE, TM_USART1_PARITY, TM_USART1_STOP_BITS, TM_USART1_WORD_LENGTH);
	}
#endif
#ifdef USART2
	if (USARTx == USART2) {
		TM_USART_INT_Init(USART2, pinspack, baudrate, FlowControl, TM_USART2_MODE, TM_USART2_PARITY, TM_USART2_STOP_BITS, TM_USART2_WORD_LENGTH);
	}
#endif
#ifdef USART3
	if (USARTx == USART3) {
		TM_USART_INT_Init(USART3, pinspack, baudrate, FlowControl, TM_USART3_MODE, TM_USART3_PARITY, TM_USART3_STOP_BITS, TM_USART3_WORD_LENGTH);
	}
#endif
#ifdef UART4
	if (USARTx == UART4) {
		TM_USART_INT_Init(UART4, pinspack, baudrate, FlowControl, TM_UART4_MODE, TM_UART4_PARITY, TM_UART4_STOP_BITS, TM_UART4_WORD_LENGTH);
	}
#endif
#ifdef UART5
	if (USARTx == UART5) {
		TM_USART_INT_Init(UART5, pinspack, baudrate, FlowControl, TM_UART5_MODE, TM_UART5_PARITY, TM_UART5_STOP_BITS, TM_UART5_WORD_LENGTH);
	}
#endif
#ifdef USART6
	if (USARTx == USART6) {
		TM_USART_INT_Init(USART6, pinspack, baudrate, FlowControl, TM_USART6_MODE, TM_USART6_PARITY, TM_USART6_STOP_BITS, TM_USART6_WORD_LENGTH);
	}
#endif
#ifdef UART7
	if (USARTx == UART7) {
		TM_USART_INT_Init(UART7, pinspack, baudrate, FlowControl, TM_UART7_MODE, TM_UART7_PARITY, TM_UART7_STOP_BITS, TM_UART7_WORD_LENGTH);
	}
#endif
#ifdef UART8
	if (USARTx == UART8) {
		TM_USART_INT_Init(UART8, pinspack, baudrate, FlowControl, TM_UART8_MODE, TM_UART8_PARITY, TM_UART8_STOP_BITS, TM_UART8_WORD_LENGTH);
	}
#endif
}

uint8_t TM_USART_Getc(USART_TypeDef* USARTx) {
	uint8_t c;
	
	/* Read character from buffer */
	if (TM_BUFFER_Read(TM_USART_INT_GetUSARTBuffer(USARTx), &c, 1)) {
		return c;
	}
	
	/* Read was not successfull */
	return 0;
}

uint16_t TM_USART_Gets(USART_TypeDef* USARTx, char* buffer, uint16_t bufsize) {
	return TM_BUFFER_ReadString(TM_USART_INT_GetUSARTBuffer(USARTx), buffer, bufsize);
}

uint8_t TM_USART_BufferEmpty(USART_TypeDef* USARTx) {
	return TM_BUFFER_GetFull(TM_USART_INT_GetUSARTBuffer(USARTx)) == 0;
}

uint8_t TM_USART_BufferFull(USART_TypeDef* USARTx) {
	return TM_BUFFER_GetFree(TM_USART_INT_GetUSARTBuffer(USARTx)) == 0;
}

void TM_USART_ClearBuffer(USART_TypeDef* USARTx) {
	TM_BUFFER_Reset(TM_USART_INT_GetUSARTBuffer(USARTx));
}

void TM_USART_SetCustomStringEndCharacter(USART_TypeDef* USARTx, uint8_t Character) {
	TM_BUFFER_SetStringDelimiter(TM_USART_INT_GetUSARTBuffer(USARTx), Character);
}

uint8_t TM_USART_FindCharacter(USART_TypeDef* USARTx, uint8_t c) {
	return TM_BUFFER_FindElement(TM_USART_INT_GetUSARTBuffer(USARTx), c);
}

void TM_USART_Puts(USART_TypeDef* USARTx, char* str) {
	/* Go through entire string */
	while (*str) {
		/* Wait to be ready, buffer empty */
		USART_WAIT(USARTx);
		/* Send data */
		USART_WRITE_DATA(USARTx, (uint16_t)(*str++));
		/* Wait to be ready, buffer empty */
		USART_WAIT(USARTx);
	}
}

void TM_USART_Send(USART_TypeDef* USARTx, uint8_t* DataArray, uint16_t count) {
	/* Go through entire data array */
	while (count--) {
		/* Wait to be ready, buffer empty */
		USART_WAIT(USARTx);
		/* Send data */
		USART_WRITE_DATA(USARTx, (uint16_t)(*DataArray++));
		/* Wait to be ready, buffer empty */
		USART_WAIT(USARTx);
	}
}

uint8_t TM_USART_FindString(USART_TypeDef* USARTx, char* str) {
	return TM_BUFFER_Find(TM_USART_INT_GetUSARTBuffer(USARTx), (uint8_t *)str, strlen(str));
}

__weak void TM_USART_InitCustomPinsCallback(USART_TypeDef* USARTx, uint16_t AlternateFunction) { 
	/* NOTE: This function Should not be modified, when the callback is needed,
           the TM_USART_InitCustomPinsCallback could be implemented in the user file
   */
}

/* Private functions */
static void TM_USART_INT_InsertToBuffer(TM_BUFFER_t* u, uint8_t c) {
	TM_BUFFER_Write(u, &c, 1);
}

static TM_BUFFER_t* TM_USART_INT_GetUSARTBuffer(USART_TypeDef* USARTx) {
	TM_BUFFER_t* u;
	
#ifdef USART1
	if (USARTx == USART1) {
		u = &TM_USART1;
	}
#endif
#ifdef USART2
	if (USARTx == USART2) {
		u = &TM_USART2;
	}
#endif
#ifdef USART3
	if (USARTx == USART3) {
		u = &TM_USART3;
	}
#endif
#ifdef UART4
	if (USARTx == UART4) {
		u = &TM_UART4;
	}
#endif
#ifdef UART5
	if (USARTx == UART5) {
		u = &TM_UART5;
	}
#endif
#ifdef USART6
	if (USARTx == USART6) {
		u = &TM_USART6;
	}
#endif
#ifdef UART7
	if (USARTx == UART7) {
		u = &TM_UART7;
	}
#endif
#ifdef UART8
	if (USARTx == UART8) {
		u = &TM_UART8;
	}
#endif

	return u;
}

static uint8_t TM_USART_INT_GetSubPriority(USART_TypeDef* USARTx) {
	uint8_t u;
	
#ifdef USART1
	if (USARTx == USART1) {
		u = 0;
	}
#endif
#ifdef USART2
	if (USARTx == USART2) {
		u = 1;
	}
#endif
#ifdef USART3
	if (USARTx == USART3) {
		u = 2;
	}
#endif
#ifdef UART4
	if (USARTx == UART4) {
		u = 4;
	}
#endif
#ifdef UART5
	if (USARTx == UART5) {
		u = 5;
	}
#endif
#ifdef USART6
	if (USARTx == USART6) {
		u = 6;
	}
#endif
#ifdef UART7
	if (USARTx == UART7) {
		u = 7;
	}
#endif
#ifdef UART8
	if (USARTx == UART8) {
		u = 8;
	}
#endif
	
	return u;
}

#ifdef USART1
void TM_USART1_InitPins(TM_USART_PinsPack_t pinspack) {	
	/* Init pins */
#if defined(GPIOA)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART1);
	}
#endif
#if defined(GPIOB)
	if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART1);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(USART1, GPIO_AF7_USART1);
	}
}
#endif

#ifdef USART2
void TM_USART2_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOA)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_2 | GPIO_PIN_3, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART2);
	}
#endif
#if defined(GPIOD)
	if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_5 | GPIO_PIN_6, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART2);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(USART2, GPIO_AF7_USART2);
	}
}
#endif

#ifdef USART3
void TM_USART3_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOB)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_10 | GPIO_PIN_11, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART3);
	}
#endif
#if defined(GPIOC)
	if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_10 | GPIO_PIN_11, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART3);
	}
#endif
#if defined(GPIOD)
	if (pinspack == TM_USART_PinsPack_3) {
		TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_8 | GPIO_PIN_9, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF7_USART3);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(USART3, GPIO_AF7_USART3);
	}
}
#endif

#ifdef UART4
void TM_UART4_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOA)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_UART4);
	}
#endif
#if defined(GPIOC)
	if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_10 | GPIO_PIN_11, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_UART4);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(UART4, GPIO_AF8_UART4);
	}
}
#endif

#ifdef UART5
void TM_UART5_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOC) && defined(GPIOD)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF_UART5);
		TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_2, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF_UART5);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(UART5, GPIO_AF_UART5);
	}
}
#endif

#ifdef USART6
void TM_USART6_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOC)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_6 | GPIO_PIN_7, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_USART6);
	}
#endif
#if defined(GPIOG)
	if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_14 | GPIO_PIN_9, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_USART6);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(USART6, GPIO_AF8_USART6);
	}
}
#endif

#ifdef UART7
void TM_UART7_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOE)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_8 | GPIO_PIN_7, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_UART7);
	}
#endif
#if defined(GPIOF)
	if (pinspack == TM_USART_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_7 | GPIO_PIN_6, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_UART7);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(UART7, GPIO_AF8_UART7);
	}
}
#endif

#ifdef UART8
void TM_UART8_InitPins(TM_USART_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOE)
	if (pinspack == TM_USART_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_1 | GPIO_PIN_0, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF8_UART8);
	}
#endif
	if (pinspack == TM_USART_PinsPack_Custom) {
		/* Init custom pins, callback used */
		TM_USART_InitCustomPinsCallback(UART8, GPIO_AF8_UART8);
	}
}
#endif

#ifdef USART1
void USART1_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (USART1->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_USART1_USE_CUSTOM_IRQ
		/* Call user function */
		TM_USART1_ReceiveHandler(USART_READ_DATA(USART1));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_USART1, USART_READ_DATA(USART1));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(USART1);
}
#endif

#ifdef USART2
void USART2_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (USART2->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_USART2_USE_CUSTOM_IRQ
		/* Call user function */
		TM_USART2_ReceiveHandler(USART_READ_DATA(USART2));
#else 
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_USART2, USART_READ_DATA(USART2));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(USART2);
}
#endif

#ifdef USART3
void USART3_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (USART3->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_USART3_USE_CUSTOM_IRQ
		/* Call user function */
		TM_USART3_ReceiveHandler(USART_READ_DATA(USART3));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_USART3, USART_READ_DATA(USART3));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(USART3);
}
#endif

#ifdef UART4
void UART4_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (UART4->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_UART4_USE_CUSTOM_IRQ
		/* Call user function */
		TM_UART4_ReceiveHandler(USART_READ_DATA(UART4));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_UART4, USART_READ_DATA(UART4));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(UART4);
}
#endif

#ifdef UART5
void UART5_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (UART5->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_UART5_USE_CUSTOM_IRQ
		/* Call user function */
		TM_UART5_ReceiveHandler(USART_READ_DATA(UART5));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_UART5, USART_READ_DATA(UART5));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(UART5);
}
#endif

#ifdef USART6
void USART6_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (USART6->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_USART6_USE_CUSTOM_IRQ
		/* Call user function */
		TM_USART6_ReceiveHandler(USART_READ_DATA(USART6));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_USART6, USART_READ_DATA(USART6));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(USART6);
}
#endif

#ifdef UART7
void UART7_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (UART7->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_UART7_USE_CUSTOM_IRQ
		/* Call user function */
		TM_UART7_ReceiveHandler(USART_READ_DATA(UART7));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_UART7, USART_READ_DATA(UART7));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(UART7);
}
#endif

#ifdef UART8
void UART8_IRQHandler(void) {
	/* Check if interrupt was because data is received */
	if (UART8->USART_STATUS_REG & USART_ISR_RXNE) {
#ifdef TM_UART8_USE_CUSTOM_IRQ
		/* Call user function */
		TM_UART8_ReceiveHandler(USART_READ_DATA(UART8));
#else
		/* Put received data into internal buffer */
		TM_USART_INT_InsertToBuffer(&TM_UART8, USART_READ_DATA(UART8));
#endif
	}
	
	/* Clear all USART flags */
	TM_USART_INT_ClearAllFlags(UART8);
}
#endif

static void TM_USART_INT_Init(
	USART_TypeDef* USARTx,
	TM_USART_PinsPack_t pinspack,
	uint32_t baudrate,
	TM_USART_HardwareFlowControl_t FlowControl,
	uint32_t Mode,
	uint32_t Parity,
	uint32_t StopBits,
	uint32_t WordLength
) {
	UART_HandleTypeDef UARTHandle;
	IRQn_Type irq;

	/* Set USART baudrate */
	UARTHandle.Instance = USARTx;
	UARTHandle.Init.BaudRate = baudrate;
	
	/*
	 * Initialize USARTx pins
	 * Set channel for USARTx NVIC
	 */
#ifdef USART1
	if (USARTx == USART1) {
		/* Enable USART clock */
		__HAL_RCC_USART1_CLK_ENABLE();
		
		/* Init pins */
		TM_USART1_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = USART1_IRQn;
	}
#endif
#ifdef USART2
	if (USARTx == USART2) {
		/* Enable USART clock */
		__HAL_RCC_USART2_CLK_ENABLE();
		
		/* Init pins */
		TM_USART2_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = USART2_IRQn;
	}
#endif
#ifdef USART3
	if (USARTx == USART3) {
		/* Enable USART clock */
		__HAL_RCC_USART3_CLK_ENABLE();
		
		/* Init pins */
		TM_USART3_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = USART3_IRQn;
	}
#endif
#ifdef UART4
	if (USARTx == UART4) {
		/* Enable UART clock */
		__HAL_RCC_UART4_CLK_ENABLE();
		
		/* Init pins */
		TM_UART4_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = UART4_IRQn;
	}
#endif
#ifdef UART5
	if (USARTx == UART5) {
		/* Enable UART clock */
		__HAL_RCC_UART5_CLK_ENABLE();

		/* Init pins */
		TM_UART5_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = UART5_IRQn;
	}
#endif
#ifdef USART6
	if (USARTx == USART6) {
		/* Enable UART clock */
		__HAL_RCC_USART6_CLK_ENABLE();
		
		/* Init pins */
		TM_USART6_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = USART6_IRQn;
	}
#endif
#ifdef UART7
	if (USARTx == UART7) {
		/* Enable UART clock */
		__HAL_RCC_UART7_CLK_ENABLE();
		
		/* Init pins */
		TM_UART7_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = UART7_IRQn;
	}
#endif
#ifdef UART8
	if (USARTx == UART8) {
		/* Enable UART clock */
		__HAL_RCC_UART8_CLK_ENABLE();

		/* Init pins */
		TM_UART8_InitPins(pinspack);
		
		/* Set IRQ channel */
		irq = UART8_IRQn;
	}
#endif
	
	/* Fill default settings */
	UARTHandle.Init.HwFlowCtl = FlowControl;
	UARTHandle.Init.Mode = Mode;
	UARTHandle.Init.Parity = Parity;
	UARTHandle.Init.StopBits = StopBits;
	UARTHandle.Init.WordLength = WordLength;
	
	/* Disable if not already */
	USARTx->CR1 &= ~USART_CR1_UE;
	
	/* Init */
	HAL_UART_Init(&UARTHandle);
	
	/* Enable RX interrupt */
	USARTx->CR1 |= USART_CR1_RXNEIE;

	/* Set priority */
	HAL_NVIC_SetPriority(irq, USART_NVIC_PRIORITY, TM_USART_INT_GetSubPriority(USARTx));
	
	/* Enable interrupt */
	HAL_NVIC_EnableIRQ(irq);
	
	/* Enable USART peripheral */
	USARTx->CR1 |= USART_CR1_UE;
}

static UART_HandleTypeDef UART_Handle;
static void TM_USART_INT_ClearAllFlags(USART_TypeDef* USARTx) {
	UART_Handle.Instance = USARTx;
	
#ifdef __HAL_UART_CLEAR_PEFLAG
	__HAL_UART_CLEAR_PEFLAG(&UART_Handle);
#endif
#ifdef __HAL_UART_CLEAR_FEFLAG
	__HAL_UART_CLEAR_FEFLAG(&UART_Handle);
#endif
#ifdef __HAL_UART_CLEAR_NEFLAG
	__HAL_UART_CLEAR_NEFLAG(&UART_Handle);
#endif
#ifdef __HAL_UART_CLEAR_OREFLAG
	__HAL_UART_CLEAR_OREFLAG(&UART_Handle);
#endif
#ifdef __HAL_UART_CLEAR_IDLEFLAG
	__HAL_UART_CLEAR_IDLEFLAG(&UART_Handle);
#endif
}

