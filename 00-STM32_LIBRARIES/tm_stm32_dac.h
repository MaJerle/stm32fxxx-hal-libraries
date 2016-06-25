/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com/2015/10/hal-library-28-digital-to-analog-converter-for-stm32fxxx/
 * @link    DAC library for ST32F0xx, STM32F4xx and STM32F7xx
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   DAC library for STM32F4xx and STM32F7xx
 *	
@verbatim
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
@endverbatim
 */
#ifndef TM_DAC_H
#define TM_DAC_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif
	
/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_DAC
 * @brief    DAC library for STM32F0xx, STM32F4xx and STM32F7xx - http://stm32f4-discovery.com/2015/10/hal-library-28-digital-to-analog-converter-for-stm32fxxx/
 * @{
 *
 * This library provides 12-bit digital to analog output, values from 0 to 4095.
 *
 * It is designed without DMA signal generator, only data user pass to @ref TM_DAC_SetValue are set to output.
 *
 * @note For STM32F4xx and STM32F7xx optimization calls are used to make fast set of channel data.
 *	
 * @par STM pinout
 *
@verbatim
DAC	channel    STM32Fxxx    DESCRIPTION
		
DAC1           PA4          Pin for DAC channel 1
DAC2           PA5          Pin for DAC channel 2
@endverbatim
 *
 * \par Changelog
 *
@verbatim
 Version 1.0
  - First release
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM GPIO
@endverbatim
 */

/**
 * Includes
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"

/* Check if peripheral supported */
#if !defined(DAC)
#error "Target device does not support Digital-To-Analog converter!"
#endif

/**
 * @defgroup TM_DAC_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Select which DAC channel
 */
typedef enum _TM_DAC_Channel_t {
	TM_DAC_Channel_1 = 0x00, /*!< DAC channel 1 */
	TM_DAC_Channel_2 = 0x01  /*!< DAC channel 2 */
} TM_DAC_Channel_t;

/**
 * @}
 */

/**
 * @defgroup TM_DAC_Functions
 * @brief    Library Functions
 * @{
 */
 
/**
 * @brief  Initializes DAC channel and its GPIO pin
 * @param  DACx: DAC Channel you will use. This parameter can be a value of @ref TM_DAC_Channel_t enumeration
 * @retval None
 */
void TM_DAC_Init(TM_DAC_Channel_t DACx);

/**
 * @brief  Sets analog value to ADCx
 * @param  DACx: DAC Channel you will use. This parameter can be a value of @ref TM_DAC_Channel_t enumeration
 * @param  value: 12-bit unsigned value for 12-bit DAC
 * @retval None
 */
void TM_DAC_SetValue(TM_DAC_Channel_t DACx, uint16_t value);

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
