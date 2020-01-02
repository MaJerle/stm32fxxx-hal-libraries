/**
 * Keil project example for USB HID and MSC at the same time
 *
 * @note      Check defines.h file for configuration settings!
 *
 * Example shows how to use 2 USB HID devices at a time!
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
#include "tm_stm32_usb_host_hid.h"
#include "tm_stm32_lcd.h"

/* Check if we should use LCD */
#if defined(STM32F429_DISCOVERY) || defined(STM32F7_DISCOVERY) || defined(STM32F439_EVAL)
#define USE_LCD     1
#else
#define USE_LCD     0
#endif

/* Flags */
uint8_t FS_Printed = 0, HS_Printed = 0;
	
int main(void) {
	/* Init system */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init delay */
	TM_DELAY_Init();
	
#if USE_LCD
	/* Init LCD */
	TM_LCD_Init();
	TM_LCD_SetFont(&TM_Font_7x10);
	TM_LCD_SetXY(5, 5);
#endif
	
	/* Init USB */
	TM_USB_Init();
	
	/* Init USB Host, FS and HS modes */
	TM_USBH_Init(TM_USB_Both);
	
	/* Enable HID HOST class for both USB modes */
	TM_USBH_HID_Init(TM_USB_Both);
	
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
					printf("FS: Keyboard connected! VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_FS), TM_USBH_GetPID(TM_USB_FS));
					
					/* Turn on green LED */
					TM_DISCO_LedOn(LED_GREEN);
					
					/* Set flag */
					FS_Printed = 1;
				}
			} else if (TM_USBH_HID_GetConnected(TM_USB_FS) == TM_USBH_HID_Mouse) {
				/* Mouse is connected on FS port */
				
				/* If not printed yet */
				if (!FS_Printed) {
					/* Print to LCD */
					printf("FS: Mouse connected! VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_FS), TM_USBH_GetPID(TM_USB_FS));
					
					/* Turn on green LED */
					TM_DISCO_LedOn(LED_GREEN);
				
					/* Set flag */
					FS_Printed = 1;
				}
			}
		} else if (FS_Printed) {
			/* Check if no device connected */
			if (TM_USBH_HID_GetConnected(TM_USB_FS) == TM_USBH_HID_None) {
				/* Nothing connected anymore */
				/* Print to LCD */
				printf("FS: Device disconnected\n");
				
				/* Clear flag */
				FS_Printed = 0;
			
				/* Turn off green LED */
				TM_DISCO_LedOff(LED_GREEN);
			}
		}
		
		
		/* Check if device connected on HS port */
		if (TM_USBH_IsConnected(TM_USB_HS) == TM_USBH_Result_Ok) {
			/* Check if any HID devie is connected to HS port */
			if (TM_USBH_HID_GetConnected(TM_USB_HS) == TM_USBH_HID_Keyboard) {
				/* Keyboard is connected on HS port */
				
				/* If not printed yet */
				if (!HS_Printed) {
					/* Print to LCD */
					printf("HS: Keyboard connected! VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_HS), TM_USBH_GetPID(TM_USB_HS));
					
					/* Turn on green LED */
					TM_DISCO_LedOn(LED_GREEN);
					
					/* Set flag */
					HS_Printed = 1;
				}
			} else if (TM_USBH_HID_GetConnected(TM_USB_HS) == TM_USBH_HID_Mouse) {
				/* Mouse is connected on HS port */
				
				/* If not printed yet */
				if (!HS_Printed) {
					/* Print to LCD */
					printf("HS: Mouse connected! VID: %04X; PID: %04X\n", TM_USBH_GetVID(TM_USB_HS), TM_USBH_GetPID(TM_USB_HS));
					
					/* Turn on green LED */
					TM_DISCO_LedOn(LED_GREEN);
				
					/* Set flag */
					HS_Printed = 1;
				}
			}
		} else if (HS_Printed) {
			/* Check if no device connected */
			if (TM_USBH_HID_GetConnected(TM_USB_HS) == TM_USBH_HID_None) {
				/* Nothing connected anymore */
				/* Print to LCD */
				printf("HS: Device disconnected\n");
				
				/* Clear flag */
				HS_Printed = 0;
			
				/* Turn off green LED */
				TM_DISCO_LedOff(LED_GREEN);
			}
		}
	}
}

/* printf handler */
int fputc(int ch, FILE* fil) {
#if USE_LCD
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
