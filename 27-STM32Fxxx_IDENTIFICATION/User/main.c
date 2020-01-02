/**
 * Keil project example for identification library
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
#include "tm_stm32_id.h"
	
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

	/* Display fixes */
#if defined(STM32F7_DISCOVERY)
	TM_LCD_SetFont(&TM_Font_7x10);
#elif defined(STM32F429_DISCOVERY)
	TM_LCD_SetFont(&TM_Font_7x10);
	TM_LCD_SetOrientation(3);
#endif
	
	/* Format unique ID */
	sprintf(buff, "Unique ID: 0x%08X 0x%08X 0x%08X", TM_ID_GetUnique32(0), TM_ID_GetUnique32(1), TM_ID_GetUnique32(2));
	TM_LCD_SetXY(10, 10);
	TM_LCD_Puts(buff);
	
	/* Format device signature */
	sprintf(buff, "Device signature: 0x%04X", TM_ID_GetSignature());
	TM_LCD_SetXY(10, 30);
	TM_LCD_Puts(buff);
	
	/* Format revision */
	sprintf(buff, "Revision: 0x%04X", TM_ID_GetRevision());
	TM_LCD_SetXY(10, 50);
	TM_LCD_Puts(buff);
	
	/* Format package */
	sprintf(buff, "Package: 0x%04X", TM_ID_GetPackage());
	TM_LCD_SetXY(10, 70);
	TM_LCD_Puts(buff);
	
	/* Format flash size */
	sprintf(buff, "Flash size: %04d kBytes", TM_ID_GetFlashSize());
	TM_LCD_SetXY(10, 90);
	TM_LCD_Puts(buff);

	while (1) {

	}
}


