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
#include "tm_stm32_touch.h"
#include "string.h"

/* Check for defines */
#if defined(STM32F439_EVAL) || defined(TOUCH_USE_STM32F439_EVAL)
#include "tm_stm32_touch_ts3510.h"
#endif
#if defined(STM32F7_DISCOVERY) || defined(TOUCH_USE_STM32F7_DISCOVERY)
#include "tm_stm32_touch_ft5336.h"
#endif

/* Static driver structure */
static TM_TOUCH_DRIVER_t TouchDriver;

TM_TOUCH_Result_t TM_TOUCH_Init(TM_TOUCH_DRIVER_t* Driver, TM_TOUCH_t* TS) {
	/* Set memory to zero */
	memset((uint8_t *)&TouchDriver, 0, sizeof(TM_TOUCH_DRIVER_t));

	/* Check for default driver */
	if (Driver != NULL) {
		TouchDriver.Init = Driver->Init;
		TouchDriver.Read = Driver->Read;
	} else {
		/* Set default values */
#if defined(STM32F439_EVAL) || defined(TOUCH_USE_STM32F439_EVAL)
		TouchDriver.Init = TM_TOUCH_TS3510_Init;
		TouchDriver.Read = TM_TOUCH_TS3510_Read;
#elif defined(STM32F7_DISCOVERY) || defined(TOUCH_USE_STM32F7_DISCOVERY)
		TouchDriver.Init = TM_TOUCH_FT5336_Init;
		TouchDriver.Read = TM_TOUCH_FT5336_Read;
#else
		/* Return error, no default drivers available */
		return TM_TOUCH_Result_Error;
#endif
	}
	
	/* Try to INIT */
	if (TouchDriver.Init(TS) == 0) {
		/* Return OK */
		return TM_TOUCH_Result_Ok;
	}
	
	/* Return ERROR */
	return TM_TOUCH_Result_Error;
}

uint8_t TM_TOUCH_Read(TM_TOUCH_t* TS) {
	uint8_t status, i;
	uint16_t tmp;
	
	/* Read touch values from sensor */
	status = TouchDriver.Read(TS);
	
	/* Return touch status */
	if (status) {
		return status;
	}
	
	/* Check for orientations */
	if (TS->Orientation == 0) {
		/* Rotate all X and Y values */
		for (i = 0; i < TS->NumPresses; i++) {
			TS->X[i] = TS->MaxX - TS->X[i];
			TS->Y[i] = TS->MaxY - TS->Y[i];
		}
	}
	if (TS->Orientation == 2) {
		/* Rotate all X and Y values */
		for (i = 0; i < TS->NumPresses; i++) {
			tmp = TS->X[i];
			TS->X[i] = TS->MaxY - TS->Y[i];
			TS->Y[i] = tmp;
		}
	}
	if (TS->Orientation == 3) {
		/* Rotate all X and Y values */
		for (i = 0; i < TS->NumPresses; i++) {
			tmp = TS->X[i];
			TS->X[i] = TS->Y[i];
			TS->Y[i] = TS->MaxX - tmp;
		}
	}
	
	
	/* Return OK */
	return 0;
}
