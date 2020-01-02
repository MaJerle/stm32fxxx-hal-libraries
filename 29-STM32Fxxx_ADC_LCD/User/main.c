/**
 * Keil project example for Analog to Digital converter and LCD display with result
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen MAJERLE
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
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
#include "tm_stm32_adc.h"
#include "tm_stm32_lcd.h"

/* Result value */
uint16_t result;

/* Array for string */
char str[50];

int main(void) {	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Fill with background */
	TM_LCD_Fill(LCD_COLOR_BLUE2);
	
	/* Init ADC1 channel 1 = PA1 */
	TM_ADC_Init(ADC1, TM_ADC_Channel_1);
		
	/* Set LCD position and font */
	TM_LCD_SetXY(10, 10);
	TM_LCD_SetFont(&TM_Font_11x18);
	
	/* Set string */
	TM_LCD_Puts("ADC for STM32Fxxx");
	
	/* Set LCD colors */
	TM_LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLUE2);
	
	/* Set new font */
	TM_LCD_SetFont(&TM_Font_7x10);
	
	while (1) {
		/* Read values from ADC, 12 bit result is returned */
		result = TM_ADC_Read(ADC1, TM_ADC_Channel_1);
		
		/* Set LCD position and font */
		TM_LCD_SetXY(10, 40);
		
		/* Format string */
		sprintf(str, "ADC: 1, Ch: 1 = PA5; Value: %4d\n", result);
		
		/* Print string */
		TM_LCD_Puts(str);
	}
}

