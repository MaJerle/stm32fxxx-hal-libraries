/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen MAJERLE
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
#include "tm_stm32_touch.h"
#include "string.h"

/* Check for defines */
#if defined(STM32F439_EVAL) || defined(TOUCH_USE_STM32F439_EVAL)
#include "tm_stm32_touch_ts3510.h"
#endif
#if defined(STM32F7_DISCOVERY) || defined(TOUCH_USE_STM32F7_DISCOVERY)
#include "tm_stm32_touch_ft5336.h"
#endif
#if defined(STM32F769_DISCOVERY) || defined(TOUCH_USE_STM32F769_DISCOVERY)
#include "tm_stm32_touch_ft6206.h"
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
#elif defined(STM32F769_DISCOVERY) || defined(TOUCH_USE_STM32F769_DISCOVERY)
		TouchDriver.Init = TM_TOUCH_FT6206_Init;
		TouchDriver.Read = TM_TOUCH_FT6206_Read;
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
    
    /* Check if anything to do */
    if (TS->Orientation != TOUCH_ORIENT_DEFAULT) {
        for (i = 0; i < TS->NumPresses; i++) {
            /* Invert X values */
            if (TS->Orientation & TOUCH_ORIENT_INVERT_X) {
                TS->X[i] = TS->MaxX - TS->X[i];
            }
            /* Invert Y values */
            if (TS->Orientation & TOUCH_ORIENT_INVERT_Y) {
                TS->Y[i] = TS->MaxY - TS->Y[i];
            }
			/* Swap X and Y values */
            if (TS->Orientation & TOUCH_ORIENT_SWAP) {
                tmp = TS->X[i];
                TS->X[i] = TS->Y[i];
                TS->Y[i] = tmp;
            }
		}
    }
	
	/* Return OK */
	return 0;
}
