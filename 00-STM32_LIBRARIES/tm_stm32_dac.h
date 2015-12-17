/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com/2015/10/hal-library-28-digital-to-analog-converter-for-stm32fxxx/
 * @link    DAC library for ST32F0xx, STM32F4xx and STM32F7xx
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   DAC library for STM32F4xx and STM32F7xx
 *	
@verbatim
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
