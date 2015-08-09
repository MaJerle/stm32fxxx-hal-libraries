/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB CDC Device library for STM32Fxxx devices
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
#ifndef TM_USBD_CDC_H
#define TM_USBD_CDC_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USBD_CDC
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
 - TM BUFFER
 - TM DELAY
 - TM USB
 - TM USB DEVICE
 - USB Device Stack
 - USB Device CDC
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_buffer.h"
#include "tm_stm32_usb.h"
#include "tm_stm32_usb_device.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "string.h"

/**
 * @defgroup TM_USBD_CDC_Macros
 * @brief    Library defines
 * @{
 */

/* Receive & transmit buffer size for both USB modes */
#ifndef USBD_CDC_BUFFER_SIZE
#define USBD_CDC_BUFFER_SIZE               256
#endif

/* Receive buffer size for FS mode */
#ifndef USBD_CDC_RECEIVE_BUFFER_SIZE_FS
#define USBD_CDC_RECEIVE_BUFFER_SIZE_FS    USBD_CDC_BUFFER_SIZE
#endif
/* Receive buffer size for HS mode */
#ifndef USBD_CDC_RECEIVE_BUFFER_SIZE_HS
#define USBD_CDC_RECEIVE_BUFFER_SIZE_HS    USBD_CDC_BUFFER_SIZE
#endif
/* Transmit buffer size for FS mode */
#ifndef USBD_CDC_TRANSMIT_BUFFER_SIZE_FS
#define USBD_CDC_TRANSMIT_BUFFER_SIZE_FS   USBD_CDC_BUFFER_SIZE
#endif
/* Transmit buffer size for HS mode */
#ifndef USBD_CDC_TRANSMIT_BUFFER_SIZE_HS
#define USBD_CDC_TRANSMIT_BUFFER_SIZE_HS   USBD_CDC_BUFFER_SIZE
#endif
/* Temporary buffer TX size for USB transmissions */
#ifndef USBD_CDC_TMP_TRANSMIT_BUFFER_SIZE
#define USBD_CDC_TMP_TRANSMIT_BUFFER_SIZE  USBD_CDC_BUFFER_SIZE
#endif
/* Temporary buffer RX size for USB transmissions */
#ifndef USBD_CDC_TMP_RECEIVE_BUFFER_SIZE
#define USBD_CDC_TMP_RECEIVE_BUFFER_SIZE   USBD_CDC_BUFFER_SIZE
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_USBD_CDC_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup TM_USBD_CDC_Functions
 * @brief    Library Functions
 * @{
 */
 
/**
 * @brief  Initializes USB DEVICE for CDC class on specific USB mode
 * @param  USB_Mode: USB Mode where CDC DEVICE will be enabled. This parameter can be a value of @ref TM_USB_t enumeration 
 * @retval Member of @ref TM_USBD_Result_t enumeration
 */
TM_USBD_Result_t TM_USBD_CDC_Init(TM_USB_t USB_Mode);

/**
 * @brief  Sends all remaining data from CDC TX buffer to USB out
 * @param  USB_Mode: USB Mode where process will be done. This parameter can be a value of @ref TM_USB_t enumeration 
 * @retval None
 */
void TM_USBD_CDC_Process(TM_USB_t USB_Mode);

/**
 * @brief  Puts string over USB CDC
 * @param  USB_Mode: USB Mode where transmission will be done. This parameter can be a value of @ref TM_USB_t enumeration
 * @param  *str: Pointer to string to be sent over CDC
 * @retval Number of characters added to TX buffer
 */
uint16_t TM_USBD_CDC_Puts(TM_USB_t USB_Mode, const char* str);

/**
 * @brief  Puts character over USB CDC
 * @param  USB_Mode: USB Mode where transmission will be done. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  ch: Character to be sent over CDC
 * @retval Number of characters added to TX buffer
 */
uint16_t TM_USBD_CDC_Putc(TM_USB_t USB_Mode, char ch);

/**
 * @brief  Gets character from USB CDC RX buffer
 * @param  USB_Mode: USB Mode where char will be read. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  *ch: Pointer to character to store value into
 * @retval 1 in case character is read or zero if buffer empty
 */
uint8_t TM_USBD_CDC_Getc(TM_USB_t USB_Mode, char* ch);

/**
 * @brief  Gets string from USB CDC RX buffer
 * @param  USB_Mode: USB Mode where string will be read. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  *buff: Pointer to buffer where string will be saved
 * @param  buffsize: Buffer size in units of bytes
 * @retval Number of elements in string
 */
uint16_t TM_USBD_CDC_Gets(TM_USB_t USB_Mode, char* buff, uint16_t buffsize);

/* Private functions */
void TM_USBD_CDC_INT_AddToBuffer(USBD_HandleTypeDef* pdev, uint8_t* Values, uint16_t Num);

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
