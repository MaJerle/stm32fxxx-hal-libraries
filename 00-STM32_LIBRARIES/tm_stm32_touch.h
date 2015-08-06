/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Touch library for all touch screen controllers
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
#ifndef TM_TOUCH_H
#define TM_TOUCH_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_TOUCH
 * @brief    Touch library for touch screen controllers
 * @{
 *
 * By default, tt supports STM32F7-Discovery and STM32F439-Eval board, but can be easily switched for any touch you have in mind.
 *
 * \par Set driver
 * 
\code
//Use touch driver on STM32F7-Discovery, FT5336 controller
#define TOUCH_USE_STM32F7_DISCOVERY
	
//Use touch driver on STM32F439-Eval board, TS3510 controller
#define TOUCH_USE_STM32F439_EVAL
\endcode
 *
 * When selecting "built-in" drivers, you also have to include some libs.
 * - TM TOUCH TS3510 library for STM32F439-Eval
 * - TM TOUCH FT5336 library for STM32F7-Discovery
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
 - TM TOUCH TS3510
 - TM TOUCH FT5336
 - TM I2C
 - TM GPIO
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"

/**
 * @defgroup TM_TOUCH_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_TOUCH_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Touch working structure
 */
typedef struct {
	uint8_t NumPresses;   /*!< Number of touches (fingers) detected */
	uint16_t X[10];       /*!< X positions for touches */
	uint16_t Y[10];       /*!< Y positions for touches */
	uint8_t Events;       /*!< Events if any */
	uint8_t Orientation;  /*!< Touch orientation to match LCD orientation */
	uint16_t MaxX;        /*!< Touch MAX X value */
	uint16_t MaxY;        /*!< Touch MAX Y value */
} TM_TOUCH_t;

/**
 * @brief  TOUCH Driver structure
 */
typedef struct {
	uint8_t (*Init)(TM_TOUCH_t*); /*!< Pointer to init function for touch controller */
	uint8_t (*Read)(TM_TOUCH_t*); /*!< Pointer to read function for touch controller */
} TM_TOUCH_DRIVER_t;

/**
 * @brief  TOUCH result enumeration
 */
typedef enum {
	TM_TOUCH_Result_Ok = 0x00, /*!< Everything OK */
	TM_TOUCH_Result_Error      /*!< An error occurred */
} TM_TOUCH_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_TOUCH_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes touch controller 
 * @param  *Driver: Pointer to filled @ref TM_TOUCH_DRIVER_t to be used for touch. Set parameter to NULL if you want to use default drivers which lib supports
 * @param  *TS: Pointer to empty @ref TM_TOUCH_t structure
 * @retval Member of @ref TM_TOUCH_Result_t enumeration
 */
TM_TOUCH_Result_t TM_TOUCH_Init(TM_TOUCH_DRIVER_t* Driver, TM_TOUCH_t* TS);

/**
 * @brief  Reads touch data from sensor 
 * @param  *TouchData: Poiter to @ref TM_TOUCH_t structure where data will be stored
 * @retval Touch status:
 *            - 0: OK
 *            - > 0: Error
 */
uint8_t TM_TOUCH_Read(TM_TOUCH_t* TouchData);

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
