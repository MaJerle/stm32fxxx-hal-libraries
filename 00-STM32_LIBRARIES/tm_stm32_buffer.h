/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Beneric cyclic buffer library 
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
#ifndef TM_BUFFER_H
#define TM_BUFFER_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_BUFFER
 * @brief    Generic cyclic buffer library
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
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "stdlib.h"

/**
 * @defgroup TM_BUFFER_Macros
 * @brief    Library defines
 * @{
 */

#define BUFFER_INITIALIZED     0x01 /*!< Buffer initialized flag */
#define BUFFER_MALLOC          0x02 /*!< Buffer uses malloc for memory allocation */

/**
 * @}
 */
 
/**
 * @defgroup TM_BUFFER_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Buffer structure
 */
typedef struct _TM_BUFFER_t {
	uint16_t Size;   /*!< Size of buffer in units of bytes */
	uint16_t In;     /*!< Input pointer to save next value */
	uint16_t Out;    /*!< Output pointer to read next value */
	uint16_t Num;    /*!< Number of elements in buffer */
	uint8_t* Buffer; /*!< Pointer to buffer data array */
	uint8_t Flags;   /*!< Flags for buffer */
} TM_BUFFER_t;

/**
 * @}
 */

/**
 * @defgroup TM_BUFFER_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes buffer structure for work
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure to initialize
 * @param  Size: Size of buffer in units of bytes
 * @param  *BufferPtr: Pointer to array for buffer storage. Its length should be equal to @param Size parameter.
 * @param  UseMalloc: Set to 1 if you want to allocate buffer using malloc. In case of parameter is 1, BufferPtr param can be NULL
 * @retval Buffer initialization status:
 *            - 0: Buffer iintialized OK
 *            - 0: Buffer iintialized error. Malloc has failed with allocation
 */
uint8_t TM_BUFFER_Init(TM_BUFFER_t* Buffer, uint16_t Size, uint8_t* BufferPtr, uint8_t UseMalloc);

/**
 * @brief  Free memory for buffer allocated using malloc
 * @note   This function has sense only if malloc was used for dynamic allocation
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @retval None
 */
void TM_BUFFER_Free(TM_BUFFER_t* Buffer);

/**
 * @brief  Writes data to buffer
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @param  *Data: Pointer to data to be written
 * @param  count: Number of elements of type unsigned char to write
 * @retval Number of elements written in buffer 
 */
uint16_t TM_BUFFER_Write(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t count);

/**
 * @brief  Reads data from buffer
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @param  *Data: Pointer to data where read values will be stored
 * @param  count: Number of elements of type unsigned char to read
 * @retval Number of elements read from buffer 
 */
uint16_t TM_BUFFER_Read(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t count);

/**
 * @brief  Gets number of free elements in buffer 
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @retval Number of free elements in buffer
 */
uint16_t TM_BUFFER_GetFree(TM_BUFFER_t* Buffer);

/**
 * @brief  Gets number of elements in buffer 
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @retval Number of elements in buffer
 */
uint16_t TM_BUFFER_GetFull(TM_BUFFER_t* Buffer);

/**
 * @brief  Resets (clears) buffer pointers
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @retval None
 */
void TM_BUFFER_Reset(TM_BUFFER_t* Buffer);

/**
 * @brief  Checks if specific element value is stored in buffer
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @param  uint8_t Element: Element to check
 */
uint8_t TM_BUFFER_FindElement(TM_BUFFER_t* Buffer, uint8_t Element);

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
