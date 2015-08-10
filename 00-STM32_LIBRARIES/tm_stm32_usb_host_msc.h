/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB HOST for MSC devices library
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
#ifndef TM_USBH_MSC_H
#define TM_USBH_MSC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USBH_MSC
 * @brief    USB HOST MSC for storage devices like USB flash keys - http://stm32f4-discovery.com/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @{
 *
 * This library is high level library for checking for MSC connected devices and if they are ready.
 *
 * @note  Check @ref TM_USB library for configuration settings first!
 *
 * For writing/reading from devices, you will also need @ref TM_FATFS library with driver for USB FS and/or HS modes.
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
 - TM USB HOST
 - USB Host Stack
 - USB Host MSC class
 - TM FATFS
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_usb.h"
#include "tm_stm32_usb_host.h"
#include "usbh_msc.h"
#include "tm_stm32_fatfs.h"

/**
 * @defgroup TM_USBH_MSC_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_USBH_MSC_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup TM_USBH_MSC_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes USB HOST MSC class for specific USB mode (PORT)
 * @param  USB_Mode: USB mode where MSC HOST class will be registered. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_MSC_Init(TM_USB_t USB_Mode);

/**
 * @brief  Checks if connecteed device on USB mode is type of MSC class
 * @param  USB_Mode: USB mode where device will be checked. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_MSC_IsConnected(TM_USB_t USB_Mode);

/**
 * @brief  Checks if connecteed MSC device on USB mode is ready to be used with FATFS
 * @param  USB_Mode: USB mode where device will be checked. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_MSC_IsReady(TM_USB_t USB_Mode);

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
