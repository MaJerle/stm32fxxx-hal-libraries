/**
 * @author  Tilen MAJERLE
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/09/hal-library-26-power-voltage-detector-pvd-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   PVD Voltage detector for STM32Fxxx
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen MAJERLE

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
#ifndef TM_PVD_H
#define TM_PVD_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_PVD
 * @brief    PVD - Power Voltage Detector for STM32Fxxx - http://stm32f4-discovery.net/2015/09/hal-library-26-power-voltage-detector-pvd-for-stm32fxxx/
 * @{
 *
 * PVD can detect voltage changes in your system. It features 8 different voltage levels between 2 and 3 volts. For detailed voltages, check device datasheet.
 *
 * Library supports STM32F4xx, STM32F7xx and STM32F0xx devices. For F0xx series, not all devices are supported!
 *
 * \par Change configuration
 *
 * PVD uses interrupts for voltage detection. For this purpose, NVIC configuration must be used.
 * I made default NVIC configuration, but if you need to change it, defines.h file is for you.
 *
\code
//NVIC preemption priority
#define PVD_NVIC_PRIORITY      0x04

//NVIC Subpriority
#define PVD_NVIC_SUBPRIORITY   0x00
\endcode
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - attributes.h
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "attributes.h"

/**
 * @defgroup TM_PVD_Macros
 * @brief    Library defines
 * @{
 */

/* NVIC preemption priority */
#ifndef PVD_NVIC_PRIORITY
#define PVD_NVIC_PRIORITY      0x04
#endif

/* NVIC subpriority */
#ifndef PVD_NVIC_SUBPRIORITY
#define PVD_NVIC_SUBPRIORITY   0x00
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_PVD_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  PVD Triggers for interrupt
 */
typedef enum {
	TM_PVD_Trigger_Rising = PWR_PVD_MODE_IT_RISING,                /*!< PVD will trigger interrupt when voltage rises above treshold */
	TM_PVD_Trigger_Falling = PWR_PVD_MODE_IT_FALLING,              /*!< PVD will trigger interrupt when voltage falls below treshold */
	TM_PVD_Trigger_Rising_Falling = PWR_PVD_MODE_IT_RISING_FALLING /*!< PVD will trigger interrupt when voltage rises or falls above/below treshold */
} TM_PVD_Trigger_t;

/**
 * @brief  PVD levels for interrupt triggering
 * @note   Check datasheets for proper values for voltages
 */
typedef enum {
	TM_PVD_Level_0 = PWR_PVDLEVEL_0, /*!< PVD level 0 is used as treshold value */
	TM_PVD_Level_1 = PWR_PVDLEVEL_1, /*!< PVD level 1 is used as treshold value */
	TM_PVD_Level_2 = PWR_PVDLEVEL_2, /*!< PVD level 2 is used as treshold value */
	TM_PVD_Level_3 = PWR_PVDLEVEL_3, /*!< PVD level 3 is used as treshold value */
	TM_PVD_Level_4 = PWR_PVDLEVEL_4, /*!< PVD level 4 is used as treshold value */
	TM_PVD_Level_5 = PWR_PVDLEVEL_5, /*!< PVD level 5 is used as treshold value */
	TM_PVD_Level_6 = PWR_PVDLEVEL_6, /*!< PVD level 6 is used as treshold value */
	TM_PVD_Level_7 = PWR_PVDLEVEL_7  /*!< PVD level 7 is used as treshold value */
} TM_PVD_Level_t;

/**
 * @}
 */

/**
 * @defgroup TM_PVD_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Enables PVD feature for interrupt generation when voltage changes are detected
 * @param  Level: Level voltage when interrupt will happen. This parameter can be a value of @ref TM_PVD_Level_t enumeration
 * @param  Trigger: Trigger option when interrupt will be generated. This parameter can be a value of @ref TM_PVD_Trigger_t enumeration
 * @retval None
 */
void TM_PVD_Enable(TM_PVD_Level_t Level, TM_PVD_Trigger_t Trigger);

/**
 * @brief  Disables PVD feature, disables NVIC and EXTI interrupt
 * @param  None
 * @retval None
 */
void TM_PVD_Disable(void);

/**
 * @brief  Callback for handling PVD interrupts. This function is automatically called from interrupt service routine when needed.
 * @param  status: Status about voltage detected
 *            - 0: Voltage is above treshold level
 *            - > 0: Voltage is below treshold value
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_PVD_Handler(uint8_t status);

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
