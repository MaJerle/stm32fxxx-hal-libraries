/**
 * Keil project for SDCARD reader using SDIO/SDMMC example
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
#include "tm_stm32_fatfs.h"
#include "tm_stm32_lcd.h"
#include "stdio.h"

/* Fatfs structure */
FATFS FS;
FIL fil;
FRESULT fres;

/* Size structure for FATFS */
TM_FATFS_Size_t CardSize;

/* Buffer variable */
char buffer[100];

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Try to mount card */
	if (f_mount(&FS, "SD:", 1) == FR_OK) {
		/* Try to open file */
		if ((fres = f_open(&fil, "SD:first_file.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE)) == FR_OK) {
			/* Read SDCARD size */
			TM_FATFS_GetDriveSize("SD:", &CardSize);
			
			/* Format string */
			sprintf(buffer, "Total card size: %u kBytes\n", CardSize.Total);
			
			/* Write total card size to file */
			f_puts(buffer, &fil);
			
			/* Format string for free card size */
			sprintf(buffer, "Free card size:  %u kBytes\n", CardSize.Free);
			
			/* Write free card size to file */
			f_puts(buffer, &fil);
			
			/* Close file */
			f_close(&fil);
			
			/* Turn led ON */
			TM_DISCO_LedOn(LED_ALL);
		}
		
		/* Unmount SDCARD */
		f_mount(NULL, "SD:", 1);
	}
	
	/* Do nothing */
	while (1) {
		
	}
}

