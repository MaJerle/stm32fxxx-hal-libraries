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
