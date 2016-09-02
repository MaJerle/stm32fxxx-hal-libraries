/**
 * Keil project example for LCD 
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
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

/* Font size structure */
TM_FONT_SIZE_t FontSize;

/* String to draw on LCD */
#if defined(STM32F7_DISCOVERY)
char str[] = "This is LCD driver for F7-Discovery board";
#elif defined(STM32F439_EVAL)
char str[] = "This is LCD driver for F439-Eval board";
#elif defined(STM32F429_DISCOVERY)
char str[] = "F429-Discovery board";
#endif

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Fill LCD with color */
	TM_LCD_Fill(0x4321);
	
	/* Put string on the middle of LCD */
	TM_LCD_SetFont(&TM_Font_11x18);
	
	/* Get width and height of string */
	TM_FONT_GetStringSize(str, &FontSize, &TM_Font_11x18);
	
	/* Calculate middle of LCD */
	TM_LCD_SetXY((TM_LCD_GetWidth() - FontSize.Width) / 2, (TM_LCD_GetHeight() - FontSize.Height) / 2);
	
	/* Put string to LCD */
	TM_LCD_Puts(str);
	
	while (1) {
		/* Do nothing */
	}
}

