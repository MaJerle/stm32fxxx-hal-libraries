/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB Device library for STM32Fxxx devices
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
#ifndef TM_USBD_LIB_H
#define TM_USBD_LIB_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USBD
 * @brief    USB Device library for STM32Fxxx devices
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
 - TM USB
 - USB Device Stack
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_usb.h"
#include "usbd_def.h"
#include "usbd_core.h"

/* External variables */
#ifdef USB_USE_FS
extern USBD_HandleTypeDef hUSBDevice_FS;
#endif
#ifdef USB_USE_HS
extern USBD_HandleTypeDef hUSBDevice_HS;
#endif

/**
 * @defgroup TM_USBD_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_USBD_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  USB Device result enumeration
 */
typedef enum {
	TM_USBD_Result_Ok = 0x00, /*!< Everything OK */
	TM_USBD_Result_Error      /*!< An error has occurred */
} TM_USBD_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_USBD_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes USB Device for specific mode
 * @param  USB_Mode: USB Mode to be initialized. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_Init(TM_USB_t USB_Mode);

/**
 * @brief  Starts USB Device for specific mode
 * @param  USB_Mode: USB Mode to be started. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_Start(TM_USB_t USB_Mode);

/**
 * @brief  Restarts USB Device for specific mode
 * @param  USB_Mode: USB Mode to be restarted. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_Restart(TM_USB_t USB_Mode);

/**
 * @brief  Initializes USB Device for specific mode
 * @param  USB_Mode: USB Mode to be initialized. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_Stop(TM_USB_t USB_Mode);

/**
 * @brief  Gets pointer to USB handle
 * @note   This function is not meant for public use
 * @retval Pointer to @ref USBD_HandleTypeDef structure
 */
USBD_HandleTypeDef* TM_USBD_GetUSBPointer(TM_USB_t USB_Mode);

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
