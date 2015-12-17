/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/10/hal-library-29-analog-to-digital-converter-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   ADC library for STM32Fxxx
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
#ifndef TM_ADC_H
#define TM_ADC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_ADC
 * @brief    ADC library for STM32Fxxx - http://stm32f4-discovery.com/2015/10/hal-library-29-analog-to-digital-converter-for-stm32fxxx/
 * @{
 *
 * This library allows you to operate with ADC peripheral on STM32Fxxx devices.
 * It works on STM32F0xx, STM32F4xx and STM32F7xx devices
 *
 * \par How it works
 * 
 * Library uses polling mode of each ADC channel to read data from.
 * It is designed for use where small data rate of ADC is needed,
 * such as reading battery voltage where you need MAX 1 sample per second.
 *
 * \par ADC external pinout
 *
 * ADC supports up to 16 external ADC channels connected to GPIO pins.
 *
\verbatim
CHANNEL   ADC1   ADC2   ADC3

0         PA0    PA0    PA0
1         PA1    PA1    PA1
2         PA2    PA2    PA2
3         PA3    PA3    PA3
4         PA4    PA4    PF6
5         PA5    PA5    PF7
6         PA6    PA6    PF8
7         PA7    PA7    PF9
8         PB0    PB0    PF10
9         PB1    PB1    PF3
10        PC0    PC0    PC0
11        PC1    PC1    PC1
12        PC2    PC2    PC2
13        PC3    PC3    PC3
14        PC4    PC4    PF4
15        PC5    PC5    PF5
\endverbatim
 *
 * \par Reading Battery Voltage
 *
 * Circuit for battery is integrated inside STM32 device.
 * It can measure a battery, connected to VBAT pin, meant for RTC and backup purpose only.
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
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"

/**
 * @defgroup TM_ADC_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  Default supply voltage in mV
 */
#ifndef ADC_SUPPLY_VOLTAGE
#define ADC_SUPPLY_VOLTAGE      3300
#endif

/**
 * @brief  Multipliers for VBAT measurement 
 */
#if defined(STM32F405xx) || defined(STM32F407xx) || defined(STM32F415xx) || defined(STM32F417xx)
#define ADC_VBAT_MULTI			2
#else
#define ADC_VBAT_MULTI			4
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_ADC_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  ADC available channels
 */
typedef enum {
	TM_ADC_Channel_0 = 0x00, /*!< Operate with ADC channel 0 */
	TM_ADC_Channel_1,        /*!< Operate with ADC channel 1 */
	TM_ADC_Channel_2,        /*!< Operate with ADC channel 2 */
	TM_ADC_Channel_3,        /*!< Operate with ADC channel 3 */
	TM_ADC_Channel_4,        /*!< Operate with ADC channel 4 */
	TM_ADC_Channel_5,        /*!< Operate with ADC channel 5 */
	TM_ADC_Channel_6,        /*!< Operate with ADC channel 6 */
	TM_ADC_Channel_7,        /*!< Operate with ADC channel 7 */
	TM_ADC_Channel_8,        /*!< Operate with ADC channel 8 */
	TM_ADC_Channel_9,        /*!< Operate with ADC channel 9 */
	TM_ADC_Channel_10,       /*!< Operate with ADC channel 10 */
	TM_ADC_Channel_11,       /*!< Operate with ADC channel 11 */
	TM_ADC_Channel_12,       /*!< Operate with ADC channel 12 */
	TM_ADC_Channel_13,       /*!< Operate with ADC channel 13 */
	TM_ADC_Channel_14,       /*!< Operate with ADC channel 14 */
	TM_ADC_Channel_15,       /*!< Operate with ADC channel 15 */
	TM_ADC_Channel_16,       /*!< Operate with ADC channel 16 */
	TM_ADC_Channel_17,       /*!< Operate with ADC channel 17 */
	TM_ADC_Channel_18       /*!< Operate with ADC channel 18 */
} TM_ADC_Channel_t;

/**
 * @}
 */

/**
 * @defgroup TM_ADC_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes ADCx peripheral
 * @param  *ADCx: ADCx peripheral to initialize
 * @retval None
 */
void TM_ADC_InitADC(ADC_TypeDef* ADCx);

/**
 * @brief  Initializes ADCx with ADCx channel
 * @param  *ADCx: ADCx peripheral to operate with
 * @param  channel: channel for ADCx. This parameter can be a value of @ref TM_ADC_Channel_t enumeration
 * @retval None
 */
void TM_ADC_Init(ADC_TypeDef* ADCx, TM_ADC_Channel_t channel);

/**
 * @brief  Reads from ADCx channel
 * @param  *ADCx: ADCx peripheral to operate with
 * @param  channel: channel for ADCx to read from. This parameter can be a value of @ref TM_ADC_Channel_t enumeration
 * @retval ADC value
 */
uint16_t TM_ADC_Read(ADC_TypeDef* ADCx, TM_ADC_Channel_t channel);

/**
 * @brief  Enables Vbat channel for ADC
 * @param  None
 * @retval None
 */
void TM_ADC_EnableVbat(void);

/**
 * @brief  Disables Vbat channel for ADC
 * @param  None
 * @retval None
 */
void TM_ADC_DisableVbat(void);

/**
 * @brief  Reads vbat pin voltage
 * @param  *ADCx: ADCx peripheral to use for Vbat measurement
 * @retval Battery voltage in mV
 */
uint16_t TM_ADC_ReadVbat(ADC_TypeDef* ADCx);

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
