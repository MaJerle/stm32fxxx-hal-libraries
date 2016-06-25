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
#include "tm_stm32_am2301.h"

/* Private functions */
static TM_AM2301_Result_t TM_AM2301_INT_Read(TM_AM2301_t* data);

/* Private defines */
#define AM2301_PIN_LOW(str)     TM_GPIO_SetPinLow((str)->GPIOx, (str)->GPIO_Pin)
#define AM2301_PIN_HIGH(str)    TM_GPIO_SetPinHigh((str)->GPIOx, (str)->GPIO_Pin)
#define AM2301_PIN_IN(str)      TM_GPIO_SetPinAsInput((str)->GPIOx, (str)->GPIO_Pin)
#define AM2301_PIN_OUT(str)     TM_GPIO_SetPinAsOutput((str)->GPIOx, (str)->GPIO_Pin)
#define AM2301_PIN_READ(str)    TM_GPIO_GetInputPinValue((str)->GPIOx, (str)->GPIO_Pin)

TM_AM2301_Result_t TM_AM2301_Init(TM_AM2301_t* AMStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	/* Initialize delay */
	TM_DELAY_Init();
	
	/* Initialize pin */
	TM_GPIO_Init(GPIOx, GPIO_Pin, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	
	/* Save settings */
	AMStruct->GPIOx = GPIOx;
	AMStruct->GPIO_Pin = GPIO_Pin;
	
	/* Return OK */
	return TM_AM2301_Result_Ok;
}

TM_AM2301_Result_t TM_AM2301_Read(TM_AM2301_t* AMStruct) {
	/* Read data */
	return TM_AM2301_INT_Read(AMStruct);
}

/* Internal function */
TM_AM2301_Result_t TM_AM2301_INT_Read(TM_AM2301_t* data) {
	volatile uint32_t time;
	uint8_t i, j, d[5];
	
	/* Pin output */
	AM2301_PIN_OUT(data);
	/* Set pin low for ~800-1000 us */
	AM2301_PIN_LOW(data);
	Delay(1000);
	/* Set pin high to ~30 us */
	AM2301_PIN_HIGH(data);
	Delay(30);
	
	/* Read mode */
	AM2301_PIN_IN(data);
	
	time = 0;
	/* Wait 20us for acknowledge, low signal */
	while (AM2301_PIN_READ(data)) {
		if (time > 20) {
			return TM_AM2301_Result_CONNECTION_ERROR;
		}
		/* Increase time */
		time++;
		/* Wait 1 us */
		Delay(1);
	}
	
	time = 0;
	/* Wait high signal, about 80-85us long (measured with logic analyzer) */
	while (!AM2301_PIN_READ(data)) {
		if (time > 85) {
			return TM_AM2301_Result_WAITHIGH_ERROR;
		}
		/* Increase time */
		time++;
		/* Wait 1 us */
		Delay(1);
	}
	
	time = 0;
	/* Wait low signal, about 80-85us long (measured with logic analyzer) */
	while (AM2301_PIN_READ(data)) {
		if (time > 85) {
			return TM_AM2301_Result_WAITLOW_ERROR;
		}
		/* Increase time */
		time++;
		/* Wait 1 us */
		Delay(1);
	}
	
	/* Read 5 bytes */
	for (j = 0; j < 5; j++) {
		d[j] = 0;
		for (i = 8; i > 0; i--) {
			/* We are in low signal now, wait for high signal and measure time */
			time = 0;
			/* Wait high signal, about 57-63us long (measured with logic analyzer) */
			while (!AM2301_PIN_READ(data)) {
				if (time > 75) {
					return TM_AM2301_Result_WAITHIGH_LOOP_ERROR;
				}
				/* Increase time */
				time++;
				/* Wait 1 us */
				Delay(1);
			}
			/* High signal detected, start measure high signal, it can be 26us for 0 or 70us for 1 */
			time = 0;
			/* Wait low signal, between 26 and 70us long (measured with logic analyzer) */
			while (AM2301_PIN_READ(data)) {
				if (time > 90) {
					return TM_AM2301_Result_WAITLOW_LOOP_ERROR;
				}
				/* Increase time */
				time++;
				/* Wait 1 us */
				Delay(1);
			}
			
			if (time > 18 && time < 37) {
				/* We read 0 */
			} else {
				/* We read 1 */
				d[j] |= 1 << (i - 1);
			}
		}
	}
	
	/* Check for parity */
	if (((d[0] + d[1] + d[2] + d[3]) & 0xFF) != d[4]) {
		/* Parity error, data not valid */
		return TM_AM2301_Result_PARITY_ERROR;
	}
	
	/* Set humidity */
	data->Hum = d[0] << 8 | d[1];
	/* Negative temperature */
	if (d[2] & 0x80) {
		data->Temp = -((d[2] & 0x7F) << 8 | d[3]);
	} else {
		data->Temp = (d[2]) << 8 | d[3];
	}
	
	/* Data OK */
	return TM_AM2301_Result_Ok;
}

