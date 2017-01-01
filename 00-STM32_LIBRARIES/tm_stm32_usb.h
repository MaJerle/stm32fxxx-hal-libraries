/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   USB library for STM32Fxxx devices
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
#ifndef TM_USB_H
#define TM_USB_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_USB
 * @brief    USB library for STM32Fxxx devices - http://stm32f4-discovery.net/2015/08/hal-library-21-multi-purpose-usb-library-for-stm32fxxx/
 * @{
 *
 * This library is main library for all my other USB implementations.
 * Its main purpose is to handle all common settings, like initializing GPIO pins and handling interrupts
 * 
 * @note  None of these functions should be directly called from user. They are called from my other libraries like @ref TM_USB_HOST and @ref TM_USB_DEVICE implementations. 
 *
 * \par Library features
 *
 * This library is used to set some settings for your USB work when needed. These are:
 *
\verbatim
- Enabling USB pins when needed
- Setting up configuration for specific board used for USB
- Driving VBUS when device acts like HOST
- Handling USB interrupts
- Library supports different USB configurations:
  - Both FS and HS ports defined as HOST mode
  - Both FS and HS ports defined as DEVICE mode
  - FS port device, HS port HOST
  - HS port device, FS port HOST
\endverbatim
 *
 * \par Setting up main configurations
 *
 * By default, you will have to enable some configurations in order to use USB properly.
 * This can be done in defines.h project settings. Open file, copy/edit things you need.
 *
\code
//Use this define, if you want to enable USB functionality for USB FS (Full speed) mode in your project
//By default it is disabled to save some RAM
#define USB_USE_FS

//Use this define, if you want to enable USB functionality for USB HS (High speed) mode in your project
//By default it is disabled to save some RAM
#define USB_USE_HS

//Use this define, if you are using external PHY chip for USB on HS port
//If you don't use it, then HS mode will be USB HS in FS (High Speed in Full speed) mode
//This mode is automatically enabled for HS mode if using STM32F7-Discovery board
#define USB_USE_ULPI_PHY

//Use this define if you will use FS or HS USB mode in HOST mode
#define USB_USE_HOST

//Use this define if you will use FS or HS USB mode in DEVICE mode
#define USB_USE_DEVICE
\endcode
 *
 * For example, you want to use FS port for USB HID host and HS port for USB CDC device. You will need at least these defines:
\code
//Enable FS mode functionality
#define USB_USE_FS

//Enable HS mode functionality
#define USB_USE_HS

//Enable HOST mode functionality
#define USB_USE_HOST

//Enable DEVICE mode functionality
#define USB_USE_DEVICE
\endcode
 *
 * \par USB FS mode settings
 * 
 * When using USB FS mode, STM32Fxxx needs 48MHz clock, provided from PLL, so you have to make sure that your PLL clock is set correct.
 * <code>PLL_M</code> parameter should be the same as value of external crystal frequency in MHz. For example, if HSE_VALUE is 25000000, then PLL_M should be set to 25
 *
 * \par USB HS mode settings
 *
 * For USB HS mode with external ULPI PHY, you don't need any specific clock settings, because ULPI will do this for you.
 * However, if you are using HS in FS mode, you have to setup 48MHz clock the same as when using FS mode.
 *
 * \par STM32F4/7xx boards and configurations
 *
 * @note  If you follow my description in @ref TM_DISCO library on how to select your used board, then this library will automatically select your "target".
 *
 * You will need to set bottom defines in case you don't use my @ref TM_DISCO library configuration for board used.
 * 
\code
//Enable default USB configurations for STM32F7-Discovery
#define USB_USE_STM32F7_DISCOVERY

//Enable default USB configurations for STM32F429-Discovery
#define USB_USE_STM32F29_DISCOVERY

//Enable default USB configurations for STM32F4-Discovery
#define USB_USE_STM32F4_DISCOVERY
\endcode
 *
 * \par STM32F4-Discovery pinout
 *
 * Micro USB connector is on USB FS mode
 *
\verbatim
PA12 <-> Data +
PA11 <-> Data -
PA10 <-> ID    
PA9  <-> VBUS  
PC0  <-> VBUS_EN
\endverbatim
 *
 * \par STM32F429-Discovery pinout
 *
 * Micro USB connector is on USB HS in FS mode, because there is no external PHY for real USB HS mode
 *
\verbatim
PB15 <-> Data +
PB14 <-> Data -
PB12 <-> ID    
PB13 <-> VBUS  
PC4  <-> VBUS_EN
\endverbatim
 *
 * \par STM32F7-Discovery pinout
 *
 * Micro USB connectors are on USB FS mode and on real USB HS mode with external PHY chip
 *
\verbatim
USB FS mode      | USB HS mode
                 |
PA12 <-> Data +  | PA3 <-> D0 | PB10 <-> D4 | PC0 <-> STP
PA11 <-> Data -  | PB0 <-> D1 | PB11 <-> D5 | PA5 <-> CLK
PA10 <-> ID      | PB1 <-> D2 | PB12 <-> D6 | PH4 <-> NXT
PA9  <-> VBUS    | PB5 <-> D3 | PB13 <-> D7 | PC2 <-> DIR
PD5  <-> VBUS_EN |
\endverbatim
 * 
 * \par Enabling USB output
 *
 * When USB port is declared as HOST mode, then you has ability to control (if you have external control for that) if external componentas allows you that.
 * All discovery board has STMPS2151 chip which is designed for USB purposes. It has enable pin and allows up to 500mA current. If more current want to flow,
 * chip will automaticall disable output and output pin will go low to indicate short circuit.
 *
 * So, when using discovery board in USB port without external PHY, you have to enable USB output to give power on USB pin.
 *
 * As mentioned above, library has feature to control this output. When you enable USB with @ref TM_USB_Init function, output is also enabled if needed.
 * To tell library, which output should be used, some defines should be set. Open defines.h file and copy/edit settings.
 * @note  If you are using some of my predefined boards, then this settings are automatically done and you don't need to do anything here.
 *
\code
//Use ENABLE pin on USB HS mode if needed
#define USB_HS_USE_ENABLE_PIN
#define USB_HS_ENABLE_PORT     GPIOC      //Enable PORT
#define USB_HS_ENABLE_PIN      GPIO_PIN_4 //Enable PIN
#define USB_HS_ENABLE_STATE    0          //State of pin when output is enabled

//Use ENABLE pin on USB FS mode if needed
#define USB_FS_USE_ENABLE_PIN
#define USB_FS_ENABLE_PORT     GPIOC      //Enable PORT
#define USB_FS_ENABLE_PIN      GPIO_PIN_4 //Enable PIN
#define USB_FS_ENABLE_STATE    0          //State of pin when output is enabled
\endcode
 *
 * @note  If enable pin is one pin on STM32F4/7xx device, then after you make defines, you are done with worrings.
 *
 * @note  If enable pin is like on STM32F439-EVAL, where I2C IO expander is used, then you can use @ref TM_USB_DriveVBUSCallback 
 *           callback function which will be called when there should be set output for USB. Check function for more information.
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
 - TM GPIO
 - USB CORE
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"

/**
 * @defgroup TM_USB_Macros
 * @brief    Library defines
 * @{
 */

/* Check for STM32F439-EVAL board */
#if defined(STM32F439_EVAL) || defined(USB_USE_STM32F439_EVAL)
#ifndef USB_USE_STM32F439_EVAL
#define USB_USE_STM32F439_EVAL
#endif
	
	/* Use ULPI PHY for HS */
	#define USB_USE_ULPI_PHY
#endif

/* Check for STM32F7-Discovery board */
#if defined(STM32F7_DISCOVERY) || defined(USB_USE_STM32F7_DISCOVERY)
#ifndef USB_USE_STM32F7_DISCOVERY
#define USB_USE_STM32F7_DISCOVERY
#endif
	/* Use enable pin */
	#define USB_FS_USE_ENABLE_PIN
	#define USB_FS_ENABLE_PORT     GPIOD
	#define USB_FS_ENABLE_PIN      GPIO_PIN_5
	#define USB_FS_ENABLE_STATE    0
	
	/* Use ULPI PHY for HS */
	#define USB_USE_ULPI_PHY
#endif

/* Check for STM32F429-Discovery board */
#if defined(STM32F429_DISCOVERY) || defined(USB_USE_STM32F429_DISCOVERY)
#ifndef USB_USE_STM32F429_DISCOVERY
#define USB_USE_STM32F429_DISCOVERY
#endif
	/* Use enable pin */
	#define USB_HS_USE_ENABLE_PIN
	#define USB_HS_ENABLE_PORT     GPIOC
	#define USB_HS_ENABLE_PIN      GPIO_PIN_4
	#define USB_HS_ENABLE_STATE    0
#endif

/* Check for STM32F4-Discovery board */
#if defined(STM32F4_DISCOVERY) || defined(USB_USE_STM32F4_DISCOVERY)
#ifndef USB_USE_STM32F4_DISCOVERY
#define USB_USE_STM32F4_DISCOVERY
#endif
	/* Use enable pin */
	#define USB_FS_USE_ENABLE_PIN
	#define USB_FS_ENABLE_PORT     GPIOC
	#define USB_FS_ENABLE_PIN      GPIO_PIN_0
	#define USB_FS_ENABLE_STATE    0
#endif

/* NVIC preemption priority */
#ifndef USB_NVIC_PRIORITY
#define USB_NVIC_PRIORITY          1
#endif

/* Defines for FS and HS ID modes */
#ifndef USB_ID_FS
#define USB_ID_FS                  0
#endif
#ifndef USB_ID_HS
#define USB_ID_HS                  1
#endif

/* Used USB configurations */
/* Use defines.h for configuration! */
/* You can also uncomment lines here, but when new update arrives, things may be owerwritten again */
#ifndef USB_USE_FS
//#define USB_USE_FS
#endif
#ifndef USB_USE_HS
//#define USB_USE_HS
#endif
#ifndef USB_USE_HOST
//#define USB_USE_HOST
#endif
#ifndef USB_USE_DEVICE
//#define USB_USE_DEVICE
#endif
#ifndef USB_USE_ULPI_PHY
//#define USB_USE_ULPI_PHY
#endif

/* Check if USB HS supported */
#if defined(USB_USE_HS) && !defined(USB_OTG_HS)
/* Print warning */
#warning "tm_stm32_usb.h: USB HS mode is not supported for your target! USB HS mode support will be disabled!"

/* Disable support */
#undef USB_USE_HS
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_USB_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  USB result enumeration
 */
typedef enum {
	TM_USB_Result_Ok = 0x00, /*!< Everything OK */
	TM_USB_Result_Error      /*!< An error has occurred */
} TM_USB_Result_t;

/**
 * @brief  USB modes enumeration
 */
typedef enum {
	TM_USB_FS = 0x00, /*!< USB Full Speed mode */
	TM_USB_HS = 0x01, /*!< USB High Speed mode */
	TM_USB_Both       /*!< USB both modes */
} TM_USB_t;

/**
 * @}
 */

/**
 * @defgroup TM_USB_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes USB for future work
 * @note   This function must always be called at the beginning in case USB will be used in application
 * @param  None
 * @retval Member of @ref TM_USB_Result_t enumeration
 */
TM_USB_Result_t TM_USB_Init(void);

/**
 * @brief  Initializes USB Full Speed mode
 * @note   This functions is called from USB stack when needed, no need for manual calling
 * @param  None
 * @retval Member of @ref TM_USB_Result_t enumeration
 */
TM_USB_Result_t TM_USB_InitFS(void);

/**
 * @brief  Initializes USB High Speed mode
 * @note   This functions is called from USB stack when needed, no need for manual calling
 * @param  None
 * @retval Member of @ref TM_USB_Result_t enumeration
 */
TM_USB_Result_t TM_USB_InitHS(void);

/**
 * @brief  Enables or disabled VBUS (Power supply) for USB communication
 * @note   This functions has sense only if you have external hardware which can be controlled to enable/disable VBUS
 * @note   Functions has sense only when HOST mode is used with USB
 * @param  USB_Mode: USB Mode where VBUS should be enabled or disabled
 * @param  state: State to use for VBUS
 *            - 0: VBUS should be disabled, USB deactivated 
 *            - 1: VBUS should be enabled, USB activated
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_USB_DriveVBUSCallback(TM_USB_t USB_Mode, uint8_t state);

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
