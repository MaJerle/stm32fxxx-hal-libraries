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
#include "tm_stm32_iwdg.h"

uint8_t TM_IWDG_Init(TM_IWDG_Timeout_t timeout) {
	uint8_t result = 0;
	uint16_t reload = 0;
	
	/* Check if the system has resumed from IWDG reset */
#if defined(STM32F4xx)
	if (RCC->CSR & RCC_CSR_WDGRSTF) {
#else
	if (RCC->CSR & RCC_CSR_IWDGRSTF) {	
#endif
		/* Reset by IWDG */
		result = 1;
		
		/* Clear reset flags */
		RCC->CSR |= RCC_CSR_RMVF;
	}

	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG->KR = 0x5555;

	/* Set proper clock depending on timeout user select */
	if (timeout >= TM_IWDG_Timeout_8s) {
		/* IWDG counter clock: LSI/256 = 128Hz */
		IWDG->PR = 0x07;
	} else {
		/* IWDG counter clock: LSI/32 = 1024Hz */
		IWDG->PR = 0x03;
	}
	
	/* Set counter reload value */
	if (timeout == TM_IWDG_Timeout_5ms) {
		reload = 5; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_10ms) {
		reload = 10; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_15ms) {
		reload = 15; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_30ms) {
		reload = 31; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_60ms) {
		reload = 61; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_120ms) {
		reload = 123; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_250ms) {
		reload = 255; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_500ms) {
		reload = 511; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_1s) {
		reload = 1023; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_2s) {
		reload = 2047; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_4s) {
		reload = 4095; /* 1024 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_8s) {
		reload = 1023; /* 128 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_16s) {
		reload = 2047; /* 128 Hz IWDG ticking */
	} else if (timeout == TM_IWDG_Timeout_32s) {
		reload = 4095; /* 128 Hz IWDG ticking */
	}
	
	/* Set reload */
	IWDG->RLR = reload;

	/* Reload IWDG counter */
	IWDG->KR = 0xAAAA;

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG->KR = 0xCCCC;
	
	/* Return status */
	return result;
}
