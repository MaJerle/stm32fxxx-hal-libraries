/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_buffer.h"

uint8_t TM_BUFFER_Init(TM_BUFFER_t* Buffer, uint16_t Size, uint8_t* BufferPtr) {
	/* Set buffer values to all zeros */
	memset(Buffer, 0, sizeof(TM_BUFFER_t));
	
	/* Set default values */
	Buffer->Size = Size;
	Buffer->Buffer = BufferPtr;
	Buffer->StringDelimiter = '\n';
	
	/* Check if malloc should be used */
	if (!Buffer->Buffer) {
		/* Try to allocate */
		Buffer->Buffer = (uint8_t *) LIB_ALLOC_FUNC(Size * sizeof(uint8_t));
		
		/* Check if allocated */
		if (!Buffer->Buffer) {
			/* Reset size */
			Buffer->Size = 0;
			
			/* Return error */
			return 1;
		} else {
			/* Set flag for malloc */
			Buffer->Flags |= BUFFER_MALLOC;
		}
	}
	
	/* We are initialized */
	Buffer->Flags |= BUFFER_INITIALIZED;
	
	/* Initialized OK */
	return 0;
}

void TM_BUFFER_Free(TM_BUFFER_t* Buffer) {
	/* Check buffer structure */
	if (Buffer == NULL) {
		return;
	}
	
	/* If malloc was used for allocation */
	if (Buffer->Flags & BUFFER_MALLOC) {
		/* Free memory */
		LIB_FREE_FUNC(Buffer->Buffer);
	}
	
	/* Clear flags */
	Buffer->Flags = 0;
	Buffer->Size = 0;
}

uint16_t TM_BUFFER_Write(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t count) {
	uint8_t i = 0;
	
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}

	/* Check input pointer */
	if (Buffer->In >= Buffer->Size) {
		Buffer->In = 0;
	}
	
	/* Go through all elements */
	while (count-- && Buffer->Num < Buffer->Size) {
		/* Add to buffer */
		Buffer->Buffer[Buffer->In] = *Data++;
		
		/* Increase pointers */
		Buffer->In++;
		Buffer->Num++;
		i++;
		
		/* Check input overflow */
		if (Buffer->In >= Buffer->Size) {
			Buffer->In = 0;
		}
	}
	
	/* Return number of elements stored in memory */
	return i;
}

uint16_t TM_BUFFER_Read(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t count) {
	uint16_t i = 0;
	
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}

	/* Check output pointer */
	if (Buffer->Out >= Buffer->Size) {
		Buffer->Out = 0;
	}
	
	/* Go through all elements */
	while (count-- && Buffer->Num > 0) {
		/* Save to user buffer */
		*Data++ = Buffer->Buffer[Buffer->Out];
		
		/* Increase pointers */
		Buffer->Out++;
		Buffer->Num--;
		i++;

		/* Check output overflow */
		if (Buffer->Out >= Buffer->Size) {
			Buffer->Out = 0;
		}
	}
	
	/* Return number of elements read from buffer */
	return i;
}

uint16_t TM_BUFFER_GetFree(TM_BUFFER_t* Buffer) {
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Return number of free memory in buffer */
	return (Buffer->Size - Buffer->Num);
}

uint16_t TM_BUFFER_GetFull(TM_BUFFER_t* Buffer) {
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Return number of elements in buffer */
	return Buffer->Num;
}

void TM_BUFFER_Reset(TM_BUFFER_t* Buffer) {
	/* Check buffer structure */
	if (Buffer == NULL) {
		return;
	}
	
	/* Reset values */
	Buffer->In = 0;
	Buffer->Out = 0;
	Buffer->Num = 0;
}

uint16_t TM_BUFFER_FindElement(TM_BUFFER_t* Buffer, uint8_t Element) {
	uint16_t Num, Out, retval = 0;
	
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Create temporary variables */
	Num = TM_BUFFER_GetFull(Buffer);
	Out = Buffer->Out;
	
	/* Go through input elements */
	while (Num > 0) {
		/* Check output overflow */
		if (Out >= Buffer->Size) {
			Out = 0;
		}
		
		/* Check for element */
		if ((uint8_t)Buffer->Buffer[Out] == (uint8_t)Element) {
			/* Element found, return position in buffer */
			return (retval + 1);
		}
		
		/* Set new variables */
		Out++;
		Num--;
		retval++;
	}
	
	/* Element is not in buffer */
	return 0;
}

uint16_t TM_BUFFER_Find(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t Size) {
	uint16_t Num, Out, i, retval = 0;
	uint8_t found = 0;

	/* Check buffer structure and number of elements in buffer */
	if (Buffer == NULL || (Num = TM_BUFFER_GetFull(Buffer)) < Size) {
		return 0;
	}

	/* Create temporary variables */
	Out = Buffer->Out;

	/* Go through input elements in buffer */
	while (Num > 0) {
		/* Check output overflow */
		if (Out >= Buffer->Size) {
			Out = 0;
		}

		/* Check if current element in buffer matches first element in data array */
		if ((uint8_t)Buffer->Buffer[Out] == (uint8_t)Data[0]) {
			found = 1;
		}

		/* Set new variables */
		Out++;
		Num--;
		retval++;

		/* We have found first element */
		if (found) {
			/* First character found */
			/* Check others */
			i = 1;
			while (i < Size) {
				/* Check output overflow */
				if (Out >= Buffer->Size) {
					Out = 0;
				}

				/* Check if current character in buffer matches character in string */
				if ((uint8_t)Buffer->Buffer[Out] != (uint8_t)Data[i]) {
					retval += i - 1;
					break;
				}

				/* Set new variables */
				Out++;
				Num--;
				i++;
			}

			/* We have found data sequence in buffer */
			if (i == Size) {
				return retval;
			}
		}
	}

	/* Data sequence is not in buffer */
	return 0;
}

uint16_t TM_BUFFER_ReadString(TM_BUFFER_t* Buffer, char* buff, uint16_t buffsize) {
	uint16_t i = 0;
	uint8_t ch;
	uint16_t freeMem;
	
	/* Check value buffer */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Get free */
	freeMem = TM_BUFFER_GetFree(Buffer);
	
	/* Check for any data on USART */
	if (
		freeMem == 0 ||                                                    /*!< Buffer empty */
		(
			TM_BUFFER_FindElement(Buffer, Buffer->StringDelimiter) == 0 && /*!< String delimiter is not in buffer */
			freeMem != 0 &&                                                /*!< Buffer is not full */
			TM_BUFFER_GetFull(Buffer) < buffsize                           /*!< User buffer size is larger than number of elements in buffer */
		)
	) {
		/* Return 0 */
		return 0;
	}
	
	/* If available buffer size is more than 0 characters */
	while (i < (buffsize - 1)) {
		/* We have available data */
		TM_BUFFER_Read(Buffer, &ch, 1);
		
		/* Save character */
		buff[i] = (char)ch;
		
		/* Check for end of string */
		if ((char)buff[i] == (char)Buffer->StringDelimiter) {
			/* Done */
			break;
		}
		
		/* Increase */
		i++;
	}
	
	/* Add zero to the end of string */
	if (i == (buffsize - 1)) {
		buff[i] = 0;
	} else {
		buff[++i] = 0;
	}

	/* Return number of characters in buffer */
	return i;
}
