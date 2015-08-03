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
#include "tm_stm32_rcc.h"

TM_RCC_Result_t TM_RCC_InitSystem(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

#if defined(STM32F7xx) && !defined(DISABLE_CACHE)
	/* Enable I-Cache */
	SCB_EnableICache();
	
	/* Enable D-Cache */
	SCB_EnableDCache();
#endif
	
	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

#if !defined(STM32F0xx)
	/* Set voltage scaling */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
#endif
	
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE;
	
	/* Select proper PLL input clock */
	if (RCC_OSCILLATORTYPE == RCC_OSCILLATORTYPE_HSE) {	
		RCC_OscInitStruct.HSEState = RCC_HSE_ON;
		RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#if defined(STM32F0xx)
		RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
		RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
#endif
	} else {
		RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
		RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
#if defined(STM32F0xx)
		RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
		RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
#endif
	}
	
	/* Set PLL parameters */
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	
#if !defined(STM32F0xx)
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM;
	RCC_OscInitStruct.PLL.PLLN = RCC_PLLN;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ;
#endif
	
#if defined(STM32F446xx)
#if defined(RCC_PLLR)
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR;
#else
	RCC_OscInitStruct.PLL.PLLR = 7;
#endif
#endif
	
	/* Try to init */
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		return TM_RCC_Result_Error;
	}

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F446xx) || defined(STM32F7xx)
	/* Activate the Over-Drive mode */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		return TM_RCC_Result_Error;
	}
#endif

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
#if !defined(STM32F0xx)
	RCC_ClkInitStruct.ClockType |= RCC_CLOCKTYPE_PCLK2;
#endif
	
#if defined(STM32F405xx) || \
	defined(STM32F415xx) || \
	defined(STM32F407xx) || \
	defined(STM32F417xx) || \
	defined(STM32F427xx) || \
	defined(STM32F437xx) || \
	defined(STM32F429xx) || \
	defined(STM32F439xx) || \
	defined(STM32F446xx) || \
	defined(STM32F7xx) 
	
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
#elif defined(STM32F0xx)
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
#else
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
#endif
	
	/* Try to init */
#if defined(STM32F0xx)
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
#else
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK) {
#endif
		return TM_RCC_Result_Error;
	}
	
	/* Return OK */
	return TM_RCC_Result_Ok;
}
