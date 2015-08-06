/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB Host library for STM32Fxxx devices
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
#ifndef TM_USBH_H
#define TM_USBH_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USBH
 * @brief    USB Host library for STM32Fxxx devices
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
 - USB Host stack
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_usb.h"
#include "usbh_core.h"

/* External variables */
extern USBH_HandleTypeDef hUSBHost_FS;
extern USBH_HandleTypeDef hUSBHost_HS;

/**
 * @defgroup TM_USBH_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_USB_typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  USB Host result enumeration
 */
typedef enum {
	TM_USBH_Result_Ok = 0x00, /*!< Everything OK */
	TM_USBH_Result_Error      /*!< An error has occurred */
} TM_USBH_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_USBH_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes USB Host for specific mode
 * @param  USB_Mode: USB Mode to be initialized. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_Init(TM_USB_t USB_Mode);

/**
 * @brief  Starts USB Host for specific mode
 * @param  USB_Mode: USB Mode to be started. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_Start(TM_USB_t USB_Mode);

/**
 * @brief  Restarts USB Host for specific mode
 * @param  USB_Mode: USB Mode to be restarted. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_Restart(TM_USB_t USB_Mode);

/**
 * @brief  Initializes USB Host for specific mode
 * @param  USB_Mode: USB Mode to be initialized. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_Stop(TM_USB_t USB_Mode);

/**
 * @brief  Processes USB Host for specific mode
 * @note   This function HAVE TO be called periodically or USB HOST won't work!
 * @param  USB_Mode: USB Mode to be processed. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_Process(TM_USB_t USB_Mode);

/**
 * @brief  Checks if USB device is connected on speficic USB mode
 * @param  USB_Mode: USB Mode to be checked for connected device. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_IsConnected(TM_USB_t USB_Mode);

/**
 * @brief  Gets Vendor ID (Allocated by USB association) of connected USB device on specific USB PORT
 * @param  USB_Mode: USB Mode where VID will be checked. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Vendor ID number
 */
uint16_t TM_USBH_GetVID(TM_USB_t USB_Mode);

/**
 * @brief  Gets Product ID of connected USB device on specific USB PORT
 * @param  USB_Mode: USB Mode where VID will be checked. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Product ID number
 */
uint16_t TM_USBH_GetPID(TM_USB_t USB_Mode);

/**
 * @brief  Gets pointer to USB handle
 * @note   This function is not meant for public use
 * @retval Pointer to @ref USBH_HandleTypeDef structure
 */
USBH_HandleTypeDef* TM_USBH_GetUSBPointer(TM_USB_t USB_Mode);

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
