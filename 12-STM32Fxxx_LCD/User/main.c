/**
 * Keil project template
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
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init LCD */
	TM_LCD_Init();

	/* GO to layer 1 */
	TM_LCD_SetLayer1();
	
	/* Fill LCD with color */
	TM_LCD_Fill(0x1234);
	
	/* Go to layer 2 */
	TM_LCD_SetLayer2();
	
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
	
	/* Go back to layer 1 */
	TM_LCD_SetLayer1();
	
	/* Use DMA2D for test operation */
	TM_DMA2DGRAPHIC_DrawFilledRectangle(30, 30, 100, 50, 0x5555);
	
	/* Draw circle */
	TM_LCD_DrawLine(10, 10, TM_LCD_GetWidth() - 10, TM_LCD_GetHeight() - 10, 0x4567);
	
	while (1) {
		/* Delay */
		HAL_Delay(500);
		
		/* Change visible layers */
		//TM_LCD_ChangeLayers();
	}
}

