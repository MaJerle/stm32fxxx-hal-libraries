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
#include "tm_stm32_touch_ts3510.h"

uint8_t TM_TOUCH_TS3510_Init(TM_TOUCH_t* TS) {
	/* Init I2C */
	TM_I2C_Init(TOUCH_TS3510_I2C, TOUCH_TS3510_I2C_PP, 100000);
	
	/* Set max values */
	TS->MaxX = 639;
	TS->MaxY = 479;
	
	/* Return 0 = OK */
	return 0;
}

uint8_t TM_TOUCH_TS3510_Read(TM_TOUCH_t* TS) {
	uint8_t DataSend[] = {0x81, 0x08};
	uint8_t DataReceive[11];
	uint16_t X1, Y1, X2, Y2;
	
	/* Read data */
	TM_I2C_WriteReadRepeatedStart(
		TOUCH_TS3510_I2C,
		TOUCH_TS3510_I2C_DEV,
		0x00,
		DataSend,
		2,
		0x8A,
		DataReceive,
		11
	);
	
	/* Format data */
	X1 = DataReceive[1] << 8 | DataReceive[2];
	Y1 = DataReceive[3] << 8 | DataReceive[4];
	X2 = DataReceive[5] << 8 | DataReceive[6];
	Y2 = DataReceive[7] << 8 | DataReceive[8];
	
	/* Check values */
	if (X1 != 0xFFFF && Y1 != 0xFFFF) {
		/* At least one is detected */
		TS->NumPresses = 1;
		TS->X[0] = X1;
		TS->Y[0] = Y1;
		
		/* Check values */
		if (X2 != 0xFFFF && Y2 != 0xFFFF) {
			/* Second is also detected */
			TS->NumPresses++;
			TS->X[1] = X2;
			TS->Y[1] = Y2;	
		}
	} else {
		/* No press detected */
		TS->NumPresses = 0;
	}
	
	/* Everything OK */
	return 0;
}
