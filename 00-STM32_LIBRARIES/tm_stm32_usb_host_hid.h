/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   USB HOST for HID devices library
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
 * @brief    USB HOST for HID devices library - http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @{
 *
 * This library is high level implementation for USB HOST HID devices. It allows you to read keyboard and mouse data which are connected to USB FS or HS port.
 *
 * @note  Check @ref TM_USB library for configuration settings first!
 *
 * It is built on ST's USB HOST stack and requires all libraries from ST.
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
 - USB Host HID class
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_usb.h"
#include "tm_stm32_usb_host.h"
#include "usbh_hid.h"

/**
 * @defgroup TM_USBH_HID_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup TM_USBH_HID_Typedefs
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
 * @brief  USB HID Host keyboard structure
 */
typedef struct _TM_USBH_HID_Keyboard_t {
	uint8_t Status;           /*!< Keyboard status value */
	union {                   /*!< Union with status flags for special keys */
		struct {
			uint8_t LCtrl:1;  /*!< Set to 1 when left control is detected as pressed */ 
			uint8_t LShift:1; /*!< Set to 1 when left shift is detected as pressed */
			uint8_t LAlt:1;   /*!< Set to 1 when left alt is detected as pressed */
			uint8_t LGUI:1;   /*!< Set to 1 when left GUI is detected as pressed */
			uint8_t RCtrl:1;  /*!< Set to 1 when right control is detected as pressed */ 
			uint8_t RShift:1; /*!< Set to 1 when right shift is detected as pressed */
			uint8_t RAlt:1;   /*!< Set to 1 when right alt is detected as pressed */
			uint8_t RGUI:1;   /*!< Set to 1 when right GUI is detected as pressed */
			uint8_t Ctrl:1;   /*!< Set to 1 if left or right ctrl is pressed */
			uint8_t Shift:1;  /*!< Set to 1 if left or right shift is pressed */
			uint8_t Alt:1;    /*!< Set to 1 if left or right alt is pressed */
			uint8_t GUI:1;    /*!< Set to 1 if left or right GUI is pressed */
		} S;
		uint16_t SpecialVal;  /*!< Value for union */
	} Special;
	uint8_t Keys[6];          /*!< 6 keyboard keys values */
	uint8_t C;                /*!< ASCII code for pressed keyboard */
} TM_USBH_HID_Keyboard_t;

/** 
 * @brief  USB HID Host mouse structure
 */
typedef struct _TM_USBH_HID_Mouse_t {
	uint16_t AbsoluteX; /*!< Absolute X value for mouse */
	uint16_t AbsoluteY; /*!< Absolute Y value for mouse */
	int8_t RelativeX;   /*!< Relative X value for mouse from last check */
	int8_t RelativeY;   /*!< Relative Y value for mouse from last check */
	uint8_t Buttons[3]; /*!< Values for 3 buttons */
} TM_USBH_HID_Mouse_t;

/**
 * @}
 */

/**
 * @defgroup TM_USBH_HID_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes USB HOST HID class for specific USB mode (PORT)
 * @param  USB_Mode: USB mode where HID HOST class will be registered. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_Result_t enumeration
 */
TM_USBH_Result_t TM_USBH_HID_Init(TM_USB_t USB_Mode);

/**
 * @brief  Gets connected device on specific USB port of type HID DEVICE
 * @param  USB_Mode: USB mode where to check for connected HID device. This parameter can be a value of @ref TM_USB_t enumeration
 * @retval Member of @ref TM_USBH_HID_t enumeration
 */
TM_USBH_HID_t TM_USBH_HID_GetConnected(TM_USB_t USB_Mode);

/**
 * @brief  Gets keyboard pressed buttons and ASCII value for pressed button
 * @param  USB_Mode: USB mode where read keyboard data. This parameter can be a value of @ref TM_USB_t enumeration
 * @param  *KeyboardStruct: Pointer to @ref TM_USBH_HID_Keyboard_t where values will be stored
 * @retval Member of @ref TM_USBH_HID_t enumeration. @arg TM_USBH_HID_Keyboard will be returned if data are valid
 */
TM_USBH_HID_t TM_USBH_HID_GetKeyboard(TM_USB_t USB_Mode, TM_USBH_HID_Keyboard_t* KeyboardStruct);

/**
 * @brief  Gets mouse pressed buttons and X/Y cursor positions
 * @param  USB_Mode: USB mode where read mouse data. This parameter can be a value of @ref TM_USB_t enumeration
 * @param  *MouseStruct: Pointer to @ref TM_USBH_HID_Mouse_t where values will be stored
 * @retval Member of @ref TM_USBH_HID_t enumeration. @arg TM_USBH_HID_Mouse will be returned if data are valid
 */
TM_USBH_HID_t TM_USBH_HID_GetMouse(TM_USB_t USB_Mode, TM_USBH_HID_Mouse_t* MouseStruct);

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
