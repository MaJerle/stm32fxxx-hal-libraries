/**
 *  Main file which is included from all libraries provided by Tilen Majerle
 * 
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@version 	v1.0
 *	@ide		Keil uVision 5
 *	@license	GNU GPL v3
 *	
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
#ifndef TM_STM32Fxxx_H
#define TM_STM32Fxxx_H

/* Defines for family used with libraries */
/* Uncomment line for your family used with HAL libraries */
//#define STM32F0xx /*!< Use STM32F0xx libraries */
//#define STM32F4xx /*!< Use STM32F4xx libraries */
//#define STM32F7xx /*!< Use STM32F7xx libraries */

/* Check if HAL drivers enabled */
#ifndef USE_HAL_DRIVER
#define USE_HAL_DRIVER
#endif

/* Include proper header file */
/* STM32F7xx */
#if defined(STM32F0xx) || defined(STM32F0XX)
#ifndef STM32F0xx
#define STM32F0xx
#endif
#ifndef STM32F0XX
#define STM32F0XX
#endif
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#endif

/* STM32F4xx */
#if defined(STM32F4xx) || defined(STM32F4XX)
#ifndef STM32F4xx
#define STM32F4xx
#endif
#ifndef STM32F4XX
#define STM32F4XX
#endif
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#endif

/* STM32F7xx */
#if defined(STM32F7xx) || defined(STM32F7XX)
#ifndef STM32F7xx
#define STM32F7xx
#endif
#ifndef STM32F7XX
#define STM32F7XX
#endif
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#endif

/* Check if anything defined */
#if !defined(STM32F0xx) && !defined(STM32F4xx) && !defined(STM32F7xx)
#error "There is not selected STM32 family used. Check stm32fxxx_hal.h file for configuration!"
#endif

/* Init main libraries used everywhere */
#include "tm_stm32_rcc.h"
#include "tm_stm32_gpio.h"

#endif
