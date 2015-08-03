/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   USB library for STM32Fxxx devices
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
 * @brief    USB library for STM32Fxxx devices
 * @{
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

/**
 * @defgroup TM_USB_Macros
 * @brief    Library defines
 * @{
 */

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
#define USB_NVIC_PRIORITY          6
#endif

/* Defines */
#ifndef USB_ID_FS
#define USB_ID_FS                  0
#endif
#ifndef USB_ID_HS
#define USB_ID_HS                  1
#endif

/* Used USB configurations */
/* Use defines.h for configuration! */
//#define USB_USE_FS
//#define USB_USE_HS
//#define USB_USE_HOST
//#define USB_USE_DEVICE
//#define USB_USE_ULPI_PHY

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
