/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   USB Device library for STM32Fxxx devices
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
#ifndef TM_USBD_H
#define TM_USBD_H 100

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
 * @brief    USB Device library for STM32Fxxx devices - http://stm32f4-discovery.com/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @{
 *
 * This is main USB DEVICE library, whos job is to control device. It can:
 *
\verbatim
- Start USB as device
- Stop USB as device
- Restart USB as device
- Control USB FS and HS modes
\endverbatim
 *
 * @note  All DEVICE related libraries (like @ref TM_USBD_CDC) includes this library!
 *
 * @note  Check @ref TM_USB library for configuration settings first!
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
 - USB Stack
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
 * @brief  Checks if connected device to HOST mode is configured and ready to use
 * @param  USB_Mode: USB port where to check for ready status. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_IsDeviceReady(TM_USB_t USB_Mode);

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
