/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
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
#include "tm_stm32_dac.h"

/* DAC HANDLE */
static DAC_HandleTypeDef DAC_Handle[2];
static DAC_ChannelConfTypeDef DAC_ChannelConf;

void TM_DAC_Init(TM_DAC_Channel_t DACx) {
	uint16_t GPIO_Pin;
	
	/* Select proper GPIO pin */
	if (DACx == TM_DAC_Channel_1) {
		GPIO_Pin = GPIO_PIN_4;
	} else {
		GPIO_Pin = GPIO_PIN_5;
	}
	
	/* Initialize GPIO pin */
	TM_GPIO_Init(GPIOA, GPIO_Pin, TM_GPIO_Mode_AN, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast);

	/* Enable DAC clock */
#if defined(__HAL_RCC_DAC1_CLK_ENABLE)
	__HAL_RCC_DAC1_CLK_ENABLE();
#else
	__HAL_RCC_DAC_CLK_ENABLE();
#endif
	
	/* Set DAC options */
	DAC_Handle[(uint8_t)DACx].Instance = DAC;
	
	/* Set channel settings */
	DAC_ChannelConf.DAC_Trigger = DAC_TRIGGER_NONE;
	DAC_ChannelConf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	
	/* Init DAC */
	HAL_DAC_Init(&DAC_Handle[(uint8_t)DACx]);
	
	/* Init and enable proper DAC */
	if (DACx == TM_DAC_Channel_1) {
		/* Init channel 1 */
		HAL_DAC_ConfigChannel(&DAC_Handle[(uint8_t)DACx], &DAC_ChannelConf, DAC_CHANNEL_1);
		
		/* Enable DAC channel 1 */
		DAC->CR |= DAC_CR_EN1;
	} else {
		/* Init channel 2 */
		HAL_DAC_ConfigChannel(&DAC_Handle[(uint8_t)DACx], &DAC_ChannelConf, DAC_CHANNEL_2);
		
		/* Enable DAC channel 2 */
		DAC->CR |= DAC_CR_EN2;
	}
}

void TM_DAC_SetValue(TM_DAC_Channel_t DACx, uint16_t value) {
	/* Check value */
	if (value > 0x0FFF) {
		value = 0x0FFF;
	}
	
	/* Set 12-bit value, right aligned */
	if (DACx == TM_DAC_Channel_1) {
#if defined(STM32F4xx) || defined(STM32F7xx)
		DAC->DHR12R1 = value;
#else
		HAL_DAC_SetValue(&DAC_Handle[(uint8_t)DACx], DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
#endif
	} else {
#if defined(STM32F4xx) || defined(STM32F7xx)
		DAC->DHR12R2 = value;
#else
		HAL_DAC_SetValue(&DAC_Handle[(uint8_t)DACx], DAC_CHANNEL_2, DAC_ALIGN_12B_R, value);
#endif
	}
	
}
