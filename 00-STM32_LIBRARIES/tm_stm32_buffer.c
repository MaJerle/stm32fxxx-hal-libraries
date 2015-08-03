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

uint8_t TM_BUFFER_Init(TM_BUFFER_t* Buffer, uint16_t Size, uint8_t* BufferPtr, uint8_t UseMalloc) {
	/* Set default values */
	Buffer->In = 0;
	Buffer->Out = 0;
	Buffer->Num = 0;
	Buffer->Size = Size;
	Buffer->Flags = 0;
	Buffer->Buffer = BufferPtr;
	
	/* Check if malloc is used */
	if (UseMalloc) {
		/* Try to allocate */
		Buffer->Buffer = (uint8_t *)malloc(Size * sizeof(uint8_t));
		
		/* Check if allocated */
		if (Buffer->Buffer == NULL) {
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
	/* If malloc was used for allocation */
	if (Buffer->Flags & BUFFER_MALLOC) {
		/* Free memory */
		free(Buffer->Buffer);
	}
	
	/* Clear flag */
	Buffer->Flags = 0;
	Buffer->Size = 0;
}

uint16_t TM_BUFFER_Write(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t count) {
	uint16_t i;
	
	/* Go through all elements */
	for (i = 0; i < count; i++) {
		/* Check if memory available */
		if (Buffer->Num < Buffer->Size) {
			/* Check input overflow */
			if (Buffer->In >= Buffer->Size) {
				Buffer->In = 0;
			}
			
			/* Add to buffer */
			Buffer->Buffer[Buffer->In] = *Data++;
			
			/* Increase pointers */
			Buffer->In++;
			Buffer->Num++;
		} else {
			break;
		}
	}
	
	/* Return number of elements stored in memory */
	return i;
}

uint16_t TM_BUFFER_Read(TM_BUFFER_t* Buffer, uint8_t* Data, uint16_t count) {
	uint16_t i;
	
	/* Go through all elements */
	for (i = 0; i < count; i++) {
		/* Check if memory available */
		if (Buffer->Num > 0) {
			/* Check output overflow */
			if (Buffer->Out >= Buffer->Size) {
				Buffer->Out = 0;
			}
			
			/* Save to user buffer */
			*Data++ = Buffer->Buffer[Buffer->Out];
			
			/* Increase pointers */
			Buffer->Out++;
			Buffer->Num--;
		} else {
			break;
		}
	}
	
	/* Return number of elements read from buffer */
	return i;
}

uint16_t TM_BUFFER_GetFree(TM_BUFFER_t* Buffer) {
	/* Return number of free memory in buffer */
	return (Buffer->Size - Buffer->Num);
}

uint16_t TM_BUFFER_GetFull(TM_BUFFER_t* Buffer) {
	/* Return number of elements in buffer */
	return Buffer->Num;
}

void TM_BUFFER_Reset(TM_BUFFER_t* Buffer) {
	/* Reset values */
	Buffer->In = 0;
	Buffer->Out = 0;
	Buffer->Num = 0;
}

uint8_t TM_BUFFER_FindElement(TM_BUFFER_t* Buffer, uint8_t Element) {
	uint16_t Num, Out;
	
	/* Create temporary variables */
	Num = Buffer->Num;
	Out = Buffer->Out;
	
	while (Num > 0) {
		/* Check output overflow */
		if (Out == Buffer->Size) {
			Out = 0;
		}
		
		/* Check for character */
		if ((uint8_t)Buffer->Buffer[Out] == (uint8_t)Element) {
			/* Character found */
			return 1;
		}
		
		/* Set new variables */
		Out++;
		Num--;
	}
	
	/* Character is not in buffer */
	return 0;
}
