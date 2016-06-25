/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2014/07/library-20-independent-watchdog-timer-on-stm32f4xx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Independent watchdog timer for STM32Fxxx MCUs
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
#ifndef TM_IWDG_H
#define TM_IWDG_H 100
/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_IWDG
 * @brief    Independent Watchdog timer for STM32Fxxx - http://stm32f4-discovery.com/2014/07/library-20-independent-watchdog-timer-on-stm32f4xx/
 * @{
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
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"

/**
 * @defgroup TM_IWDG_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief Watchdog timeout settings
 */
typedef enum {
	TM_IWDG_Timeout_5ms = 0x00,   /*!< System reset called every 5ms */
	TM_IWDG_Timeout_10ms = 0x01,  /*!< System reset called every 10ms */
	TM_IWDG_Timeout_15ms = 0x02,  /*!< System reset called every 15ms */
	TM_IWDG_Timeout_30ms = 0x03,  /*!< System reset called every 30ms */
	TM_IWDG_Timeout_60ms = 0x04,  /*!< System reset called every 60ms */
	TM_IWDG_Timeout_120ms = 0x05, /*!< System reset called every 120ms */
	TM_IWDG_Timeout_250ms = 0x06, /*!< System reset called every 250ms */
	TM_IWDG_Timeout_500ms = 0x07, /*!< System reset called every 500ms */
	TM_IWDG_Timeout_1s = 0x08,    /*!< System reset called every 1s */
	TM_IWDG_Timeout_2s = 0x09,    /*!< System reset called every 2s */
	TM_IWDG_Timeout_4s = 0x0A,    /*!< System reset called every 4s */
	TM_IWDG_Timeout_8s = 0x0B,    /*!< System reset called every 8s */
	TM_IWDG_Timeout_16s = 0x0C,   /*!< System reset called every 16s */
	TM_IWDG_Timeout_32s = 0x0D    /*!< System reset called every 32s. This is maximum value allowed with IWDG timer */
} TM_IWDG_Timeout_t;

/**
 * @}
 */

/**
 * @defgroup TM_IWDG_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initialize Watchdog timer
 * @note   After you initialize it, you can't disable it unless reset occur.
 * @param  timeout. Timeout value when watchdog reset happen if not reset by user.
 *            This parameter can be a value of @ref TM_IWDG_Timeout_t enumeration
 * @retval Value if system was reset because of watchdog timer
 *            - 1: Reset happen because of watchdog
 *            - 0: Otherwise
 */
uint8_t TM_IWDG_Init(TM_IWDG_Timeout_t timeout);

/**
 * @brief  Reset IWDG timer
 * @note   This function have to be called periodically to reset watchdog timer, or your system will reset
 * @param  None
 * @retval None
 * @note   Defined as macro for faster execution
 */
#define TM_IWDG_Reset()     (IWDG->KR = 0xAAAA)

/**
 * @}
 */
 
/**
 * @}
 */

/**
 * @}
 */

#endif

