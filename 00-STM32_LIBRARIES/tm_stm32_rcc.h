/** 
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/07/hal-library-01-rcc-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   RCC Library for STM32F4xx and STM32F7xx devices
 *
\verbatim
   ----------------------------------------------------------------------
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_RCC_H
#define TM_RCC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
	
/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_RCC
 * @brief    RCC Library for STM32Fxxx - http://stm32f4-discovery.com/2015/07/hal-library-01-rcc-for-stm32fxxx/
 * @{
 *
 * RCC library provides initialization of clock at the beginning. Function @ref TM_RCC_InitSystem should be called at beginning of @ref main function to initialize system.
 *
 * @note  In case of STM32F7xx is used, this library also enabled CACHE for Instructions and Data.
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - Initial release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"

/**
 * @defgroup TM_RCC_Macros
 * @brief    RCC Library macros
 * @{
 */

/* Set default values if not defined by user */
#if !defined(RCC_OSCILLATORTYPE) || !defined(RCC_PLLM) || !defined(RCC_PLLN) || !defined(RCC_PLLP) || !defined(RCC_PLLQ) 
#define RCC_OSCILLATORTYPE    RCC_OSCILLATORTYPE_HSE
#define RCC_PLLM              8
#define RCC_PLLN              360
#define RCC_PLLP              2
#define RCC_PLLQ              7
#endif

/**
 * @}
 */

/**
 * @defgroup TM_RCC_Typedefs
 * @brief    RCC Typedefs used for RCC library for initialization purposes
 * @{
 */

/**
 * @brief  RCC result enumeration
 */
typedef enum {
	TM_RCC_Result_Ok = 0x00, /*!< Everything OK */
	TM_RCC_Result_Error      /*!< An error occurred */
} TM_RCC_Result_t;

/**
 * @} TM_RCC_Typedefs
 */

/**
 * @defgroup TM_RCC_Functions
 * @brief    RCC Functions
 * @{
 */

/**
 * @brief  Initializes system clocks
 * @note   This function should be called on the start of main program
 * @note   When STM32F7xx target is used, D and I caches are also enabled with this function
 * @param  None
 * @retval RCC System status
 */
TM_RCC_Result_t TM_RCC_InitSystem(void);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
