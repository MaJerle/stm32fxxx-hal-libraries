/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/07/hal-library-2-leds-and-button-for-stm32-boards/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Leds and button library for all STM32F4 boards and STM32F7 boards (discovery/nucleo)
 *	
@verbatim
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
@endverbatim
 */
#ifndef TM_DISCO_H
#define TM_DISCO_H 100

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
 * @brief    Leds and buttons for discovery, nucleo and eval boards
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
 * - STM32F7-Discovery: (STM32F746) - <code>STM32F7_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN   on PI1
 *   - Button: (LOW when pressed)
 *     - Blue button on PI16
 *
 * \par Select your board
 *
 * To select your board, you have several options:
 *   - Add define for your board in defines.h file or
 *   - Add define for your board in compiler's global settings
 *     - For Keil uVision you have "Options for Target" and "C/C++" tab where you can set this.
 *
 * Imagine, we want to work with STM324x9-Eval board. Then, you can open <code>defines.h</code> file and add define:
 *
@verbatim
//Select STM32F439-Eval for DISCO library
#define STM32F439_EVAL
@endverbatim
 * Or if you want STM32F429-Discovery, do this:
@verbatim
//Select STM32F429-Discovery for DISCO library
#define STM32F429_DISCOVERY
@endverbatim
 *
 * \par Changelog
 *
@verbatim
 Version 1.0
  - First release
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM GPIO
@endverbatim
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
	
	#define TM_DISCO_SWAP_LOGIC
	
	#define TM_DISCO_LED_PORT			GPIOG
	#define TM_DISCO_LED_PINS			LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define TM_DISCO_BUTTON_PORT		GPIOA
	#define TM_DISCO_BUTTON_PIN			0x0001U
	#define TM_DISCO_BUTTON_PRESSED		1
	#define TM_DISCO_BUTTON_PULL		TM_GPIO_PuPd_DOWN
/* STM32F429 Discovery */
#elif defined(STM32F429_DISCOVERY)
	#define LED_GREEN					0x2000U
	#define LED_RED						0x4000U
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN | LED_RED
	
	#define TM_DISCO_LED_PORT			GPIOG
	#define TM_DISCO_LED_PINS			LED_GREEN | LED_RED

	#define TM_DISCO_BUTTON_PORT		GPIOA
	#define TM_DISCO_BUTTON_PIN			0x0001U
	#define TM_DISCO_BUTTON_PRESSED		1
	#define TM_DISCO_BUTTON_PULL		TM_GPIO_PuPd_DOWN
/* STM32F4 & STM32F401 & STM32F411 Discovery */
#elif defined(STM32F4_DISCOVERY) || defined(STM32F401_DISCOVERY) || defined(STM32F411_DISCOVERY)
	#define LED_GREEN					0x1000U
	#define LED_ORANGE					0x2000U
	#define LED_RED						0x4000U
	#define LED_BLUE					0x8000U
	#define LED_ALL						((uint16_t)((uint16_t)LED_GREEN | (uint16_t)LED_RED | (uint16_t)LED_ORANGE | (uint16_t)LED_BLUE))
	
	#define TM_DISCO_LED_PORT			GPIOD
	#define TM_DISCO_LED_PINS			LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define TM_DISCO_BUTTON_PORT		GPIOA
	#define TM_DISCO_BUTTON_PIN			0x0001U
	#define TM_DISCO_BUTTON_PRESSED		1
	#define TM_DISCO_BUTTON_PULL		TM_GPIO_PuPd_DOWN
/* Nucleo F401-RE & F411-RE & F446-RE */
#elif defined(NUCLEO_F401) || defined(NUCLEO_F411) || defined(NUCLEO_F446) || defined(NUCLEO_F091)
	#define LED_GREEN					GPIO_PIN_5
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN
	
	#define TM_DISCO_LED_PORT			GPIOA
	#define TM_DISCO_LED_PINS			LED_GREEN

	#define TM_DISCO_BUTTON_PORT		GPIOC
	#define TM_DISCO_BUTTON_PIN			0x2000U
	#define TM_DISCO_BUTTON_PRESSED		0
	#define TM_DISCO_BUTTON_PULL		TM_GPIO_PuPd_UP
/* STM32F7-Discovery */
#elif defined(STM32F7_DISCOVERY)
	#define LED_GREEN					0x0002U
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN
	
	#define TM_DISCO_LED_PORT			GPIOI
	#define TM_DISCO_LED_PINS			LED_GREEN

	#define TM_DISCO_BUTTON_PORT		GPIOI
	#define TM_DISCO_BUTTON_PIN			0x0800
	#define TM_DISCO_BUTTON_PRESSED		1
	#define TM_DISCO_BUTTON_PULL		TM_GPIO_PuPd_DOWN
/* No board used */
#else
	#warning "tm_stm32_disco.h: Please select your board. Open tm_stm32_disco.h and follow instructions! Support is currently disabled"
	
	#define LED_GREEN					0
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						0
	
	#define TM_DISCO_LED_PORT			GPIOA
	#define TM_DISCO_LED_PINS			LED_ALL

	#define TM_DISCO_BUTTON_PORT		GPIOA
	#define TM_DISCO_BUTTON_PIN			0
	#define TM_DISCO_BUTTON_PRESSED		0
	#define TM_DISCO_BUTTON_PULL		TM_GPIO_PuPd_NOPULL
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
#ifndef TM_DISCO_SWAP_LOGIC
#define TM_DISCO_LedOn(led)        TM_GPIO_SetPinHigh(TM_DISCO_LED_PORT, (led))
#else
#define TM_DISCO_LedOn(led)        TM_GPIO_SetPinLow(TM_DISCO_LED_PORT, (led))
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
#ifndef TM_DISCO_SWAP_LOGIC
#define TM_DISCO_LedOff(led)       TM_GPIO_SetPinLow(TM_DISCO_LED_PORT, (uint16_t)(led))
#else
#define TM_DISCO_LedOff(led)       TM_GPIO_SetPinHigh(TM_DISCO_LED_PORT, (led))
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
#define TM_DISCO_LedToggle(led)        TM_GPIO_TogglePinValue(TM_DISCO_LED_PORT, (led))

/**
 * @brief  Checks if led is on
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: Led you want to checking
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval 1 if led is on or 0 if not
 */
#ifndef TM_DISCO_SWAP_LOGIC
#define TM_DISCO_LedIsOn(led)          TM_GPIO_GetOutputPinValue(TM_DISCO_LED_PORT, (led))
#else
#define TM_DISCO_LedIsOn(led)          !TM_GPIO_GetOutputPinValue(TM_DISCO_LED_PORT, (led))
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
#define TM_DISCO_ButtonPressed()       ((TM_GPIO_GetInputPinValue(TM_DISCO_BUTTON_PORT, TM_DISCO_BUTTON_PIN) == 0) != TM_DISCO_BUTTON_PRESSED)

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
