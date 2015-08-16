/**
 * Keil project example for TOUCH controllers
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
#include "tm_stm32_touch.h"
#include "tm_stm32_lcd.h"

/* Touch screen data structure */
TM_TOUCH_t TS;

/* For string drawings */
char str[200];

/* Colors used for drawing for touch pressed */
uint16_t TOUCH_Colors[10] = {
	LCD_COLOR_BLACK,
	LCD_COLOR_BLUE,
	LCD_COLOR_RED,
	LCD_COLOR_GREEN,
	LCD_COLOR_YELLOW
};

int main(void) {
	uint8_t i;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init delay functions */
	TM_DELAY_Init();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Fill LCD with color */
	TM_LCD_Fill(0xFFFF);
	
	/* Set custom orientation for LCD */
	TM_LCD_SetOrientation(1);
	
	/* Get orientation from LCD and save to Touch Screen structure */
	TS.Orientation = TM_LCD_GetOrientation();
	
	/* Init touch, use default drivers, depends on defines in library */
	/* Check library description for more information */
	TM_TOUCH_Init(NULL, &TS);
	
	while (1) {
		/* Read touch */
		TM_TOUCH_Read(&TS);
		
		/* Check if pressed */
		if (TS.NumPresses) {
			/* Go through all presses on LCD */
			for (i = 0; i < TS.NumPresses; i++) {
				/* Draw circle */
				TM_LCD_DrawFilledCircle(TS.X[i], TS.Y[i], 5, TOUCH_Colors[i]);
				
				/* Format string */
				sprintf(str, "X: %3d Y: %3d", TS.X[i], TS.Y[i]);
				
				/* Print on LCD */
				TM_LCD_SetXY(10, 10 + i * 20);
				TM_LCD_Puts(str);
			}
		}
		
		Delayms(5);
	}
}

