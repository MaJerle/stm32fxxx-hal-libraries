/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Library template 
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
#ifndef TM_TOUCH_TS3510_H
#define TM_TOUCH_TS3510_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_TOUCH_TS3510
 * @brief    Library description here
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
#include "tm_stm32_i2c.h"
#include "tm_stm32_touch.h"

/**
 * @defgroup TM_LIB_Macros
 * @brief    Library defines
 * @{
 */

/* Touch I2C settings */
#ifndef TOUCH_TS3510_I2C
#define TOUCH_TS3510_I2C        I2C1
#define TOUCH_TS3510_I2C_PP     TM_I2C_PinsPack_3
#endif

/* I2C device address */
#define TOUCH_TS3510_I2C_DEV    0x80

/**
 * @}
 */
 
/**
 * @defgroup TM_TOUCH_TS3510_Typedefs
 * @brief    Library Typedefs
 * @{
 */
/* Typedefs here */
/**
 * @}
 */

/**
 * @defgroup TM_TOUCH_TS3510_Functions
 * @brief    Library Functions
 * @{
 */

uint8_t TM_TOUCH_TS3510_Init(TM_TOUCH_t* TS);
uint8_t TM_TOUCH_TS3510_Read(TM_TOUCH_t* TS);

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
