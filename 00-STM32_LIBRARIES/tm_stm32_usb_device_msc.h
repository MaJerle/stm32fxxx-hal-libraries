/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB MSC Device library for STM32Fxxx devices
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
#ifndef TM_USBD_MSC_H
#define TM_USBD_MSC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USBD_MSC
 * @brief    USB MSC Device library for STM32Fxxx devices - http://stm32f4-discovery.com/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
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
 - TM BUFFER
 - TM DELAY
 - TM USB
 - TM USB DEVICE
 - USB Device Stack
 - USB Device MSC
 - TM FATFS with SDCARD SDIO driver
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usb.h"
#include "tm_stm32_usb_device.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "usbd_msc_storage.h"
#include "string.h"

/**
 * @defgroup TM_USBD_MSC_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup TM_USBD_MSC_Functions
 * @brief    Library Functions
 * @{
 */
 
/**
 * @brief  Initializes USB DEVICE for MSC class on specific USB mode
 * @param  USB_Mode: USB Mode where MSC DEVICE will be enabled. This parameter can be a value of @ref TM_USB_t enumeration 
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_MSC_Init(TM_USB_t USB_Mode);

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
