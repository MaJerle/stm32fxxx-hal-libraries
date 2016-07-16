/**
 * Keil project example for DMA library
 *
 * This example works for STM32F4xx and STM32F7xx devices only!
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_lcd.h"
#include "tm_stm32_filters.h"

#define SAMPLING_FREQ      100.0f /* We virtually have 100 samples per second */
#define SIN_FREQ           1.1f
#define SIN_NOISE_FREQ     30.0f

float32_t Data[480];     /* 480 sample elements, at 100Hz gives us 4.8 seconds */

/* Coefficients for FIR filter */
/**
 * Coefficients for FIR filter.
 *
 * Generated using Filter design and analysis tool inside Matlab.
 *
 * Type: FIR, Window
 * Window: Chebysev
 * Sampling frequency Fs = 100
 * Cutoff frequency Fc = 2
 * Filter order: 50
 */
const int FIRFilter_CoeffsSize = 51;
const float32_t FIRFilter_Coeffs[51] = {
	9.717897974e-22,2.968381978e-06,1.442271696e-05,4.535880362e-05, 0.000114576811,
	0.000251264195, 0.0004972499446, 0.000908364018,  0.00155428343, 0.002516295528,
	0.003882582532, 0.005740928929, 0.008169140667,  0.01122390758,  0.01492925081,
	0.01926600747,  0.02416394092,  0.02949799597,  0.03508982062,  0.04071519896,
	0.0461172536,   0.05102449656,  0.05517212674,  0.05832434818,  0.06029526517,
	0.0609658882,   0.06029526517,  0.05832434818,  0.05517212674,  0.05102449656,
	0.0461172536,   0.04071519896,  0.03508982062,  0.02949799597,  0.02416394092,
	0.01926600747,  0.01492925081,  0.01122390758, 0.008169140667, 0.005740928929,
	0.003882582532, 0.002516295528,  0.00155428343, 0.000908364018,0.0004972499446,
	0.000251264195, 0.000114576811,4.535880362e-05,1.442271696e-05,2.968381978e-06,
	9.717897974e-22
};

/* IIR coeficients for HP filter */
const float32_t IIRFilter_Coeffs[5] = {
	1.0, -1.0, 0, 0.95, 0
};

/* Private functions */
static void LCD_DrawSignal(float32_t* InputArray, size_t size, uint16_t color);

/* Filter instances */
TM_FILTER_FIR_F32_t* FIR;
TM_FILTER_IIR_F32_t* IIR;

int main(void) {
	uint16_t i;	
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Draw on LCD */
	TM_LCD_Puts("FIR AND IIR Filter example");
	
	/* Straight line in the middle */
	TM_LCD_DrawLine(0, TM_LCD_GetHeight() / 2, TM_LCD_GetWidth(), TM_LCD_GetHeight() / 2, LCD_COLOR_BLACK);

	/*****************************************/
	/* Create sinus signal                   */
	/*****************************************/	
	/* Create sinus signal with high frequency on it */
	for (i = 0; i < (sizeof(Data) / sizeof(Data[0])); i++) {
		/* Set offset to signal, we will remove it later with IIR filter (highpass) */
		Data[i] = 3;
		/* Base frequency we are satisfied with */
		Data[i] -= 5.0f * sin(i * 2.0f * SIN_FREQ * 3.1415f / SAMPLING_FREQ);
		/* High frequency noise we want to remove later with FIR filter (lowpass) */
		Data[i] -= 1.5f * sin(i * 2.0f * SIN_NOISE_FREQ * 3.1415f / SAMPLING_FREQ);
	}
	
	/* Draw base signal with high frequency signal */
	LCD_DrawSignal(Data, sizeof(Data) / sizeof(Data[0]), LCD_COLOR_BLACK);
	
	/*****************************************/
	/* IIR FILTER process                    */
	/*****************************************/
	/* Create IIR filter object, let malloc do the job with states */
	/* We will use highpass filtering for IIR to remove base offset in signal */
	IIR = TM_FILTER_IIR_F32_Init(1, IIRFilter_Coeffs, NULL, 1);
	
	/* Filter initialized, we have enough memory */
	if (IIR != NULL) {
		/* Process filtering */
		TM_FILTER_IIR_F32_ProcessAll(IIR, Data, Data, sizeof(Data) / sizeof(Data[0]));
		
		/* Draw output signal */
		LCD_DrawSignal(Data, sizeof(Data) / sizeof(Data[0]), LCD_COLOR_RED);
	} else {
		TM_LCD_SetXY(0, 20);
		TM_LCD_Puts("IIR Init failed! Increase heap memory!");
	}

	/*****************************************/
	/* FIR FILTER process                    */
	/*****************************************/	
	/* Create FIR filter object, let malloc do the job with states buffer */
	/* We will add 1 sample per filter call at a time */
	/* Fir is used for low-pass filtering process */
	FIR = TM_FILTER_FIR_F32_Init(FIRFilter_CoeffsSize, FIRFilter_Coeffs, NULL, 1);
	
	/* Filter initialized, we have enough memory */
	if (FIR != NULL) {
		/* Process filtering */
		TM_FILTER_FIR_F32_ProcessAll(FIR, Data, Data, sizeof(Data) / sizeof(Data[0]));
		
		/* Draw output signal */
		LCD_DrawSignal(Data, sizeof(Data) / sizeof(Data[0]), LCD_COLOR_BLUE);
	} else {
		TM_LCD_SetXY(0, 40);
		TM_LCD_Puts("FIR Init failed! Increase heap memory!");
	}
	
	/* Do nothing */
	while (1) {
		
	}
}

/* Draws graph to LCD */
static void LCD_DrawSignal(float32_t* InputArray, size_t size, uint16_t color) {
	float32_t sx, sy, nx, ny;
	uint16_t i;
	
	sx = 0, sy = TM_LCD_GetHeight() / 2;
	for (i = 0; i < size; i++) {
		nx = i;
		ny = InputArray[i] * TM_LCD_GetHeight() / 30 + TM_LCD_GetHeight() / 2;
		TM_LCD_DrawLine(sx, sy, nx, ny,  color);
		
		sx = nx;
		sy = ny;
	}
}
