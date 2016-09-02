/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   USB CDC Device library for STM32Fxxx devices
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
 * @brief    USB CDC Device library for STM32Fxxx devices - http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @{
 *
 * This library is designed to allow STM32Fxxx device to acts like CDC device to your computer (Virtual COM port).
 *
 * @note  Check @ref TM_USB library for configuration settings first!
 *
 * \par Main features
 *
\verbatim
- Works on USB FS or HS mode
- Can receive/transmit single character, string or custom array
- Buffer sizes can be selected for user needs
\endverbatim
 *
 * \par Read terminal settings
 *
 * Using this library, you can read settings set by user from terminal window on computer if needed.
 * Check @ref TM_USBD_CDC_GetSettings function and @ref TM_USBD_CDC_Settings_t structure for more information.
 *
 * \par USBD CDC configuration
 *
 * Each user can use this library for its own needs. For that purpose, some configurations can be done to maximize use of library.
 * In this function, user can "only" set settings regarding USB CDC TX and RX ring buffers used for CDC application.
 * 
 * Use defines.h file for copy/paste settings you need and change it.
 *
\code
//Receive & transmit buffers size for both USB modes
//This setting is by default the value for all other settings used in library if not changed
//Entire RAM usage if both modes enabled will be 6 * USBD_CDC_BUFFER_SIZE if default settings used
#define USBD_CDC_BUFFER_SIZE               256

//Receive buffer size for FS mode
//Number of bytes for receive on FS USB port if FS mode is enabled in TM_USB library
//Set this to large value if you will use FS mode with a lot of receive data from USB CDC
#define USBD_CDC_RECEIVE_BUFFER_SIZE_FS    USBD_CDC_BUFFER_SIZE

//Receive buffer size for HS mode
//Number of bytes for receive on HS USB port if HS mode is enabled in TM_USB library
//Set this to large value if you will use HS mode with a lot of receive data from USB CDC
#define USBD_CDC_RECEIVE_BUFFER_SIZE_HS    USBD_CDC_BUFFER_SIZE

//Transmit buffer size for FS mode
//Number of bytes for transmit on FS USB port if FS mode is enabled in TM_USB library
//Set this to large value if you will use FS mode with a lot of transmit data to USB CDC
#define USBD_CDC_TRANSMIT_BUFFER_SIZE_FS   USBD_CDC_BUFFER_SIZE

//Transmit buffer size for HS mode
//Number of bytes for transmit on HS USB port if HS mode is enabled in TM_USB library
//Set this to large value if you will use HS mode with a lot of transmit data to USB CDC
#define USBD_CDC_TRANSMIT_BUFFER_SIZE_HS   USBD_CDC_BUFFER_SIZE

//Temporary buffer TX size for USB transmissions
//Used for temporary storage data for USB transmission
//Set to large value if a lot of data will be transmitted from device to USB CDC
#define USBD_CDC_TMP_TRANSMIT_BUFFER_SIZE  USBD_CDC_BUFFER_SIZE

//Temporary buffer RX size for USB transmissions
//Used for temporary storage data for USB transmission
//Set to large value if a lot of data will be received from USB CDC to device
#define USBD_CDC_TMP_RECEIVE_BUFFER_SIZE   USBD_CDC_BUFFER_SIZE
\endcode
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
 * @brief  Settings structure for USB CDC
 */
typedef struct {
	uint32_t Baudrate; /*!< Baudrate, which is set by user on terminal. 
							Value is number of bits per second, for example: 115200 */
	uint8_t Stopbits;  /*!< Stop bits, which is set by user on terminal.
							Possible values:
							   - 0: 1 stop bit
							   - 1: 1.5 stop bits -> This value is not supported on UART on STM32 in case you want to make USB <-> UART converter
							   - 2: 2 stop bits */
	uint8_t DataBits;  /*!< Data bits, which is set by user on terminal.
							Possible values:
							   - 5: 5 data bits
							   - 6: 6 data bits
							   - 7: 7 data bits
							   - 8: 8 data bits
							   - 9: 9 data bits */
	uint8_t Parity;    /*!< Parity, which is set by user on terminal.
							Possible values:
							   - 0: No parity
							   - 1: Odd parity
							   - 2: Even parity
							   - 3: Mark parity
							   - 4: Space parity */
	uint8_t Updated;   /*!< When you check for settings in my function, 
							this will be set to 1 if user has changed parameters,
							so you can reinitialize USART peripheral if you need to. */
} TM_USBD_CDC_Settings_t;

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
 * @brief  Puts character over USB CDC
 * @param  USB_Mode: USB Mode where transmission will be done. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  ch: Character to be sent over CDC
 * @retval Number of characters added to TX buffer
 */
uint16_t TM_USBD_CDC_Putc(TM_USB_t USB_Mode, char ch);

/**
 * @brief  Puts string over USB CDC
 * @param  USB_Mode: USB Mode where transmission will be done. This parameter can be a value of @ref TM_USB_t enumeration
 * @param  *str: Pointer to string to be sent over CDC
 * @retval Number of characters added to TX buffer
 */
uint16_t TM_USBD_CDC_Puts(TM_USB_t USB_Mode, const char* str);

/**
 * @brief  Puts array of data from USB CDC RX buffer
 * @param  USB_Mode: USB Mode where string will be read. This parameter can be a value of @ref TM_USB_t enumeration
 * @param  *buff: Pointer to data to be sent over USB CDC
 * @param  count: Number of elements to send
 * @retval Number of elements written to TX buffer for CDC
 */
uint16_t TM_USBD_CDC_PutArray(TM_USB_t USB_Mode, uint8_t* buff, uint16_t count);

/**
 * @brief  Gets character from USB CDC RX buffer
 * @param  USB_Mode: USB Mode where char will be read. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  *ch: Pointer to character to store value into
 * @retval 1 in case character is read or zero if buffer empty
 */
uint8_t TM_USBD_CDC_Getc(TM_USB_t USB_Mode, char* ch);

/**
 * @brief  Gets string from USB CDC RX buffer
 * @note   Check @ref TM_BUFFER library for more info on how strings are returned
 * @param  USB_Mode: USB Mode where string will be read. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  *buff: Pointer to buffer where string will be saved
 * @param  buffsize: Buffer size in units of bytes
 * @retval Number of elements in string
 */
uint16_t TM_USBD_CDC_Gets(TM_USB_t USB_Mode, char* buff, uint16_t buffsize);

/**
 * @brief  Gets array of data from USB CDC RX buffer
 * @param  USB_Mode: USB Mode where string will be read. This parameter can be a value of @ref TM_USB_t enumeration
 * @param  *buff: Pointer to buffer where array will be stored
 * @param  count: Number of elements to read
 * @retval Number of elements read from buffer
 */
uint16_t TM_USBD_CDC_GetArray(TM_USB_t USB_Mode, uint8_t* buff, uint16_t count);

/**
 * @brief  Reads current settings set from user terminal
 * @param  USB_Mode: USB mode where to read settings. This parameter can be a value of @ref TM_USB_t enumeration 
 * @param  *Settings: Pointer to @ref TM_USBD_CDC_Settings_t struture to fill data into
 * @retval None
 */
void TM_USBD_CDC_GetSettings(TM_USB_t USB_Mode, TM_USBD_CDC_Settings_t* Settings);

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
