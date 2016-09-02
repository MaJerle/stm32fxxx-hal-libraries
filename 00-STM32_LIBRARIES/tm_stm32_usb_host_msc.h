/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   USB HOST for MSC devices library
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
 * @brief    USB HOST MSC for storage devices like USB flash keys - http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
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
