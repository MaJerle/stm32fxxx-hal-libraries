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

uint8_t TM_BUFFER_Init(TM_BUFFER_t* Buffer, uint32_t Size, uint8_t* BufferPtr) {
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

uint32_t TM_BUFFER_Write(TM_BUFFER_t* Buffer, uint8_t* Data, uint32_t count) {
	uint32_t i = 0;
	uint32_t free;
#if BUFFER_FAST
	uint32_t tocopy;
#endif

	/* Check buffer structure */
	if (Buffer == NULL || count == 0) {
		return 0;
	}

	/* Check input pointer */
	if (Buffer->In >= Buffer->Size) {
		Buffer->In = 0;
	}

	/* Get free memory */
	free = TM_BUFFER_GetFree(Buffer);

	/* Check available memory */
	if (free < count) {
		/* If no memory, stop execution */
		if (free == 0) {
			return 0;
		}

		/* Set values for write */
		count = free;
	}

	/* We have calculated memory for write */

#if BUFFER_FAST
	/* Calculate number of elements we can put at the end of buffer */
	tocopy = Buffer->Size - Buffer->In;

	/* Check for copy count */
	if (tocopy > count) {
		tocopy = count;
	}

	/* Copy content to buffer */
	memcpy(&Buffer->Buffer[Buffer->In], Data, tocopy);

	/* Increase number of bytes we copied already */
	i += tocopy;
	Buffer->In += tocopy;
	count -= tocopy;

	/* Check if anything to write */
	if (count > 0) {
		/* Start from the beginning of buffer */
		Buffer->In = 0;

		/* Copy content */
		memcpy(&Buffer->Buffer[Buffer->In], &Data[i], count);

		/* Set input pointer */
		Buffer->In = count;
	}

	/* Check input overflow */
	if (Buffer->In >= Buffer->Size) {
		Buffer->In = 0;
	}

	/* Return number of elements stored in memory */
	return (i + count);
#else
	/* Go through all elements */
	while (count--) {
		/* Add to buffer */
		Buffer->Buffer[Buffer->In++] = *Data++;

		/* Increase pointers */
		i++;

		/* Check input overflow */
		if (Buffer->In >= Buffer->Size) {
			Buffer->In = 0;
		}
	}

	/* Return number of elements written */
	return i;
#endif
}

uint32_t TM_BUFFER_Read(TM_BUFFER_t* Buffer, uint8_t* Data, uint32_t count) {
	uint32_t i = 0;
	uint32_t full;
#if BUFFER_FAST
	uint32_t tocopy;
#endif

	/* Check buffer structure */
	if (Buffer == NULL || count == 0) {
		return 0;
	}

	/* Check output pointer */
	if (Buffer->Out >= Buffer->Size) {
		Buffer->Out = 0;
	}

	/* Get free memory */
	full = TM_BUFFER_GetFull(Buffer);

	/* Check available memory */
	if (full < count) {
		/* If no memory, stop execution */
		if (full == 0) {
			return 0;
		}

		/* Set values for write */
		count = full;
	}

	/* We have calculated memory for write */

#if BUFFER_FAST
	/* Calculate number of elements we can put at the end of buffer */
	tocopy = Buffer->Size - Buffer->Out;

	/* Check for copy count */
	if (tocopy > count) {
		tocopy = count;
	}

	/* Copy content from buffer */
	memcpy(Data, &Buffer->Buffer[Buffer->Out], tocopy);

	/* Increase number of bytes we copied already */
	i += tocopy;
	Buffer->Out += tocopy;
	count -= tocopy;

	/* Check if anything to read */
	if (count > 0) {
		/* Start from the beginning of buffer */
		Buffer->Out = 0;

		/* Copy content */
		memcpy(&Data[i], &Buffer->Buffer[Buffer->Out], count);

		/* Set input pointer */
		Buffer->Out = count;
	}

	/* Check output overflow */
	if (Buffer->Out >= Buffer->Size) {
		Buffer->Out = 0;
	}

	/* Return number of elements stored in memory */
	return (i + count);
#else
	/* Go through all elements */
	while (count--) {
		/* Read from buffer */
		*Data++ = Buffer->Buffer[Buffer->Out++];

		/* Increase pointers */
		i++;

		/* Check output overflow */
		if (Buffer->Out >= Buffer->Size) {
			Buffer->Out = 0;
		}
	}

	/* Return number of elements stored in memory */
	return i;
#endif
}

uint32_t TM_BUFFER_GetFree(TM_BUFFER_t* Buffer) {
	uint32_t size, in, out;
	
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Save values */
	in = Buffer->In;
	out = Buffer->Out;
	
	/* Check if the same */
	if (in == out) {
		size = Buffer->Size;
	}

	/* Check normal mode */
	if (out > in) {
		size = out - in;
	}
	
	/* Check if overflow mode */
	if (in > out) {
		size = Buffer->Size - (in - out);
	}
	
	/* Return free memory */
	return size - 1;
}

uint32_t TM_BUFFER_GetFull(TM_BUFFER_t* Buffer) {
	uint32_t in, out, size;
	
	/* Check buffer structure */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Save values */
	in = Buffer->In;
	out = Buffer->Out;
	
	/* Pointer are same? */
	if (in == out) {
		size = 0;
	}
	
	/* Check pointers and return values */
	/* Buffer is not in overflow mode */
	if (in > out) {
		size = in - out;
	}
	
	/* Buffer is in overflow mode */
	if (out > in) {
		size = Buffer->Size - (out - in);
	}
	
	/* Return number of elements in buffer */
	return size;
}

void TM_BUFFER_Reset(TM_BUFFER_t* Buffer) {
	/* Check buffer structure */
	if (Buffer == NULL) {
		return;
	}
	
	/* Reset values */
	Buffer->In = 0;
	Buffer->Out = 0;
}

int32_t TM_BUFFER_FindElement(TM_BUFFER_t* Buffer, uint8_t Element) {
	uint32_t Num, Out, retval = 0;
	
	/* Check buffer structure */
	if (Buffer == NULL) {
		return -1;
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
			return retval;
		}
		
		/* Set new variables */
		Out++;
		Num--;
		retval++;
	}
	
	/* Element is not in buffer */
	return -1;
}

