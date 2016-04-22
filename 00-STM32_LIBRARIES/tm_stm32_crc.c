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
#include "tm_stm32_crc.h"

void TM_CRC_Init(void) {
	/* Enable CRC clock */
	__HAL_RCC_CRC_CLK_ENABLE();
}

void TM_CRC_DeInit(void) {
	/* Disable CRC clock */
	__HAL_RCC_CRC_CLK_DISABLE();
}

uint32_t TM_CRC_Calculate8(uint8_t* arr, uint32_t count, uint8_t reset) {
	uint32_t cnt;
	
	/* Reset CRC data register if necessary */
	if (reset) {
		/* Reset generator */
		CRC->CR = CRC_CR_RESET;
	}
	
	/* Calculate number of 32-bit blocks */
	cnt = count >> 2;
	
	/* Calculate */
	while (cnt--) {
		/* Set new value */
		CRC->DR = *(uint32_t *)arr;
		
		/* Increase by 4 */
		arr += 4;
	}
	
	/* Calculate remaining data as 8-bit */
	cnt = count % 4;
	
	/* Calculate */
	while (cnt--) {
		/* Set new value */
		*((uint8_t *)&CRC->DR) = *arr++;
	}
	
	/* Return data */
	return CRC->DR;
}

uint32_t TM_CRC_Calculate16(uint16_t* arr, uint32_t count, uint8_t reset) {
	uint32_t cnt;
	
	/* Reset CRC data register if necessary */
	if (reset) {
		/* Reset generator */
		CRC->CR = CRC_CR_RESET;
	}
	
	/* Calculate number of 32-bit blocks */
	cnt = count >> 1;
	
	/* Calculate */
	while (cnt--) {
		/* Set new value */
		CRC->DR = *(uint32_t *)arr;
		
		/* Increase by 2 */
		arr += 2;
	}
	
	/* Calculate remaining data as 8-bit */
	cnt = count % 4;
	
	/* Calculate */
	while (cnt--) {
		/* Set new value */
		*((uint8_t *)&CRC->DR) = *arr++;
	}
	
	/* Return data */
	return CRC->DR;
}

uint32_t TM_CRC_Calculate32(uint32_t* arr, uint32_t count, uint8_t reset) {
	/* Reset CRC data register if necessary */
	if (reset) {
		/* Reset generator */
		CRC->CR = CRC_CR_RESET;
	}
	
	/* Calculate CRC */
	while (count--) {
		/* Set new value */
		CRC->DR = *arr++;
	}
	
	/* Return data */
	return CRC->DR;
}
