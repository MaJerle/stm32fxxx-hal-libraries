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
#include "tm_stm32_adc.h"

/* Private functions */
static void TM_ADC_INT_Channel_0_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_1_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_2_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_3_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_4_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_5_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_6_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_7_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_8_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_9_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_10_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_11_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_12_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_13_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_14_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_Channel_15_Init(ADC_TypeDef* ADCx);
static void TM_ADC_INT_InitPin(GPIO_TypeDef* GPIOx, uint16_t PinX);

/* Private variables */
ADC_HandleTypeDef AdcHandle;

void TM_ADC_Init(ADC_TypeDef* ADCx, TM_ADC_Channel_t channel) {
	TM_ADC_Channel_t ch = (TM_ADC_Channel_t) channel;
	if (ch == TM_ADC_Channel_0) {
		TM_ADC_INT_Channel_0_Init(ADCx);
	} else if (ch == TM_ADC_Channel_1) {
		TM_ADC_INT_Channel_1_Init(ADCx);
	} else if (ch == TM_ADC_Channel_2) {
		TM_ADC_INT_Channel_2_Init(ADCx);
	} else if (ch == TM_ADC_Channel_3) {
		TM_ADC_INT_Channel_3_Init(ADCx);
	} else if (ch == TM_ADC_Channel_4) {
		TM_ADC_INT_Channel_4_Init(ADCx);
	} else if (ch == TM_ADC_Channel_5) {
		TM_ADC_INT_Channel_5_Init(ADCx);
	} else if (ch == TM_ADC_Channel_6) {
		TM_ADC_INT_Channel_6_Init(ADCx);
	} else if (ch == TM_ADC_Channel_7) {
		TM_ADC_INT_Channel_7_Init(ADCx);
	} else if (ch == TM_ADC_Channel_8) {
		TM_ADC_INT_Channel_8_Init(ADCx);
	} else if (ch == TM_ADC_Channel_9) {
		TM_ADC_INT_Channel_9_Init(ADCx);
	} else if (ch == TM_ADC_Channel_10) {
		TM_ADC_INT_Channel_10_Init(ADCx);
	} else if (ch == TM_ADC_Channel_11) {
		TM_ADC_INT_Channel_11_Init(ADCx);
	} else if (ch == TM_ADC_Channel_12) {
		TM_ADC_INT_Channel_12_Init(ADCx);
	} else if (ch == TM_ADC_Channel_13) {
		TM_ADC_INT_Channel_13_Init(ADCx);
	} else if (ch == TM_ADC_Channel_14) {
		TM_ADC_INT_Channel_14_Init(ADCx);
	} else if (ch == TM_ADC_Channel_15) {
		TM_ADC_INT_Channel_15_Init(ADCx);
	}
	
	/* Init ADC */
	TM_ADC_InitADC(ADCx);
}

void TM_ADC_InitADC(ADC_TypeDef* ADCx) {
	/* Enable clock */
#if defined(ADC1)
	if (ADCx == ADC1) {
		__HAL_RCC_ADC1_CLK_ENABLE();
	}
#endif
#if defined(ADC2)
	if (ADCx == ADC2) {
		__HAL_RCC_ADC2_CLK_ENABLE();
	}
#endif
#if defined(ADC3)
	if (ADCx == ADC3) {
		__HAL_RCC_ADC3_CLK_ENABLE();
	}
#endif
	
	/* Configure the ADC peripheral */
	AdcHandle.Instance = ADCx;

	/* Fill settings */
	AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ScanConvMode = DISABLE;
	AdcHandle.Init.ContinuousConvMode = DISABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;
	
#if defined(STM32F0xx)
	AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	AdcHandle.Init.LowPowerAutoWait = DISABLE;
	AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
#else
	AdcHandle.Init.NbrOfDiscConversion = 0;
	AdcHandle.Init.NbrOfConversion = 1;
	AdcHandle.Init.EOCSelection = DISABLE;
	AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
#endif

	/* Init ADC */
	HAL_ADC_Init(&AdcHandle);
}

