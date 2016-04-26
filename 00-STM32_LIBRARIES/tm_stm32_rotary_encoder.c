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
#include "tm_stm32_rotary_encoder.h"

/* Return with status macro */
#define RETURN_WITH_STATUS(p, s)    do { (p)->Rotation = s; return s; } while (0)

void TM_RE_Init(TM_RE_t* data, GPIO_TypeDef* GPIO_A_Port, uint16_t GPIO_A_Pin, GPIO_TypeDef* GPIO_B_Port, uint16_t GPIO_B_Pin) {
	/* Save parameters */
	data->GPIO_A = GPIO_A_Port;
	data->GPIO_B = GPIO_B_Port;
	data->GPIO_PIN_A = GPIO_A_Pin;
	data->GPIO_PIN_B = GPIO_B_Pin;
	
	/* Set pin A as exti interrupt */
	TM_EXTI_Attach(data->GPIO_A, data->GPIO_PIN_A, TM_EXTI_Trigger_Rising_Falling);
	
	/* Set pin B as input */
	TM_GPIO_Init(data->GPIO_B, data->GPIO_PIN_B, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
	
	/* Set default mode */
	data->Mode = TM_RE_Mode_Zero;
	
	/* Set default */
	data->RE_Count = 0;
	data->Diff = 0;
	data->Absolute = 0;
	data->LastA = 1;
}

TM_RE_Rotate_t TM_RE_Get(TM_RE_t* data) {
	/* Calculate everything */
	data->Diff = data->RE_Count - data->Absolute;
	data->Absolute = data->RE_Count;
	
	/* Check */
	if (data->RE_Count < 0) {
		RETURN_WITH_STATUS(data, TM_RE_Rotate_Decrement);
	} else if (data->RE_Count > 0) {
		RETURN_WITH_STATUS(data, TM_RE_Rotate_Increment);
	}
	
	RETURN_WITH_STATUS(data, TM_RE_Rotate_Nothing);
}

void TM_RE_SetMode(TM_RE_t* data, TM_RE_Mode_t mode) {
	/* Set mode */
	data->Mode = mode;
}

void TM_RE_Process(TM_RE_t* data) {
	uint8_t now_a;
	uint8_t now_b;
	
	/* Read inputs */
	now_a = TM_GPIO_GetInputPinValue(data->GPIO_A, data->GPIO_PIN_A);
	now_b = TM_GPIO_GetInputPinValue(data->GPIO_B, data->GPIO_PIN_B);
	
	/* Check difference */
	if (now_a != data->LastA) {
		data->LastA = now_a;
		
		if (data->LastA == 0) {
			/* Check mode */
			if (data->Mode == TM_RE_Mode_Zero) {
				if (now_b == 1) {
					data->RE_Count--;
				} else {
					data->RE_Count++;
				}
			} else {
				if (now_b == 1) {
					data->RE_Count++;
				} else {
					data->RE_Count--;
				}
			}
		}
	}
}

