/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Brown-Out settings for STM32Fxxx
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
#ifndef TM_BOR_H
#define TM_BOR_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_BOR
 * @brief    Brown-Out settings for STM32Fxxx
 * @{
 *
 * This library allows you to select brown-out level for microcontrollers when device will reset if voltage goes below value set.
 *
 * You can choose between 4 settings:
 *  - Level 0: Disabled brown-out detection, from 1.62 V to 2.1 V
 *  - Level 1: According to the datasheet, from 2.1 to 2.4 V
 *  - Level 2: According to the datasheet, from 2.4 to 2.7 V
 *  - Level 3: According to the datasheet, from 2.7 to 3.6 V
 *
 * @warning It does not work on STM32F0xx series
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
@endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"

/**
 * @defgroup TM_BOR_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_BOR_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Brown-out levels available
 */
typedef enum {
	TM_BOR_Level_None = OB_BOR_OFF, /*!< Disable brown-out detection */
	TM_BOR_Level_1 = OB_BOR_LEVEL1, /*!< Set brown out detection level 1 */
	TM_BOR_Level_2 = OB_BOR_LEVEL2, /*!< Set brown out detection level 2 */
	TM_BOR_Level_3 = OB_BOR_LEVEL3  /*!< Set brown out detection level 3 */
} TM_BOR_Level_t;

/**
 * @brief  Result enumeration for settings
 */
typedef enum {
	TM_BOR_Result_Ok = 0x00, /*!< Everything OK */
	TM_BOR_Result_Error      /*!< An error has occurred */
} TM_BOR_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_BOR_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Sets new brown-out detection level
 * @note   New value will be set only in case current value is different than user wants
 * @param  BORValue: Value level for brown-out. This parameter can be a value of @ref TM_BOR_Level_t enumeration
 * @retval Member of @ref TM_BOR_Result_t
 */
TM_BOR_Result_t TM_BOR_Set(TM_BOR_Level_t BORValue);

/**
 * @brief  Gets current brown-out detection level
 * @param  None
 * @retval Member of @ref TM_BOR_Level_t
 */
TM_BOR_Level_t TM_BOR_Get(void);

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
