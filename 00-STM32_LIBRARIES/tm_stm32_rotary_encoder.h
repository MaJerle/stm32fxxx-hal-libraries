/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Rotary encoder library for STM32F4xx devices
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
#ifndef TM_RE_H
#define TM_RE_H 100

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_ROTARY_ENCODER
 * @brief    Rotary encoder library for STM32Fxxx devices
 * @{
 *
 * This library supports up to 16 rotary encoders at a time.
 * It does not use TIM's rotary input feature but external interrupts for handling.
 *
 * \par Pinout
 *
 * Pinout is fully selectable on rotary initialization.
 *
 * \par Encoder modes
 *
 * By default, if you turn encoder CW, you will increase counter, if CCW, you will decrease counter. Basically, this depends on your wiring for rotary.
 *
 * Library allows you to turn this mode "on the fly" anytime you want. Look for @ref TM_RE_SetMode() function for that.
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
 - misc.h
 - defines.h
 - TM EXTI
 - TM GPIO
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"
#include "tm_stm32_exti.h"
 
/**
 * @defgroup TM_ROTARY_ENCODER_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Rotary encoder rotation status
 */
typedef enum {
	TM_RE_Rotate_Increment, /*!< Encoder was incremented */
	TM_RE_Rotate_Decrement, /*!< Encoder was decremented */
	TM_RE_Rotate_Nothing    /*!< Encoder stop at it was before */
} TM_RE_Rotate_t;

/**
 * @brief  Rotary encoder mode selection for rotation
 */
typedef enum {
	TM_RE_Mode_Zero, /*!< Rotary encoder mode zero. It is used for direction when it will be increment od decrement, default used */
	TM_RE_Mode_One   /*!< Rotary encoder mode one. It is used for direction when it will be increment od decrement */
} TM_RE_Mode_t;

/**
 * @brief  Rotary main working structure 
 */
typedef struct {
	int32_t Absolute;        /*!< Absolute rotation from beginning, for public use */
	int32_t Diff;            /*!< Rotary difference from last check, for public use */
	TM_RE_Rotate_t Rotation; /*!< Increment, Decrement or nothing, for public use */
	TM_RE_Mode_t Mode;       /*!< Rotary encoder mode selected */
	uint8_t LastA;           /*!< Last status of A pin when checking. Meant for private use */
	int32_t RE_Count;        /*!< Temporary variable to store data between rotation and user check */
	GPIO_TypeDef* GPIO_A;    /*!< Pointer to GPIOx for Rotary encode A pin. Meant for private use */
	GPIO_TypeDef* GPIO_B;    /*!< Pointer to GPIOx for Rotary encode B pin. Meant for private use */
	uint16_t GPIO_PIN_A;     /*!< GPIO pin for rotary encoder A pin. This pin is also set for interrupt */
	uint16_t GPIO_PIN_B;     /*!< GPIO pin for rotary encoder B pin. */
} TM_RE_t;

/**
 * @}
 */

/**
 * @defgroup TM_ROTARY_ENCODER_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Prepare Rotary Encoder to work
 * @param  *data: Pointer to @ref TM_RE_t structure
 * @retval None
 */
void TM_RE_Init(TM_RE_t* data, GPIO_TypeDef* GPIO_A_Port, uint16_t GPIO_A_Pin, GPIO_TypeDef* GPIO_B_Port, uint16_t GPIO_B_Pin);

/**
 * @brief  Set rotary encoder custom mode
 * @param  *data: Pointer to @ref TM_RE_t structure for specific rotary encoder input
 * @param  mode: Rotary mode you will use. This parameter can be a value of @ref TM_RE_Mode_t enumeration
 * @retval None
 */
void TM_RE_SetMode(TM_RE_t* data, TM_RE_Mode_t mode);

/**
 * @brief  Checks and gets new values of rotary encoder
 * @param  *data: Pointer to @ref TM_RE_t structure
 * @retval Member of @ref TM_RE_Rotate_t
 */
TM_RE_Rotate_t TM_RE_Get(TM_RE_t* data);

/**
 * @brief  Process function.
 * @note   This function have to be called inside your interrupt handler.
 * @param  *data: Pointer to rotary encoder @ret TM_RE_t data where interrupt occured
 * @retval None
 */
void TM_RE_Process(TM_RE_t* data);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

#endif