int32_t TM_BUFFER_Find(TM_BUFFER_t* Buffer, uint8_t* Data, uint32_t Size) {
	uint32_t Num, Out, i, retval = 0;
	uint8_t found = 0;

	/* Check buffer structure and number of elements in buffer */
	if (Buffer == NULL || (Num = TM_BUFFER_GetFull(Buffer)) < Size) {
		return -1;
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
	return -1;
}

uint32_t TM_BUFFER_WriteString(TM_BUFFER_t* Buffer, char* buff) {
	/* Write string to buffer */
	return TM_BUFFER_Write(Buffer, (uint8_t *)buff, strlen(buff));
}

uint32_t TM_BUFFER_ReadString(TM_BUFFER_t* Buffer, char* buff, uint32_t buffsize) {
	uint32_t i = 0;
	uint8_t ch;
	uint32_t freeMem;
	
	/* Check value buffer */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Get free */
	freeMem = TM_BUFFER_GetFree(Buffer);
	
	/* Check for any data on USART */
	if (
		freeMem == 0 ||                                                   /*!< Buffer empty */
		(
			TM_BUFFER_FindElement(Buffer, Buffer->StringDelimiter) < 0 && /*!< String delimiter is not in buffer */
			freeMem != 0 &&                                               /*!< Buffer is not full */
			TM_BUFFER_GetFull(Buffer) < buffsize                          /*!< User buffer size is larger than number of elements in buffer */
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

int8_t TM_BUFFER_CheckElement(TM_BUFFER_t* Buffer, uint32_t pos, uint8_t* element) {
	uint32_t In, Out, i = 0;
	
	/* Check value buffer */
	if (Buffer == NULL) {
		return 0;
	}
	
	/* Read current values */
	In = Buffer->In;
	Out = Buffer->Out;
	
	/* Set pointers to right location */
	while (i < pos && (In != Out)) {
		/* Increase output pointer */
		Out++;
		i++;
		
		/* Check overflow */
		if (Out >= Buffer->Size) {
			Out = 0;
		}
	}
	
	/* If positions match */
	if (i == pos) {
		/* Save element */
		*element = Buffer->Buffer[Out];
		
		/* Return OK */
		return 1;
	}
	
	/* Return zero */
	return 0;
}