uint16_t TM_ADC_Read(ADC_TypeDef* ADCx, TM_ADC_Channel_t channel) {
	ADC_ChannelConfTypeDef sConfig;
	
	/* Configure ADC regular channel */  
	sConfig.Channel = (uint8_t) channel;
	sConfig.Rank = 1;
#if defined(STM32F0xx)
	sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
#else
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	sConfig.Offset = 0;
#endif

	/* Set handle */
	AdcHandle.Instance = ADCx;
	
	/* Return zero */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
		return 0;
	}

	/* Start conversion */  
	if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
		return 0;
	}

#if defined(STM32F0xx)
	/* Poll for end */
	if (HAL_ADC_PollForConversion(&AdcHandle, 10) == HAL_OK) {
		/* Get the converted value of regular channel */
		return HAL_ADC_GetValue(&AdcHandle);
	}
#else
	/* Poll for end */
	HAL_ADC_PollForConversion(&AdcHandle, 10);

	/* Check if the continous conversion of regular channel is finished */
	if (HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC_REG) {
		/* Get the converted value of regular channel */
		return HAL_ADC_GetValue(&AdcHandle);
	}
#endif
	
	/* Return zero */
	return 0;
}

#if defined(ADC_CCR_VBATEN) || defined(ADC_CCR_VBATE)
void TM_ADC_EnableVbat(void) {
	/* Enable VBAT */
#if defined(ADC_CCR_VBATEN)
	ADC->CCR |= ADC_CCR_VBATEN;
#else
	ADC->CCR |= ADC_CCR_VBATE;
#endif
}

void TM_ADC_DisableVbat(void) {
	/* Disable VBAT */
#if defined(ADC_CCR_VBATEN)
	ADC->CCR &= ~ADC_CCR_VBATEN;
#else
	ADC->CCR &= ~ADC_CCR_VBATE;
#endif
}

uint16_t TM_ADC_ReadVbat(ADC_TypeDef* ADCx) {
	uint32_t result;
	
	/* Read battery voltage */
	result = TM_ADC_Read(ADCx, (TM_ADC_Channel_t) ADC_CHANNEL_VBAT);
	
	/* Convert to voltage */
	result = result * ADC_VBAT_MULTI * ADC_SUPPLY_VOLTAGE / 0xFFF;
	
	/* Return value in mV */
	return (uint16_t) result;
}
#endif

/* Private functions */
static void TM_ADC_INT_Channel_0_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_0);
}
static void TM_ADC_INT_Channel_1_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_1);
}
static void TM_ADC_INT_Channel_2_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_2);
}
static void TM_ADC_INT_Channel_3_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_3);
}
static void TM_ADC_INT_Channel_4_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_4);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_6);
	}
#endif
}
static void TM_ADC_INT_Channel_5_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_5);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_7);
	}
#endif
}
static void TM_ADC_INT_Channel_6_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_6);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_8);
	}
#endif
}
static void TM_ADC_INT_Channel_7_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_PIN_7);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_9);
	}
#endif
}
static void TM_ADC_INT_Channel_8_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOB, GPIO_PIN_0);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_10);
	}
#endif
}
static void TM_ADC_INT_Channel_9_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOB, GPIO_PIN_1);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_11);
	}
#endif
}
static void TM_ADC_INT_Channel_10_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_PIN_0);
}
static void TM_ADC_INT_Channel_11_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_PIN_1);
}
static void TM_ADC_INT_Channel_12_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_PIN_2);
}
static void TM_ADC_INT_Channel_13_Init(ADC_TypeDef* ADCx) {
	TM_ADC_INT_InitPin(GPIOC, GPIO_PIN_3);
}
static void TM_ADC_INT_Channel_14_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOC, GPIO_PIN_4);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_4);
	}
#endif
}
static void TM_ADC_INT_Channel_15_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1
#if defined(ADC2)
		|| ADCx == ADC2
#endif	
	) {
		TM_ADC_INT_InitPin(GPIOC, GPIO_PIN_5);
	}
#if defined(ADC3) && defined(GPIOF)
	if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_PIN_5);
	}
#endif
}

static void TM_ADC_INT_InitPin(GPIO_TypeDef* GPIOx, uint16_t PinX) {
	/* Enable GPIO pin */
	TM_GPIO_Init(GPIOx, PinX, TM_GPIO_Mode_AN, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Medium);
}
