/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Identification library for STM32F0xx, STM32F4xx and STM32F7xx devices
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
#ifndef TM_IDENTIFICATION_H
#define TM_IDENTIFICATION_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_ID
 * @brief    Identification library for STM32F0xx, STM32F4xx and STM32F7xx devices
 * @{
 *
 * \par This library allows you to read following things:
 *  - Device's 96bit unique ID
 *  - Device signature 
 *  - Revision
 *  - Flash size
 *
 * \par Revisions possible:
 *  - 0x1000: Revision A
 *  - 0x1001: Revision Z
 *  - 0x1003: Revision Y
 *  - 0x1007: Revision 1
 *  - 0x2001: Revision 3
 *
 * \par Device signatures:
 *  - 0x0413: STM32F405xx/07xx and STM32F415xx/17xx)
 *  - 0x0419: STM32F42xxx and STM32F43xxx
 *  - 0x0423: STM32F401xB/C
 *  - 0x0433: STM32F401xD/E
 *  - 0x0431: STM32F411xC/E
 *  - 0x0421: STM32F446xx
 *  - 0x0449: STM32F7x6xx
 *  - 0x0444: STM32F03xxx
 *  - 0x0445: STM32F04xxx
 *  - 0x0440: STM32F05xxx
 *  - 0x0448: STM32F07xxx
 *  - 0x0442: STM32F09xxx
 *
 * \par Package description (Binary values):
 *  - 0b01xx: LQFP208 and TFBGA216 package
 *  - 0b0011: LQFP176 and UFBGA176 package
 *  - 0b0010: WLCSP143 and LQFP144 package
 *  - 0b0001: LQFP100 package
 *
 * @note Device package address is not available on all STM32F4xx series. Please check which are supported before using it!
 *
 * \par Flash size:
 *  - Device has stored value of flash size in kB
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
\endverbatim
 */
#include "stm32fxxx_hal.h"

/**
 * @defgroup TM_ID_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  Unique ID register address location
 */
#if defined(STM32F0xx)
#define ID_UNIQUE_ADDRESS        0x1FFFF7AC /*!< STM32F0xx address */
#endif
#if defined(STM32F4xx)
#define ID_UNIQUE_ADDRESS        0x1FFF7A10 /*!< STM32F4xx address */
#endif
#if defined(STM32F7xx)
#define ID_UNIQUE_ADDRESS        0x1FF0F420 /*!< STM32F7xx address */
#endif

/**
 * @brief  Flash size register address
 */
#if defined(STM32F0xx)
#define ID_FLASH_ADDRESS         0x1FFFF7CC /*!< STM32F0xx address */
#endif
#if defined(STM32F4xx)
#define ID_FLASH_ADDRESS         0x1FFF7A22 /*!< STM32F4xx address */
#endif
#if defined(STM32F7xx)
#define ID_FLASH_ADDRESS         0x1FF0F442 /*!< STM32F7xx address */
#endif

/**
 * @brief  Device package address
 */
#define ID_PACKAGE_ADDRESS       0x1FFF7BF0

 /**
 * @}
 */

/**
 * @defgroup TM_ID_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Get STM32Fxxx device signature
 * @note   Defined as macro to get maximal response time
 * @param  None
 * @retval Device signature, bits 11:0 are valid, 15:12 are always 0
 */
#define TM_ID_GetSignature()     (DBGMCU->IDCODE & 0x00000FFF)

/**
 * @brief  Gets STM32Fxxx device revision
 * @note   Defined as macro to get maximal response time
 * @param  None
 * @retval Device revision value
 */
#define TM_ID_GetRevision()      ((DBGMCU->IDCODE >> 16) & 0x0000FFFF)

/**
 * @brief  Gets STM32Fxxx device package
 * @note   Defined as macro to get maximal response time
 * @note   This is not available for all packages, somewhere HardFault may occur
 * @param  None
 * @retval Device revision value
 */
#define TM_ID_GetPackage()       (((*(__IO uint16_t *) (ID_PACKAGE_ADDRESS)) & 0x0700) >> 8)

/**
 * @brief  Gets STM32Fxxx device flash size in kilo bytes
 * @note   Defined as macro to get maximal response time
 * @param  None
 * @retval Flash size in kilo bytes
 */
#define TM_ID_GetFlashSize()     (*(__IO uint16_t *) (ID_FLASH_ADDRESS))

/**
 * @brief  Gets STM32Fxxx unique ID number in 8-bit format
 * @note   STM32Fxxx has 96bits long unique ID, so 12 bytes are available for read in 8-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 8 bits you want to read
 *               - Values between 0 and 11 are allowed
 * @retval Unique ID address
 */
#define TM_ID_GetUnique8(x)      ((x >= 0 && x < 12) ? (*(__IO uint8_t *) (ID_UNIQUE_ADDRESS + (x))) : 0)
	
/**
 * @brief  Gets unique ID number in 16-bit format
 * @note   STM32Fxxx has 96bits long unique ID, so 6 2-bytes values are available for read in 16-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 16 bits you want to read
 *               - Values between 0 and 5 are allowed
 * @retval Unique ID address
 */
#define TM_ID_GetUnique16(x)     ((x >= 0 && x < 6) ? (*(__IO uint16_t *) (ID_UNIQUE_ADDRESS + 2 * (x))) : 0)

/**
 * @brief  Get unique ID number in 32-bit format
 * @note   STM32Fxxx has 96bits long unique ID, so 3 4-bytes values are available for read in 32-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 32 bits you want to read
 *               - Values between 0 and 2 are allowed
 * @retval Unique ID address
 */
#define TM_ID_GetUnique32(x)     ((x >= 0 && x < 3) ? (*(__IO uint32_t *) (ID_UNIQUE_ADDRESS + 4 * (x))) : 0)

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
