/**
 * Keil project example for USB HID and MSC at the same time
 *
 * @note      Check defines.h file for configuration settings!
 *
 * Example has enabled 2 USB ports (FS and HS).
 * On FS port, HID device is expected where you will see on LCD which type is connected.
 * In case of HS port, USB flash key should be connected where text will be written to file!
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
#include "tm_stm32_usb_host.h"
#include "tm_stm32_usb_host_hid.h"
#include "tm_stm32_usb_host_msc.h"
#include "tm_stm32_lcd.h"
#include "tm_stm32_fatfs.h"
	
/* FATFS related */
FATFS FATFS_USB;
FIL fil;
FRESULT fres;

/* Flags */
uint8_t FS_Printed = 0, HS_Printed = 0;
uint8_t mounted = 0;

/* USB HID HOST related */
TM_USBH_HID_Keyboard_t KbdInfo;
TM_USBH_HID_Mouse_t MouseInfo;
	
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
	
#if defined(STM32F7_DISCOVERY)
	/* Rotate LCD */
	TM_LCD_SetOrientation(2);
#endif
#endif
	
	/* Init USB */
	TM_USB_Init();
	
	/* Init USB Host, FS and HS modes */
	TM_USBH_Init(TM_USB_Both);
	
	/* Enable HID HOST class for USB FS mode */
	TM_USBH_HID_Init(TM_USB_FS);
	
	/* Enable MSC HOST class for USB HS mode */
	TM_USBH_MSC_Init(TM_USB_HS);
	
	/* Start USB host on FS and HS */
	TM_USBH_Start(TM_USB_Both);
	
	while (1) {
		/* Process USB host FS and HS modes */
		TM_USBH_Process(TM_USB_Both);
		
		/* Check if device connected on FS port */
		if (TM_USBH_IsConnected(TM_USB_FS) == TM_USBH_Result_Ok) {
			/* Check if any HID devie is connected to FS port */
			if (TM_USBH_HID_GetConnected(TM_USB_FS) == TM_USBH_HID_Keyboard) {
				/* Keyboard is connected on FS port */
				
				/* If not printed yet */
				if (!FS_Printed) {
					/* Print to LCD */
					printf("USB FS: Keyboard connected! VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_FS), TM_USBH_GetPID(TM_USB_FS));
					
					/* Set flag */
					FS_Printed = 1;
				}
				
				/* Read pressed button if any */
				if (TM_USBH_HID_GetKeyboard(TM_USB_FS, &KbdInfo) == TM_USBH_HID_Keyboard) {
					/* Something pressed on keyboard */
					
					/* Print on LCD if character is valid */
					if (KbdInfo.C) {
						printf("Character: %c\n", KbdInfo.C);
					}
					
					/* Print others */
					printf("LS: %d ", KbdInfo.Special.S.LShift);
					printf("LC: %d ", KbdInfo.Special.S.LCtrl);
					printf("LA: %d ", KbdInfo.Special.S.LAlt);
					printf("LG: %d ", KbdInfo.Special.S.LGUI);
					printf("\n");
				}
			} else if (TM_USBH_HID_GetConnected(TM_USB_FS) == TM_USBH_HID_Mouse) {
				/* Mouse is connected on FS port */
				
				/* If not printed yet */
				if (!FS_Printed) {
					/* Print to LCD */
					printf("USB FS: Mouse connected! VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_FS), TM_USBH_GetPID(TM_USB_FS));
					
					/* Turn on green LED */
					TM_DISCO_LedOn(LED_GREEN);
				
					/* Set flag */
					FS_Printed = 1;
				}
				
				/* Read mouse if any */
				if (TM_USBH_HID_GetMouse(TM_USB_FS, &MouseInfo) == TM_USBH_HID_Mouse) {
					/* Something pressed on keyboard */
					printf("Abs X: %d; Abs Y: %d; Rel X: %d; Rel Y %d\n", MouseInfo.AbsoluteX, MouseInfo.AbsoluteY, MouseInfo.RelativeX, MouseInfo.RelativeY);
					printf("Btn1: %d; Btn2: %d; Btn3: %d\n", MouseInfo.Buttons[0], MouseInfo.Buttons[1], MouseInfo.Buttons[2]);
				}
			}
		} else if (FS_Printed) {
			/* Check if no device connected */
			if (TM_USBH_HID_GetConnected(TM_USB_FS) == TM_USBH_HID_None) {
				/* Mouse is connected */
				/* Print to LCD */
				printf("USB FS: Device disconnected\n");
				
				/* Clear flag */
				FS_Printed = 0;
			
				/* Turn off green LED */
				TM_DISCO_LedOff(LED_GREEN);
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
			if (!mounted) {
				/* Try to mount and write using FATFS */
				if ((fres = f_mount(&FATFS_USB, "USBHS:", 1)) == FR_OK) {
					/* Mounted OK */
					printf("USB HS: Mounted OK!\n");
					
					/* Try to open file */
					if ((fres = f_open(&fil, "USBHS:usb_hs_file.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ)) == FR_OK) {
						/* File opened OK */
						printf("USB HS: File opened!\n");
						
						/* Go to end of file */
						f_lseek(&fil, f_size(&fil));
						
						/* Print string */
						if (f_puts("USB HS: File written using USB HS port\n", &fil)) {
							/* Written OK */
							printf("USB HS: File written OK!\n");
							
							/* Set flag */
							mounted = 1;
				
							/* Turn on red LED */
							TM_DISCO_LedOn(LED_RED);
						} else {
							printf("USB HS: File writing error!\n");
						}
						
						/* Close file */
						f_close(&fil);
						
						printf("USB HS: File closed!\n");
						
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
					printf("USB HS: Failed to mount!\n");
				}
			}
		} else {
			/* Clear flag */
			mounted = 0;
			
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
	TM_FONT_t* font = TM_LCD_GetFont();
	uint16_t currY = TM_LCD_GetCurrentY();
	
	/* Try to put character */
	if (TM_LCD_Putc(ch) != TM_LCD_Result_Ok) {
		/* Clear screen */
		TM_LCD_Fill(LCD_COLOR_WHITE);
		
		/* Go to beginning of LCD */
		TM_LCD_SetXY(5, 5);
		
		/* Try again */
		TM_LCD_Putc(ch);
	}
	
	/* Return OK */
	return ch;
#else
	return EOF;
#endif
}
