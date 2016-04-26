/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
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
