/**
 * Keil project example for USB MSC on both USB ports at the same time
 *
 * @note      Check defines.h file for configuration settings!
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
#include "tm_stm32_usb_host.h"
#include "tm_stm32_usb_host_msc.h"
#include "tm_stm32_lcd.h"
#include "tm_stm32_fatfs.h"
#include "tm_stm32_usart.h"
	
/* FATFS related */
FATFS FATFS_USB_FS, FATFS_USB_HS;
FIL fil;
FRESULT fres;

uint8_t writeArray[] = "My first file in USB";
uint32_t written;

/* Flags */
uint8_t FS_Printed = 0, HS_Printed = 0;
uint8_t FS_Mounted = 0, HS_Mounted = 0;
	
int main(void) {
	/* Init system */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init delay */
	TM_DELAY_Init();
	
#if defined(STM32F429_DISCOVERY) || defined(STM32F7_DISCOVERY) || defined(STM32F439_EVAL)
	/* Init LCD */
	TM_LCD_Init();
	TM_LCD_SetFont(&TM_Font_7x10);
	TM_LCD_SetXY(5, 5);
#endif
	
#if defined(STM32F7_DISCOVERY)
	/* Rotate LCD to portrait mode */
	TM_LCD_SetOrientation(3);
#endif
	
	/* Init USB */
	TM_USB_Init();
	
	/* Init USB Host, FS and HS modes */
	TM_USBH_Init(TM_USB_Both);
	
	/* Enable MSC HOST class for FS and HS */
	TM_USBH_MSC_Init(TM_USB_Both);
	
	/* Start USB host on FS and HS */
	TM_USBH_Start(TM_USB_Both);
	
	while (1) {
		/* Process USB host FS and HS modes */
		TM_USBH_Process(TM_USB_Both);
		
		/* Check MSC host on FS port */
		if (
			TM_USBH_IsConnected(TM_USB_FS) == TM_USBH_Result_Ok &&     /*!< Check if any device connected to USB FS port */
			TM_USBH_MSC_IsConnected(TM_USB_FS) == TM_USBH_Result_Ok && /*!< Device connected to USB port is MSC type */
			TM_USBH_MSC_IsReady(TM_USB_FS) == TM_USBH_Result_Ok        /*!< Device is ready */
		) {
			/* Device is connected on FS port */
			/* Connected device is MSC type */
			
			/* If not printed already */
			if (!FS_Printed) {
				/* Print to LCD */
				printf("--------------------------\n");
				printf("USB FS: USB MSC device connected with VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_FS), TM_USBH_GetPID(TM_USB_FS));
				
				/* Set flag */
				FS_Printed = 1;
			}
			
			/* If not mounted already */
			if (!FS_Mounted) {
				/* Try to mount and write using FATFS */
				if ((fres = f_mount(&FATFS_USB_FS, "USBFS:", 1)) == FR_OK) {
					/* Mounted OK */
					printf("USB FS: Mounted OK!\n");
					
					/* Try to open file */
					if ((fres = f_open(&fil, "USBFS:usb_fs_file.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ)) == FR_OK) {
						/* File opened OK */
						printf("USB FS: File opened!\n");
						
						/* Go to end of file */
						f_lseek(&fil, f_size(&fil));
						
						/* Print string */
						if ((fres = f_write(&fil, writeArray, sizeof(writeArray), &written)) == FR_OK) {
							/* Written OK */
							printf("USB FS: File written OK!\n");
				
							/* Turn on red LED */
							TM_DISCO_LedOn(LED_RED);
						} else {
							printf("USB FS: File writing error! FRES: %d\n", fres);
						}
						
						/* Close file */
						f_close(&fil);
						
						/* Print debug */
						printf("USB FS: File closed!\n");
							
						/* Set flag */
						FS_Mounted = 1;
						
						/* Try to reopen file */
						if ((fres = f_open(&fil, "USBFS:usb_hs_file.txt", FA_WRITE | FA_READ)) == FR_OK) {
							printf("USB FS: File reopened OK and closed again!\n");
							f_close(&fil);
						} else {
							printf("USB FS: File reopen ERROR! FRES: %d\n", fres);
						}
					} else {
						/* File not opened */
						printf("USB FS: Failed to open file!\n");
					}
					
					/* Unmount USB */
					f_mount(NULL, "USBFS:", 1);
				} else {
					/* Mounting error! */
					printf("USB FS: Failed to mount! FRES: %d\n", fres);
				}
			}
		} else {
			/* Clear flag */
			FS_Mounted = 0;
			
			/* Print disconnection */
			if (FS_Printed) {
				/* Print to LCD */
				printf("USB FS: USB MSC device disconnected!\n");
				
				/* Clear flag */
				FS_Printed = 0;
				
				/* Turn off red LED */
				TM_DISCO_LedOff(LED_RED);
			}
		}
		
		
		
		/* Check MSC host on HS port */
		if (
			TM_USBH_IsConnected(TM_USB_HS) == TM_USBH_Result_Ok &&     /*!< Check if any device connected to USB HS port */
			TM_USBH_MSC_IsConnected(TM_USB_HS) == TM_USBH_Result_Ok && /*!< Device connected to USB port is MSC type */
			TM_USBH_MSC_IsReady(TM_USB_HS) == TM_USBH_Result_Ok        /*!< Device is ready */
		) {
			/* Device is connected on HS port */
			/* Connected device is MSC type */
			
			/* If not printed already */
			if (!HS_Printed) {
				/* Print to LCD */
				printf("--------------------------\n");
				printf("USB HS: USB MSC device connected with VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_HS), TM_USBH_GetPID(TM_USB_HS));
				
				/* Set flag */
				HS_Printed = 1;
			}
			
			/* If not mounted already */
			if (!HS_Mounted) {
				/* Try to mount and write using FATFS */
				if ((fres = f_mount(&FATFS_USB_HS, "USBHS:", 1)) == FR_OK) {
					/* Mounted OK */
					printf("USB HS: Mounted OK!\n");
					
					/* Try to open file */
					if ((fres = f_open(&fil, "USBHS:usb_hs_file.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ)) == FR_OK) {
						/* File opened OK */
						printf("USB HS: File opened!\n");
						
						/* Go to end of file */
						f_lseek(&fil, f_size(&fil));
						
							/* Print string */
						if ((fres = f_write(&fil, writeArray, sizeof(writeArray), &written)) == FR_OK) {
							/* Written OK */
							printf("USB HS: File written OK!\n");
				
							/* Turn on red LED */
							TM_DISCO_LedOn(LED_RED);
						} else {
							printf("USB HS: File writing error! FRES: %d\n", fres);
						}
						
						/* Close file */
						f_close(&fil);
						
						/* Print debug */
						printf("USB HS: File closed!\n");
							
						/* Set flag */
						HS_Mounted = 1;
						
						/* Try to reopen file */
						if ((fres = f_open(&fil, "USBHS:usb_hs_file.txt", FA_WRITE | FA_READ)) == FR_OK) {
							printf("USB HS: File reopened OK and closed again!\n");
							f_close(&fil);
						} else {
							printf("USB HS: File reopen ERROR! FRES: %d\n", fres);
						}
					} else {
						/* File not opened */
						printf("USB HS: Failed to open file!\n");
					}
					
					/* Unmount USB */
					f_mount(NULL, "USBHS:", 1);
				} else {
					/* Mounting error! */
					printf("USB HS: Failed to mount! FRES: %d\n", fres);
				}
			}
		} else {
			/* Clear flag */
			HS_Mounted = 0;
			
			/* Print disconnection */
			if (HS_Printed) {
				/* Print to LCD */
				printf("USB HS: USB MSC device disconnected!\n");
				
				/* Clear flag */
				HS_Printed = 0;
				
				/* Turn off red LED */
				TM_DISCO_LedOff(LED_RED);
			}
		}
	}
}

/* printf handler */
int fputc(int ch, FILE* fil) {
#if defined(STM32F429_DISCOVERY) || defined(STM32F7_DISCOVERY) || defined(STM32F439_EVAL)
	/* Try to put character */
	if (TM_LCD_Putc(ch) != TM_LCD_Result_Ok) {
		/* Clear screen */
		TM_LCD_Fill(LCD_COLOR_WHITE);
		
		/* Go to beginning of LCD */
		TM_LCD_SetXY(5, 5);
		
		/* Try again */
		TM_LCD_Putc(ch);
	}
#endif
	
	/* Return OK */
	return ch;
}
