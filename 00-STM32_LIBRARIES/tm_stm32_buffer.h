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
 * This buffer does not only feature basic read/write operations for cyclic buffers, it also has feature to read strings from buffer if needed.
 *
 * \par Read string procedure
 *
 * Each string in buffer has separator character, in most cases, Line Feed (0x0A) is used, and is also default value when buffer is initialized.
 * When reading as string from buffer, you have to know this things:
 *
\verbatim
- Buffer will first check if string delimiter character exists in buffer. 
    - If it exists, characters will be set to buffer until delimiter is detected. 
	- Delimiter is included in string!
- If string delimiter is not in buffer, but is cyclic buffer full, 
    then string will be also filled into user buffer, because we need to free
	some memory for future characters, including string delimiter character
- If user buffer size is less than number of characters in buffer before string delimiter is found, 
    string is also filled in user buffer
- In all other cases, if there is no string delimiter in buffer, buffer will not return anything and will check it.
\endverbatim
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
	uint16_t Size;           /*!< Size of buffer in units of bytes */
	uint16_t In;             /*!< Input pointer to save next value */
	uint16_t Out;            /*!< Output pointer to read next value */
	uint16_t Num;            /*!< Number of elements in buffer */
	uint8_t* Buffer;         /*!< Pointer to buffer data array */
	uint8_t Flags;           /*!< Flags for buffer */
	uint8_t StringDelimiter; /*!< Character for string delimiter when reading from buffer as string */
    void* UserParameters;    /*!< Pointer to user value if needed */
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
 *            - > 0: Buffer initialization error. Malloc has failed with allocation
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
 * @retval Status of element:
 *            - 0: Element was not found
 *            - > 0: Element found
 */
uint8_t TM_BUFFER_FindElement(TM_BUFFER_t* Buffer, uint8_t Element);

/**
 * @brief  Sets string delimiter character when reading from buffer as string
 * @param  Buffer: Pointer to @ref TM_BUFFER_t structure
 * @param  StringDelimIter: Character as string delimiter
 * @retval None
 */
#define TM_BUFFER_SetStringDelimiter(Buffer, StringDelimiter)  ((Buffer)->StringDelimiter = (StringDelimter))

/**
 * @brief  Reads from buffer as string
 * @param  *Buffer: Pointer to @ref TM_BUFFER_t structure
 * @param  *buff: Pointer to buffer where string will be stored
 * @param  buffsize: Buffer size in units of bytes
 * @retval Number of characters in string
 */
uint16_t TM_BUFFER_ReadString(TM_BUFFER_t* Buffer, char* buff, uint16_t buffsize);

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
