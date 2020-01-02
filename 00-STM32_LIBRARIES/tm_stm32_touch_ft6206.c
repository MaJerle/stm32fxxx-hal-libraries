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
#include "tm_stm32_touch_ft6206.h"

/* Status register */
#define FT6206_STATUS_REG        0x02

/* Start locations for reading pressed touches */
static uint8_t FT6206_DataRegs[] = {0x03, 0x09, 0x0F, 0x15, 0x1B};

/* Driver functions */
TM_TOUCH_DRIVER_t TOUCH_DRIVER_FT6206 = {
    TM_TOUCH_FT6206_Init,
    TM_TOUCH_FT6206_Read
};

/* Delay function */
static void FT_Delay(__IO uint32_t d) {
	while (d--);
}

uint8_t TM_TOUCH_FT6206_Init(TM_TOUCH_t* TS) {
	uint8_t deviceID;
	
	/* Set max values */
	TS->MaxX = 799;
	TS->MaxY = 479;
	
	/* Init I2C */
	TM_I2C_Init(TOUCH_FT6206_I2C, TOUCH_FT6206_I2C_PP, 100000);
	
	/* Delay */
	FT_Delay(0xFFFFF);
	
	/* Check if device is connected */
    if (TM_I2C_IsDeviceConnected(TOUCH_FT6206_I2C, TOUCH_FT6206_I2C_DEV) != TM_I2C_Result_Ok) {
        return 1;
    }
	
	/* Check device ID */
	TM_I2C_Read(TOUCH_FT6206_I2C, TOUCH_FT6206_I2C_DEV, 0xA8, &deviceID);
	
	/* Check if OK */
	if (deviceID != 0x11) {
		/* Connected device is not FT6206 */
		return 2;
	}
    
    /* Enable touch interrupts */
    TM_I2C_Write(TOUCH_FT6206_I2C, TOUCH_FT6206_I2C_DEV, 0xA4, 0x01);
	
	/* Return 0 = OK */
	return 0;
}

uint8_t TM_TOUCH_FT6206_Read(TM_TOUCH_t* TS) {
	uint8_t status;
	uint8_t i;
	uint8_t DataRead[4];
	
	/* Reset */
	TS->NumPresses = 0;
	
	/* Check status */
	TM_I2C_Read(TOUCH_FT6206_I2C, TOUCH_FT6206_I2C_DEV, 0x02, &status);
	
	/* Mask status register */
	status &= 0x0F;
	
	/* Check if max detected more than max number of contacts */
	if (status > 5) {
		/* Return ERROR */
		return 1;
	}
	
	/* Save number of touches detected */
	TS->NumPresses = status;
	
	/* Read all positions */
	for (i = 0; i < TS->NumPresses; i++) {       
		/* Read 4 bytes in a row */
		TM_I2C_ReadMulti(TOUCH_FT6206_I2C, TOUCH_FT6206_I2C_DEV, FT6206_DataRegs[i], DataRead, 4);
		
		/* Format touches */
		TS->Y[i] = (DataRead[1]) | ((DataRead[0] & 0x0F) << 8);
		TS->X[i] = (DataRead[3]) | ((DataRead[2] & 0x0F) << 8);
	}
    
//    /* Read gesture */
//    if (TM_I2C_Read(TOUCH_FT5336_I2C, TOUCH_FT5336_I2C_DEV, 0x01, &TS->Gesture) == TM_I2C_Result_Ok) {
//        TS->Gesture = TS->Gesture;
//    }
	
	/* Return OK */
	return 0;
}
