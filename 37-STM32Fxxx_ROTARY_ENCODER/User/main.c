/**
 * Keil project example for rotary encoder
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
#include "tm_stm32_lcd.h"
#include "tm_stm32_rotary_encoder.h"
	
/* Rotary encoder structures */
TM_RE_t RE1_Data, RE2_Data;
	
int main(void) {
	char buff[100];
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Set layer */
	TM_LCD_SetLayer1();
	
	/* Initialize Rotary encoder 1, pin A = pin from button on board, pin B = PD1 */
	TM_RE_Init(&RE1_Data, DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, GPIOD, GPIO_PIN_1);
	
	/* Initialize Rotary encoder 2, pin A = PD3, pin B = PD5 */
	TM_RE_Init(&RE2_Data, GPIOD, GPIO_PIN_3, GPIOD, GPIO_PIN_5);
	
	/* Set rotation mode for rotary 1 */
	TM_RE_SetMode(&RE1_Data, TM_RE_Mode_One);

	/* Set LCD font */
#if !defined(STM32F439_EVAL)
	TM_LCD_SetFont(&TM_Font_7x10);
#endif

	/* Set location */
	TM_LCD_SetXY(10, 10);

	/* Put string */
	TM_LCD_Puts("Press blue button to simulate rotary encoder!");

	while (1) {
		/* Get new rotation */
		TM_RE_Get(&RE1_Data);
		
		/* Format & display on LCD */
		sprintf(buff, "RE1: Absolute: %4d; Difference from last check: %3d", RE1_Data.Absolute, RE1_Data.Diff);
		
		/* Set LCD location */
		TM_LCD_SetXY(10, 100);
		
		/* Show on LCD */
		TM_LCD_Puts(buff);
		
		/* Get new rotation */
		TM_RE_Get(&RE2_Data);
		
		/* Format & display on LCD */
		sprintf(buff, "RE2: Absolute: %4d; Difference from last check: %3d", RE2_Data.Absolute, RE2_Data.Diff);
		
		/* Set LCD location */
		TM_LCD_SetXY(10, 150);
		
		/* Show on LCD */
		TM_LCD_Puts(buff);
	}
}

/* TM EXTI Handler for all EXTI lines */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Check RE pin 1 */
	if (GPIO_Pin == RE1_Data.GPIO_PIN_A) {
		/* Process data */
		TM_RE_Process(&RE1_Data);
	}
	if (GPIO_Pin == RE2_Data.GPIO_PIN_A) {
		/* Process data */
		TM_RE_Process(&RE2_Data);
	}
}

