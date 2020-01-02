/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen MAJERLE
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
#include "tm_stm32_bor.h"

TM_BOR_Result_t TM_BOR_Set(TM_BOR_Level_t BORValue) {
	HAL_StatusTypeDef status;
	FLASH_OBProgramInitTypeDef FLASH_Handle;
	
	/* Check current BOR value */
	if (TM_BOR_Get() != BORValue) {
		/* Set new value */

		/* Select the desired V(BOR) Level */
		FLASH_Handle.BORLevel = (uint32_t)BORValue;
		FLASH_Handle.OptionType = OPTIONBYTE_BOR;

		/* Unlocks the option bytes block access */
		HAL_FLASH_OB_Unlock();
		
		/* Set value */
		HAL_FLASHEx_OBProgram(&FLASH_Handle); 

		/* Launch the option byte loading */
		status = HAL_FLASH_OB_Launch();
		
		/* Lock access to registers */
		HAL_FLASH_OB_Lock();
		
		/* Check success */
		if (status != HAL_OK) {
			/* Return error */
			return TM_BOR_Result_Error;
		}
	}
	
	/* Return OK */
	return TM_BOR_Result_Ok;
}

TM_BOR_Level_t TM_BOR_Get(void) {
	FLASH_OBProgramInitTypeDef FLASH_Handle;
	
	/* Read option bytes */
	HAL_FLASHEx_OBGetConfig(&FLASH_Handle);
	
	/* Return BOR value */
	return (TM_BOR_Level_t) FLASH_Handle.BORLevel;
}
