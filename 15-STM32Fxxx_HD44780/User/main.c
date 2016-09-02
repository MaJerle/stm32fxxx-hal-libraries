/**
 * Keil project example for HD44780 LCD
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
#include "tm_stm32_hd44780.h"
	
/* Rectangle for custom character */
/* xxx means doesn't care, lower 5 bits are important for LCD */
uint8_t customChar[] = {
	0x1F,	/*  xxx 11111 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x11,	/*  xxx 10001 */
	0x1F	/*  xxx 11111 */
};
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Initialize LCD 20 cols x 4 rows */
	TM_HD44780_Init(20, 4);
	
	/* Save custom character on location 0 in LCD */
	TM_HD44780_CreateChar(0, customChar);
	
	/* Put string to LCD */
	TM_HD44780_Puts(0, 0, "STM32F4/7-Disco/Eval");
	TM_HD44780_Puts(2, 1, "20x4 HD44780 LCD");
	TM_HD44780_Puts(0, 2, "stm32f4-\n\r       discovery.com");

	/* Wait a little */
	Delayms(3000);
	
	/* Clear LCD */
	TM_HD44780_Clear();
	
	/* Show cursor */
	TM_HD44780_CursorOn();
	
	/* Write new text */
	TM_HD44780_Puts(6, 2, "CLEARED!");
	
	/* Wait a little */
	Delayms(1000);
	
	/* Enable cursor blinking */
	TM_HD44780_BlinkOn();
	
	/* Show custom character at x = 1, y = 2 from RAM location 0 */
	TM_HD44780_PutCustom(1, 2, 0);
	
	while (1) {
		/* Do nothing */
	}
}

