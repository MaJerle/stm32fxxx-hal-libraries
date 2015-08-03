/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB HOST for HID devices library
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
#ifndef TM_USBH_HID_H
#define TM_USBH_HID_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USBH_HID
 * @brief    USB HOST for HID devices library
 * @{
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
 - TM USB
 - TM USB HOST
 - USB Host Stack
 - USB Host HID class
@endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_usb.h"
#include "tm_stm32_usb_host.h"
#include "usbh_hid.h"

/**
 * @defgroup TM_LIB_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_LIB_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  USB host HID types
 */
typedef enum _TM_USBH_HID_t {
	TM_USBH_HID_None = 0x00, /*!< No connected device */
	TM_USBH_HID_Keyboard,    /*!< Connected device is keyboard */
	TM_USBH_HID_Mouse        /*!< Connected device is mouse */
} TM_USBH_HID_t;

/**
 * @}
 */

/**
 * @defgroup TM_LIB_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes USB HOST HID class for specific USB mode (PORT)
 * @param  USB_Mode: USB mode where HID HOST class will be registered. This parameter can be a value of @ref TM_USBH_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_HID_Init(TM_USBH_t USB_Mode);

/**
 * @brief  Gets connected device on specific USB port of type HID DEVICE
 * @param  USB_Mode: USB mode where to check for connected HID device. This parameter can be a value of @ref TM_USBH_t enumeration
 * @retval Member of @ref TM_USBH_HID_t enumeration
 */
TM_USBH_HID_t TM_USBH_HID_GetConnected(TM_USBH_t USB_Mode);

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
