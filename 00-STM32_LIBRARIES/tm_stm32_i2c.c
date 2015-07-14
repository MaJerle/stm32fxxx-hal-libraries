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
#include "tm_stm32_i2c.h"

/* Private variables */
static uint32_t TM_I2C_Timeout;
static uint32_t TM_I2C_INT_Clocks[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

/* Private defines */
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

/* Handle values for I2C */
#ifdef I2C1
static I2C_HandleTypeDef I2C1Handle;
#endif
#ifdef I2C2
static I2C_HandleTypeDef I2C2Handle;
#endif
#ifdef I2C3
static I2C_HandleTypeDef I2C3Handle;
#endif
#ifdef I2C4
static I2C_HandleTypeDef I2C4Handle;
#endif

/* Private functions */
static void TM_I2C1_INT_InitPins(TM_I2C_PinsPack_t pinspack);
static void TM_I2C2_INT_InitPins(TM_I2C_PinsPack_t pinspack);
static void TM_I2C3_INT_InitPins(TM_I2C_PinsPack_t pinspack);
static void TM_I2C4_INT_InitPins(TM_I2C_PinsPack_t pinspack);

static I2C_HandleTypeDef* TM_I2C_GetHandle(I2C_TypeDef* I2Cx) {
#ifdef I2C1
	if (I2Cx == I2C1) {
		return &I2C1Handle;
	}
#endif
#ifdef I2C2
	if (I2Cx == I2C2) {
		return &I2C2Handle;
	}
#endif
#ifdef I2C3
	if (I2Cx == I2C3) {
		return &I2C3Handle;
	}
#endif
#ifdef I2C4
	if (I2Cx == I2C4) {
		return &I2C4Handle;
	}
#endif
}

void TM_I2C_Init(I2C_TypeDef* I2Cx, TM_I2C_PinsPack_t pinspack, uint32_t clockSpeed) {
#if defined(STM32F7xx)
	/* 100kHz @ 50MHz APB clock */
	uint32_t I2C_Timing = 0x40912732;
#endif
	/* Disable I2C first */
	I2Cx->CR1 &= ~I2C_CR1_PE;
	
#ifdef I2C1
	if (I2Cx == I2C1) {
		/* Enable clock */
		__HAL_RCC_I2C1_CLK_ENABLE();
		
		/* Enable pins */
		TM_I2C1_INT_InitPins(pinspack);
				
		I2C1Handle.Instance = I2C1;
		I2C1Handle.Init.OwnAddress2 = 0xFF;
		I2C1Handle.Init.OwnAddress1 = 0x00;
		I2C1Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		I2C1Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		I2C1Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		I2C1Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; 
#if defined(STM32F7xx)
		I2C1Handle.Init.Timing = I2C_Timing;
#else
		I2C1Handle.Init.ClockSpeed = clockSpeed;
		I2C1Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
#endif
	
		/* Initialize I2C */
		HAL_I2C_Init(&I2C1Handle);
	}
#endif
	
#ifdef I2C2	
	if (I2Cx == I2C2) {
		/* Enable clock */
		__HAL_RCC_I2C2_CLK_ENABLE();
		
		/* Enable pins */
		TM_I2C2_INT_InitPins(pinspack);
		
		I2C2Handle.Instance = I2C2;
		I2C2Handle.Init.OwnAddress2 = 0xFF;
		I2C2Handle.Init.OwnAddress1 = 0x00;
		I2C2Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		I2C2Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		I2C2Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		I2C2Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; 
#if defined(STM32F7xx)
		I2C2Handle.Init.Timing = I2C_Timing;
#else
		I2C2Handle.Init.ClockSpeed = clockSpeed;
		I2C2Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
#endif
	
		/* Initialize I2C */
		HAL_I2C_Init(&I2C2Handle);
	} 
#endif
	
#ifdef I2C3
	if (I2Cx == I2C3) {
		/* Enable clock */
		__HAL_RCC_I2C3_CLK_ENABLE();
		
		/* Enable pins */
		TM_I2C3_INT_InitPins(pinspack);
		
		I2C3Handle.Instance = I2C3;
		I2C3Handle.Init.OwnAddress2 = 0xFF;
		I2C3Handle.Init.OwnAddress1 = 0x00;
		I2C3Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		I2C3Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		I2C3Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		I2C3Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; 
#if defined(STM32F7xx)
		I2C3Handle.Init.Timing = I2C_Timing;
#else
		I2C3Handle.Init.ClockSpeed = clockSpeed;
		I2C3Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
#endif
	
		/* Initialize I2C */
		HAL_I2C_Init(&I2C3Handle);
	}
#endif
	
#ifdef I2C4
	if (I2Cx == I2C4) {
		/* Enable clock */
		__HAL_RCC_I2C4_CLK_ENABLE();
		
		/* Enable pins */
		TM_I2C4_INT_InitPins(pinspack);
		
		I2C4Handle.Instance = I2C4;
		I2C4Handle.Init.OwnAddress2 = 0xFF;
		I2C4Handle.Init.OwnAddress1 = 0x00;
		I2C4Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		I2C4Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		I2C4Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		I2C4Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; 
#if defined(STM32F7xx)
		I2C4Handle.Init.Timing = I2C_Timing;
#else
		I2C4Handle.Init.ClockSpeed = clockSpeed;
		I2C4Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
#endif
	
		/* Initialize I2C */
		HAL_I2C_Init(&I2C4Handle);
	}
#endif
	
	/* Enable I2C */
	I2Cx->CR1 |= I2C_CR1_PE;
}

uint8_t TM_I2C_Read(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg) {
	uint8_t data;
	I2C_HandleTypeDef* Handle = TM_I2C_GetHandle(I2Cx);
	
	/* Receive 1 byte from I2C */
	
	/* Send address */
	HAL_I2C_Master_Transmit(Handle, address, &reg, 1, 1000);
	
	/* Receive single byte */
	HAL_I2C_Master_Receive(Handle, address, &data, 1, 1000);
	
	/* Return data */
	return data;
}

void TM_I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	I2C_HandleTypeDef* Handle = TM_I2C_GetHandle(I2Cx);
	
	/* Receive 1 byte from I2C */
	
	/* Send address */
	HAL_I2C_Master_Transmit(Handle, address, &reg, 1, 1000);
	
	/* Receive multiple byte */
	HAL_I2C_Master_Receive(Handle, address, data, count, 1000);
}

