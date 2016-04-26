/** 
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/07/hal-library-01-rcc-for-stm32fxxx/
 * @version v1.1
 * @ide     Keil uVision
 * @license MIT
 * @brief   RCC Library for STM32F4xx and STM32F7xx devices
 *
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_RCC_H
#define TM_RCC_H 110

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
 * @note  In case of STM32F7xx is used, this library also enables CACHE for Instructions and Data.
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - Initial release

 Version 1.1
  - October 10, 2015
  - Added support for STM32F469 devices
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
