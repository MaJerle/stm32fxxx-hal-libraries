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

/* Structure for FATFS find files operations */
TM_FATFS_Search_t Files;

/* Buffer variable, it's size should be more that maximal filepath in folder searched on SDCARD */
char buffer[100];

/* Unmount line if you want to make 10 fake files first before checking SDCARD content */
#define CREATE_FILES

int main(void) {
#ifdef CREATE_FILES
	uint8_t i = 10;
#endif
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Set font */
	TM_LCD_SetFont(&TM_Font_7x10);
	
	/* Set start position */
	TM_LCD_SetXY(5, 5);
	
	/* Try to mount card */
	if ((fres = f_mount(&FS, "SD:", 1)) == FR_OK) {
#ifdef CREATE_FILES
		/* Create 10 fake files */
		while (i--) {
			/* Format path */
			sprintf(buffer, "SD:/file_%d.txt", i);
			
			/* open file */
			f_open(&fil, buffer, FA_OPEN_ALWAYS | FA_WRITE | FA_WRITE);
			
			/* Put some text */
			f_puts("Test file content!", &fil);
			
			/* Close file */
			f_close(&fil);
		}
#endif
		
		/* Make a search on files */
		if ((fres = TM_FATFS_Search("SD:", buffer, sizeof(buffer), &Files)) == FR_OK) {
			/* Search was OK */
			
			/* Format string */
			sprintf(buffer, "Search OK! Num of files: %d; Num of folders: %d\n", Files.FilesCount, Files.FoldersCount);
			
			/* Send on LCD */
			TM_LCD_Puts(buffer);
		} else {
			/* Error, probably buffer is too small for all sub folders */
			TM_LCD_Puts("Search error!\n");
		}
		
		/* Unmount SDCARD */
		f_mount(NULL, "SD:", 1);
	}
	
	/* Do nothing */
	while (1) {
		
	}
}

/*******************************************************************/
/*                      FATFS SEARCH CALLBACK                      */
/*******************************************************************/
uint8_t TM_FATFS_SearchCallback(char* path, uint8_t is_file, TM_FATFS_Search_t* FindStructure) {
	/* Print on LCD */
	TM_LCD_Puts(path);
	
	/* Delete file on card */
	if (f_unlink(path) == FR_OK) {
		TM_LCD_Puts("; Delete OK!\n");
	} else {
		TM_LCD_Puts("; Delete Error!\n");
	}
	
	/* Allow next search */
	return 1;
}
