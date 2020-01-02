/**
 * Keil project example for USB MSC on both USB ports at the same time
 *
 * @note      Check defines.h file for configuration settings!
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
#include "tm_stm32_usb_host.h"
#include "tm_stm32_usb_host_msc.h"
#include "tm_stm32_lcd.h"
#include "tm_stm32_fatfs.h"
#include "tm_stm32_usart.h"
#include "string.h"
	
/* FATFS related */
FATFS FS, FATFS_USB_HS;
FIL fil, filSD;
FRESULT fres;

#define SECTORS_COUNT         32
#define SECTORS_ALL           (SECTORS_COUNT * 128)

uint8_t data[SECTORS_COUNT * 512];
uint32_t bw, br;

/* Flags */
uint8_t FS_Mounted = 0, HS_Mounted = 0;
	
int main(void) {
	TM_RCC_InitSystem();
	HAL_Init();
	TM_DISCO_LedInit();
	TM_DELAY_Init();
#if defined(STM32F429_DISCOVERY) || defined(STM32F7_DISCOVERY) || defined(STM32F439_EVAL)
	TM_LCD_Init();
	TM_LCD_SetFont(&TM_Font_7x10);
	TM_LCD_SetXY(5, 5);
#endif
#if defined(STM32F7_DISCOVERY)
	TM_LCD_SetOrientation(3);
#endif
	TM_USB_Init();
	TM_USBH_Init(TM_USB_Both);
	TM_USBH_MSC_Init(TM_USB_Both);
	TM_USBH_Start(TM_USB_Both);
    
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
    TM_USART_Puts(USART1, "Test\n");
	
	while (1) {
		/* Process USB host FS and HS modes */
		TM_USBH_Process(TM_USB_Both);
		
		/* Check MSC host on HS port */
		if (
			TM_USBH_IsConnected(TM_USB_HS) == TM_USBH_Result_Ok &&     /*!< Check if any device connected to USB HS port */
			TM_USBH_MSC_IsConnected(TM_USB_HS) == TM_USBH_Result_Ok && /*!< Device connected to USB port is MSC type */
			TM_USBH_MSC_IsReady(TM_USB_HS) == TM_USBH_Result_Ok        /*!< Device is ready */
		) {
			if (!HS_Mounted) {
				if ((fres = f_mount(&FATFS_USB_HS, "USBHS:", 1)) == FR_OK) {
                    uint32_t sector;
                    HS_Mounted = 1;
                    printf("USB mounted!\n");
                            
                    //Read USB with disk_read operation (plain sectors) and write t oFAT to SDCARD
                    for (sector = 0; sector < SECTORS_ALL; sector += SECTORS_COUNT) {
                        if (disk_read(FATFS_USB_HS.drv, data, sector, SECTORS_COUNT) != RES_OK) {
                            printf("Failed reading %d sector\n", sector);
                            break;
                        }
                        
                        TM_USART_Send(USART1, data, sizeof(data));
                    }
                            
					f_mount(NULL, "USBHS:", 1);
				} else {
					printf("USB HS: Failed to mount! FRES: %d\n", fres);
				}
			}
		} else {
			HS_Mounted = 0;
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
