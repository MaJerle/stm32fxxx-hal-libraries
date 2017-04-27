/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-2-leds-and-button-for-stm32-boards/
 * @version v1.1
 * @ide     Keil uVision
 * @license MIT
 * @brief   Leds and button library for all STM32F4 boards and STM32F7 boards (discovery/nucleo)
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
#ifndef TM_DISCO_H
#define TM_DISCO_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_DISCO
 * @brief    Leds and buttons for discovery, nucleo and eval boards - http://stm32f4-discovery.net/2015/07/hal-library-2-leds-and-button-for-stm32-boards/
 * @{
 *	
 * \par Supported boards
 *			
 * - STM32F429 Discovery: (STM32F429ZI) - <code>STM32F429_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN   on PG13
 *     - LED_RED     on PG14
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0	
 * - STM32F401-Discovery: (STM32F401VC) - <code>STM32F401_DISCOVERY</code>
 * - STM32F411-Discovery: (STM32F411VE) - <code>STM32F411_DISCOVERY</code>
 * - STM32F4-Discovery: (STM32F407VG) - <code>STM32F4_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN   on PD12
 *     - LED_ORANGE  on PD13
 *     - LED_RED     on PD14
 *     - LED_BLUE    on PD15
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0
 * - NUCLEO-F401: (STM32F401RE) - <code>NUCLEO_F401</code>
 * - NUCLEO-F411: (STM32F411RE) - <code>NUCLEO_F411</code>
 * - NUCLEO-F446: (STM32F446RE) - <code>NUCLEO_F446</code>
 * - NUCLEO-F091: (STM32F091RC) - <code>NUCLEO_F091</code>
 *   - Led:
 *     - LED_GREEN   on PA5
 *   - Button: (LOW when pressed)
 *     - Blue button on PC13
 * - STM32439-Eval (STM32F439NI) - <code>STM32F439_EVAL</code>
 *   - Leds:
 *     - LED_GREEN   on PG6
 *     - LED_ORANGE  on PG7
 *     - LED_RED     on PG10
 *     - LED_BLUE    on PG12
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0
 * - STM32F469-Discovery: (STM32F469NI) - <code>STM32F469_DISCOVERY</code>
 *   - Leds:
 *     - LED1 on PG6
 *     - LED2 on PD4
 *     - LED3 on PD5
 *     - LED4 on PK3
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0
 * - STM32F7-Discovery: (STM32F746NI) - <code>STM32F7_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN   on PI1
 *   - Button: (LOW when pressed)
 *     - Blue button on PI16
 * - STM32F769-Discovery: (STM32F769NI) - <code>STM32F769_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN on PJ5
 *     - LED_RED   on PJ13
 *     - LED4 on PK3
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0 
 *    
 * \par Select your board
 *
 * To select your board, you have several options:
 *   - Add define for your board in defines.h file or
 *   - Add define for your board in compiler's global settings
 *     - For Keil uVision you have "Options for Target" and "C/C++" tab where you can set this.
 *
 * @note  If you are using my other libs, for example SDRAM or LCD where pins and settings depend on board used, 
 *        then I recommend you select your board using this library.
 *        If you use, for example STM32F429-Discovery board, then add global define "STM32F429_DISCOVERY" in defines.h file
 *        and all other libraries which will detect this define will know you are using this board and default settings (if exists) 
 *        will be automatically detected. For example, SDRAM/LCD/USB are 3 of many libraries which needs special
 *        defines according to board used in your project. Using global define for your board, libs will detect settings needed for work.
 *
 * Imagine, we want to work with STM324x9-Eval board. Then, you can open <code>defines.h</code> file and add define:
 *
\code
//Select STM32F439-Eval for DISCO library
#define STM32F439_EVAL
\endcode
 * Or if you want STM32F429-Discovery, do this:
\code
//Select STM32F429-Discovery for DISCO library
#define STM32F429_DISCOVERY
\endcode
 *
 * \par All boards and its defines
 *
\code{.c}
//Use proper define for your board
#define STM32F429_DISCOVERY
#define STM32F401_DISCOVERY
#define STM32F411_DISCOVERY
#define STM32F4_DISCOVERY
#define NUCLEO_F401
#define NUCLEO_F411
#define NUCLEO_F446
#define NUCLEO_F091
#define STM32F439_EVAL
#define STM32F469_DISCOVERY
#define STM32F7_DISCOVERY
#define STM32F769_DISCOVERY
\endcode
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
  
 Version 1.1
  - October 10, 2015
  - Added support for STM32F469-Discovery
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM GPIO
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"

/* STM32F439 Eval */
#if defined(STM32F439_EVAL)
	#define LED_GREEN					0x0040U
	#define LED_ORANGE					0x0080U
	#define LED_RED						0x0400U
	#define LED_BLUE					0x1000U
	#define LED_ALL						LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE
	
	#define DISCO_SWAP_LOGIC
	
	#define DISCO_LED_PORT				GPIOG
	#define DISCO_LED_PINS				LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0x0001U
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_DOWN
    
    #define DISCO_USART                 USART1
    #define DISCO_USART_PP              TM_USART_PinsPack_1
/* STM32F429 Discovery */
#elif defined(STM32F429_DISCOVERY)
	#define LED_GREEN					0x2000U
	#define LED_RED						0x4000U
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN | LED_RED
	
	#define DISCO_LED_PORT				GPIOG
	#define DISCO_LED_PINS				LED_GREEN | LED_RED

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0x0001U
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_DOWN
    
    #define DISCO_USART                 USART1
    #define DISCO_USART_PP              TM_USART_PinsPack_1
/* STM32F4 & STM32F401 & STM32F411 Discovery */
#elif defined(STM32F4_DISCOVERY) || defined(STM32F401_DISCOVERY) || defined(STM32F411_DISCOVERY)
	#define LED_GREEN					0x1000U
	#define LED_ORANGE					0x2000U
	#define LED_RED						0x4000U
	#define LED_BLUE					0x8000U
	#define LED_ALL						((uint16_t)((uint16_t)LED_GREEN | (uint16_t)LED_RED | (uint16_t)LED_ORANGE | (uint16_t)LED_BLUE))
	
	#define DISCO_LED_PORT				GPIOD
	#define DISCO_LED_PINS				LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0x0001U
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_DOWN
    
    #define DISCO_USART                 USART1
    #define DISCO_USART_PP              TM_USART_PinsPack_2
/* Nucleo F401-RE & F411-RE & F446-RE & F091-RE */
#elif defined(NUCLEO_F401) || defined(NUCLEO_F411) || defined(NUCLEO_F446) || defined(NUCLEO_F091)
	#define LED_GREEN					GPIO_PIN_5
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN
	
	#define DISCO_LED_PORT				GPIOA
	#define DISCO_LED_PINS				LED_GREEN

	#define DISCO_BUTTON_PORT			GPIOC
	#define DISCO_BUTTON_PIN			0x2000U
	#define DISCO_BUTTON_PRESSED		0
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_UP
    
    #define DISCO_USART                 USART2
    #define DISCO_USART_PP              TM_USART_PinsPack_1
/* STM32F469-Discovery */
#elif defined(STM32F469_DISCOVERY)
	#define LED_GREEN					GPIO_PIN_6
	#define LED_RED						GPIO_PIN_4
	#define LED_ORANGE					GPIO_PIN_5
	#define LED_BLUE					GPIO_PIN_3
	#define LED_ALL						LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE
	
	#define DISCO_LED_GREEN_PORT		GPIOG
	#define DISCO_LED_RED_PORT			GPIOD
	#define DISCO_LED_ORANGE_PORT		GPIOD
	#define DISCO_LED_BLUE_PORT			GPIOK
	#define DISCO_LED_PINS				LED_ALL

	#define DISCO_BUTTON_PORT			GPIOI
	#define DISCO_BUTTON_PIN			0x0800
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_DOWN
	
	#define DISCO_USE_FUNCTIONS
    
    #define DISCO_USART                 USART3
    #define DISCO_USART_PP              TM_USART_PinsPack_1
/* STM32F7-Discovery */
#elif defined(STM32F7_DISCOVERY)
	#define LED_GREEN					0x0002U
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN
	
	#define DISCO_LED_PORT				GPIOI
	#define DISCO_LED_PINS				LED_GREEN

	#define DISCO_BUTTON_PORT			GPIOI
	#define DISCO_BUTTON_PIN			0x0800
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_DOWN
    
    #define DISCO_USART                 USART1
    #define DISCO_USART_PP              TM_USART_PinsPack_3
/* STM32F769-Discovery */
#elif defined(STM32F769_DISCOVERY)
	#define LED_GREEN					0x0020U
	#define LED_RED						0x2000U
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN | LED_RED
	
	#define DISCO_LED_PORT				GPIOJ
	#define DISCO_LED_PINS				LED_ALL

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0x0001U
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_DOWN
    
    #define DISCO_USART                 USART1
    #define DISCO_USART_PP              TM_USART_PinsPack_1
/* No board used */
#else
	#warning "tm_stm32_disco.h: Please select your board. Open tm_stm32_disco.h and follow instructions! Support is currently disabled"
	
	#define LED_GREEN					0
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						0
	
	#define DISCO_LED_PORT				GPIOA
	#define DISCO_LED_PINS				LED_ALL

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0
	#define DISCO_BUTTON_PRESSED		0
	#define DISCO_BUTTON_PULL			TM_GPIO_PuPd_NOPULL
    
    #define DISCO_USART                 USART1
    #define DISCO_USART_PP              TM_USART_PinsPack_1
#endif

/**
 * @defgroup TM_DISCO_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Configures LED pins as outputs
 * @param  None
 * @retval None
 */
void TM_DISCO_LedInit(void);

/**
 * @brief  Configures Button pin as input
 * @param  None
 * @retval None
 */
void TM_DISCO_ButtonInit(void);

/**
 * @brief  Turns on LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn on
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#if defined(DISCO_USE_FUNCTIONS) 
void TM_DISCO_LedOn(uint16_t led);
#else
#ifndef DISCO_SWAP_LOGIC
#define TM_DISCO_LedOn(led)        TM_GPIO_SetPinHigh(DISCO_LED_PORT, (uint16_t)(led))
#else
#define TM_DISCO_LedOn(led)        TM_GPIO_SetPinLow(DISCO_LED_PORT, (uint16_t)(led))
#endif
#endif

/**
 * @brief  Turns off LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn off
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#if defined(DISCO_USE_FUNCTIONS) 
void TM_DISCO_LedOff(uint16_t led);
#else
#ifndef DISCO_SWAP_LOGIC
#define TM_DISCO_LedOff(led)       TM_GPIO_SetPinLow(DISCO_LED_PORT, (uint16_t)(led))
#else
#define TM_DISCO_LedOff(led)       TM_GPIO_SetPinHigh(DISCO_LED_PORT, (uint16_t)(led))
#endif
#endif

/**
 * @brief  Toggles LED on board
 * @param  led: LED you want to toggle
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#if defined(DISCO_USE_FUNCTIONS)
void TM_DISCO_LedToggle(uint16_t led);
#else
#define TM_DISCO_LedToggle(led)        TM_GPIO_TogglePinValue(DISCO_LED_PORT, (uint16_t)(led))
#endif

/**
 * @brief  Checks if led is on
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: Led you want to checking
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 * @retval Led on status:
 *           - 0: Led is off
 *           - > 0: Led is on
 */
#if defined(DISCO_USE_FUNCTIONS)
uint16_t TM_DISCO_LedIsOn(uint16_t led);
#else
#ifndef DISCO_SWAP_LOGIC
#define TM_DISCO_LedIsOn(led)          TM_GPIO_GetOutputPinValue(DISCO_LED_PORT, (uint16_t)(led))
#else
#define TM_DISCO_LedIsOn(led)          !TM_GPIO_GetOutputPinValue(DISCO_LED_PORT, (uint16_t)(led))
#endif
#endif

/**
 * @brief  Sets led value
 * @param  led: LED you want to set value
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @param  state: Set or clear led 
 *            - 0: led is off
 *            - > 0: led is on
 * @retval None
 */
#define TM_DISCO_SetLed(led, state)    ((state) ? TM_DISCO_LedOn(led): TM_DISCO_LedOff(led))

/**
 * @brief  Checks if user button is pressed
 * @param  None
 * @retval Button status
 *            - 0: Button is not pressed
 *            - > 0: Button is pressed
 */
#define TM_DISCO_ButtonPressed()       ((TM_GPIO_GetInputPinValue(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN) == 0) != DISCO_BUTTON_PRESSED)

/**
 * @brief  Checks if button was pressed now, but was not already pressed before
 * @param  None
 * @retval Button on pressed value
 *           - 0: In case that button has been already pressed on last call or was not pressed at all yet
 *           - > 0: Button was pressed, but state before was released
 */
uint8_t TM_DISCO_ButtonOnPressed(void);

/**
 * @brief  Checks if button was released now, but was already pressed before
 * @param  None
 * @retval Button on released value
 *           - 0: In case that button has been already released on last call or was not released at all yet
 *           - > 0: Button was released, but state before was pressed
 */
uint8_t TM_DISCO_ButtonOnReleased(void);

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
