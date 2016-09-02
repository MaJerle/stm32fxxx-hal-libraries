/**
 * Keil project example for SSD1306 I2C based LCD
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
#include "tm_stm32_ssd1306.h"
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init delay */
	TM_DELAY_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init SSD1306 LCD 128 x 64 px */
	if (TM_SSD1306_Init()) {
		/* SSD1306 is connected */
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		/* SSD1306 is not connected */
		while (1) {
			/* Toggle all leds */
			TM_DISCO_LedToggle(LED_ALL);
			
			/* Delay a little */
			Delayms(50);
		}
	}
	
	/* Go to location X = 30, Y = 4 */
	TM_SSD1306_GotoXY(10, 4);
	TM_SSD1306_Puts("STM32Fxxx HAL", &TM_Font_7x10, SSD1306_COLOR_WHITE);
	
	/* Go to location X = 20, Y = 25 */
	TM_SSD1306_GotoXY(8, 25);
	TM_SSD1306_Puts("I2C SSD1306 LCD", &TM_Font_7x10, SSD1306_COLOR_WHITE);
	
	/* Go to location X = 15, Y = 45 */
	TM_SSD1306_GotoXY(15, 45);
	TM_SSD1306_Puts("Tilen Majerle", &TM_Font_7x10, SSD1306_COLOR_WHITE);
	
	while (1) {
		/* Invert pixels */
		TM_SSD1306_ToggleInvert();
		
		/* Update screen */
		TM_SSD1306_UpdateScreen();
		
		/* Make a little delay */
		Delayms(500);
	}
}