uint8_t TM_I2C_ReadNoRegister(I2C_TypeDef* I2Cx, uint8_t address) {
	uint8_t data;
	I2C_HandleTypeDef* Handle = TM_I2C_GetHandle(I2Cx);
	
	/* Receive 1 byte from I2C */
	
	/* Receive single byte */
	HAL_I2C_Master_Receive(Handle, address, &data, 1, 1000);
	
	/* Return data */
	return data;
}

void TM_I2C_ReadMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count) {
	I2C_HandleTypeDef* Handle = TM_I2C_GetHandle(I2Cx);
	
	/* Receive multi bytes from I2C */
	
	/* Receive single byte */
	HAL_I2C_Master_Receive(Handle, address, data, 1, 1000);
}

void TM_I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data) {
	uint8_t d[2];
	I2C_HandleTypeDef* Handle = TM_I2C_GetHandle(I2Cx);
		
	/* Format array */
	d[0] = reg;
	d[1] = data;
	
	HAL_I2C_Master_Transmit(Handle, address, &reg, 1, 1000);
	HAL_I2C_Master_Transmit(Handle, address, &data, 1, 1000);
}

void TM_I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {

}

void TM_I2C_WriteNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t data) {

}

void TM_I2C_WriteMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count) {

}

uint8_t TM_I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address) {
	
}

__weak void TM_I2C_InitCustomPinsCallback(I2C_TypeDef* I2Cx, uint16_t AlternateFunction) {
	/* Custom user function. */
	/* In case user needs functionality for custom pins, this function should be declared outside this library */
}

/* Private functions */
#ifdef I2C1
static void TM_I2C1_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOB)
	if (pinspack == TM_I2C_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C1);
	}
#endif
#if defined(GPIOB)
	if (pinspack == TM_I2C_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C1);
	}
#endif
#if defined(GPIOB)
	if (pinspack == TM_I2C_PinsPack_3) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_6 | GPIO_PIN_9, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C1);
	}
#endif
	if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPinsCallback(I2C1, GPIO_AF4_I2C1);
	}
}
#endif
#ifdef I2C2
static void TM_I2C2_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOB)
	if (pinspack == TM_I2C_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_10 | GPIO_PIN_11, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C2);
	}
#endif
#if defined(GPIOF)
	if (pinspack == TM_I2C_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_0 | GPIO_PIN_1, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C2);
	}
#endif
#if defined(GPIOH)
	if (pinspack == TM_I2C_PinsPack_3) {
		TM_GPIO_InitAlternate(GPIOH, GPIO_PIN_4 | GPIO_PIN_5, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C2);
	}
#endif
	if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPinsCallback(I2C2, GPIO_AF4_I2C2);
	}
}
#endif
#ifdef I2C3
static void TM_I2C3_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
#if defined(GPIOA) && defined(GPIOC)
	if (pinspack == TM_I2C_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_8, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C3);
		TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_9, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C3);
	}
#endif
#if defined(GPIOH)
	if (pinspack == TM_I2C_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOH, GPIO_PIN_7 | GPIO_PIN_8, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF4_I2C3);
	}
#endif
	if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPinsCallback(I2C3, GPIO_AF4_I2C3);
	}
}
#endif
#ifdef I2C4
static void TM_I2C4_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
	if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPinsCallback(I2C3, GPIO_AF4_I2C4);
	}
}
#endif
