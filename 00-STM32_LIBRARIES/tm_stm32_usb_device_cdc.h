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
 * @defgroup TM_USBD_Macros
 * @brief    Library defines
 * @{
 */

/* Receive buffer size */
#ifndef USBD_CDC_BUFFER_SIZE
#define USBD_CDC_BUFFER_SIZE              32
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


/**
 * @}
 */
 
/**
 * @defgroup TM_USBD_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup TM_USBD_Functions
 * @brief    Library Functions
 * @{
 */

TM_USBD_Result_t TM_USBD_CDC_Init(TM_USB_t USB_Mode);
uint16_t TM_USBD_CDC_Puts(TM_USB_t USB_Mode, const char* str);
uint16_t TM_USBD_CDC_Putc(TM_USB_t USB_Mode, char ch);


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
