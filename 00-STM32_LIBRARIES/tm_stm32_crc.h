/**
 * @author  Tilen MAJERLE
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-10-crc-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   CRC for STM32Fxxx devices
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
#ifndef TM_CRC_H
#define TM_CRC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_CRC
 * @brief    CRC library for STM32Fxxx devices - http://stm32f4-discovery.net/2015/07/hal-library-10-crc-for-stm32fxxx/
 * @{
 *
 * Library uses hardware CRC unit in STM32Fxxx device
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
 * @defgroup TM_CRC_Functions
 * @brief    Library Functions
 * @{
 */

/** 
 * @brief  Initializes and enables CRC peripheral
 * @param  None
 * @retval None
 */
void TM_CRC_Init(void);

/** 
 * @brief  De initializes and disable CRC peripheral
 * @param  None
 * @retval None
 */
void TM_CRC_DeInit(void);

/**
 * @brief  Calculates 32-bit CRC value from 8-bit input array
 * @param  *arr: Pointer to 8-bit data array for CRC calculation
 * @param  count: Number of elements in array for calculation
 * @param  reset: Reset CRC peripheral to 0 state before starting new calculations
 *            - 0: CRC unit will not be reset before new calculations will happen and will use previous data to continue
 *            - > 0: CRC unit is set to 0 before first calculation
 * @retval 32-bit CRC calculated number
 */
uint32_t TM_CRC_Calculate8(uint8_t* arr, uint32_t count, uint8_t reset);

/**
 * @brief  Calculates 32-bit CRC value from 16-bit input array
 * @param  *arr: Pointer to 16-bit data array for CRC calculation
 * @param  count: Number of elements in array for calculation
 * @param  reset: Reset CRC peripheral to 0 state before starting new calculations
 *            - 0: CRC unit will not be reset before new calculations will happen and will use previous data to continue
 *            - > 0: CRC unit is set to 0 before first calculation
 * @retval 32-bit CRC calculated number
 */
uint32_t TM_CRC_Calculate16(uint16_t* arr, uint32_t count, uint8_t reset);

/**
 * @brief  Calculates 32-bit CRC value from 32-bit input array
 * @param  *arr: Pointer to 32-bit data array for CRC calculation
 * @param  count: Number of elements in array for calculation
 * @param  reset: Reset CRC peripheral to 0 state before starting new calculations
 *            - 0: CRC unit will not be reset before new calculations will happen and will use previous data to continue
 *            - > 0: CRC unit is set to 0 before first calculation
 * @retval 32-bit CRC calculated number
 */
uint32_t TM_CRC_Calculate32(uint32_t* arr, uint32_t count, uint8_t reset);

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
