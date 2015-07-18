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
			/* At least one is detected */
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
