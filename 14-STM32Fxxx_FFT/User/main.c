/**
 * Keil project example for FFT and LCD
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen MAJERLE
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required *
 *	Notes: 
 *		- Under "Options for target" > "C/C++" > "Define" you must add 2 defines (I've already add them):
 *			- ARM_MATH_CM4 or ARM_MATH_CM7
 *			- __FPU_PRESENT=1
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_fft.h"
#include "tm_stm32_lcd.h"

/* Include arm_math.h mathematic functions */
#include "arm_math.h"

/* FFT settings */
#define SAMPLES					(512)         /* 256 real party and 256 imaginary parts */
#define FFT_SIZE				(SAMPLES / 2) /* FFT size is always the same size as we have samples, so 256 in our case */

#define FFT_BAR_MAX_HEIGHT		120           /* 120 px on the LCD */

/* Global variables */
float32_t Input[SAMPLES];   /*!< Input buffer is always 2 * FFT_SIZE */
float32_t Output[FFT_SIZE]; /*!< Output buffer is always FFT_SIZE */

/*!< FFT structure */
TM_FFT_F32_t FFT;

/* Temporary sinus value */
float32_t sin_val;

/* Draw bar for LCD */
/* Simple library to draw bars */
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground, uint16_t background);

int main(void) {
	uint32_t i = 0, freq = 0;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();	
	
	/* Init LCD */
	TM_LCD_Init();

#if defined(STM32F429_DISCOVERY)
	/* Rotate LCD to landscape mode */
	TM_LCD_SetOrientation(2);
#endif
	
	/* Print on LCD */
	TM_LCD_SetXY(10, 10);
	TM_LCD_Puts("FFT Library example");
	TM_LCD_SetXY(10, 30);
	TM_LCD_Puts("with software generated");
	TM_LCD_SetXY(10, 50);
	TM_LCD_Puts("SINE wave");
	
	/* Print "logo" */
	TM_LCD_SetFont(&TM_Font_7x10);
	TM_LCD_SetXY(30, TM_LCD_GetHeight() - 15);
	TM_LCD_Puts("stm32f4-discovery.net");
	
	/* Init FFT, FFT_SIZE define is used for FFT_SIZE, samples count is FFT_SIZE * 2, don't use malloc for memory allocation */
	TM_FFT_Init_F32(&FFT, FFT_SIZE, 0);
	
	/* We didn't used malloc for allocation, so we have to set pointers ourself */
	/* Input buffer must be 2 * FFT_SIZE in length because of real and imaginary part */
	/* Output buffer must be FFT_SIZE in length */
	TM_FFT_SetBuffers_F32(&FFT, Input, Output);
	
	while (1) {
		/* Let's fake sinus signal with 5, 15 and 30Hz frequencies */
		do {
			/* Calculate sinus value */
			sin_val = 0;
			sin_val += (float)0.5 * (float)sin((float)2 * (float)3.14159265359 * (float)1 * (float)freq * (float)i / (float)(FFT_SIZE / 2));
			sin_val += (float)0.3 * (float)sin((float)2 * (float)3.14159265359 * (float)2 * (float)freq * (float)i / (float)(FFT_SIZE / 2));
			sin_val += (float)0.1 * (float)sin((float)2 * (float)3.14159265359 * (float)3 * (float)freq * (float)i / (float)(FFT_SIZE / 2));
			
			i++;
		} while (!TM_FFT_AddToBuffer(&FFT, sin_val));
			
		/* Do FFT on signal, values at each bin and calculate max value and index where max value happened */
		TM_FFT_Process_F32(&FFT);

		/* Display data on LCD, only single sided spectrum of FFT */
		for (i = 0; i < (TM_FFT_GetFFTSize(&FFT) / 2); i++) {
			/* Draw FFT results */
			DrawBar(30 + 2 * i,
					TM_LCD_GetHeight() - 30,
					FFT_BAR_MAX_HEIGHT,
					TM_FFT_GetMaxValue(&FFT),
					TM_FFT_GetFromBuffer(&FFT, i),
					0x1234,
					0xFFFF
			);
		}
		
		/* Increase frequency */
		freq++;
		
		/* Check value */
		if (freq > 100) {
			freq = 0;
		}
		
		/* Little delay */
		Delayms(50);
	}
}

/* Draw bar for LCD */
/* Simple library to draw bars */
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground, uint16_t background) {
	uint16_t height;
	value++;
	height = (uint16_t)((float32_t)value / (float32_t)maxValue * (float32_t)maxHeight);
	if (height == maxHeight) {
		TM_LCD_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
	} else if (height < maxHeight) {
		TM_LCD_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
		TM_LCD_DrawLine(bottomX, bottomY - height, bottomX, bottomY - maxHeight, background);
	}
}
