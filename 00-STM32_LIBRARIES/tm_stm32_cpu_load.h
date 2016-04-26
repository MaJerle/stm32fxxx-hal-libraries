/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/08/hal-library-19-cpu-load-monitor-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   CPU load monitoring for STM32F4/7xx
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
#ifndef TM_CPU_LOAD_H
#define TM_CPU_LOAD_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_CPULOAD
 * @brief    CPU load monitoring for STM32F4/7xx - http://stm32f4-discovery.com/2015/08/hal-library-19-cpu-load-monitor-for-stm32fxxx/
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
 - TM GENERAL
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_general.h"

/**
 * @defgroup TM_CPULOAD_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_CPULOAD_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  CPU LOAD structure
 */
typedef struct {
	float Load;      /*!< CPU load percentage */
	uint8_t Updated; /*!< Set to 1 when new CPU load is calculated */
	uint32_t WCNT;   /*!< Number of working cycles in one period. Meant for private use */
	uint32_t SCNT;   /*!< Number of sleeping cycles in one period. Meant for private use */
} TM_CPULOAD_t;

/**
 * @}
 */

/**
 * @defgroup TM_CPULOAD_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes DWT functionality for sleep counter and prepares everything for CPU LOAD measurement
 * @param  *CPU_Load: Pointer to empty @ref TM_CPULOAD_t structure
 * @retval Returns DWT counter status:
 *           - 0: DWT counter has not started, measurement will fail
 *           - > 0: DWT counter has started, everything OK
 */
uint8_t TM_CPULOAD_Init(TM_CPULOAD_t* CPU_Load);

/** 
 * @brief  Goes to low power mode and measures sleeping time and working time using DWT cycle counter
 * @note   You need wake up source, otherwise you might stay forever inside low power mode.
 *            For wakeup source, you can use any interrupt, because __WFI() instruction is used.
 * @param  *CPU_Load: Pointer to @ref TM_CPULOAD_t structure 
 * @retval CPU load value updated status.
 *           - 0: CPU load value is not updated, still old results
 *           - > 0: CPU load value is updated
 */
uint8_t TM_CPULOAD_GoToSleepMode(TM_CPULOAD_t* CPU_Load);

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
