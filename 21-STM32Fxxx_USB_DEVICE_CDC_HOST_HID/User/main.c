/**
 * Keil project example for USB CDC device (Virtual COM Port) and USB HID host at the same time
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
#include "tm_stm32_usb_device.h"
#include "tm_stm32_usb_device_cdc.h"
#include "tm_stm32_usb_host.h"
#include "tm_stm32_usb_host_hid.h"
#include "tm_stm32_lcd.h"

/* Flag for printed on LCD */
uint8_t FS_Printed = 0;
uint8_t HS_Printed = 0;

/* USB CDC settings */
TM_USBD_CDC_Settings_t USB_FS_Settings;

/* Character value */
char ch;
char string_array[30];
	
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
	
	/* Init USB peripheral */
	TM_USB_Init();
	
	/* Init HOST on HS mode */
	TM_USBH_Init(TM_USB_HS);
	
	/* Init USB CDC DEVICE on FS port */
	TM_USBD_CDC_Init(TM_USB_FS);
	
	/* Send debug */
	printf("USB FS configured as CDC device!\n");
	
	/* Init USB HOST HID on HS port */
	TM_USBH_HID_Init(TM_USB_HS);
	
	/* Send debug */
	printf("USB HS configured as HID host!\n");
	
	/* Start USB CDC device */
	TM_USBD_Start(TM_USB_FS);
	
	/* Send debug */
	printf("USB FS started!\n");
	
	/* Start USB HID host */
	TM_USBH_Start(TM_USB_HS);
	
	/* Send debug */
	printf("USB HS started!\n");
	
	while (1) {
		/* Process USB CDC device, send remaining data if needed */
		/* It is better if you call this in periodic timer, like each ms in SYSTICK handler */
		TM_USBD_CDC_Process(TM_USB_FS);

		/* Process USB HID Host */
		TM_USBH_Process(TM_USB_HS);
		
		/* Check if CDC device is ready, if drivers are installed if needed */
		if (TM_USBD_IsDeviceReady(TM_USB_FS) == TM_USBD_Result_Ok) {
			/* Turn ON led */
			TM_DISCO_LedOn(LED_GREEN);
			
			/* Print if needed */
			if (!FS_Printed) {
				printf("FS: CDC ready to use!\n");
			
				FS_Printed = 1;
			}
		} else {
			/* Turn OFF led */
			TM_DISCO_LedOff(LED_GREEN);
			
			/* Print if needed */
			if (FS_Printed) {
				printf("FS: CDC not ready to use!\n");
			
				FS_Printed = 0;
			}
		}
		
		/* Check if anything received on FS port */
		if (TM_USBD_CDC_Getc(TM_USB_FS, &ch)) {
			/* One character received */
			
			/* Send it back */
			TM_USBD_CDC_Putc(TM_USB_FS, ch);
		}
		
		/* Check HS port if anything connected */
		if (
			TM_USBH_IsConnected(TM_USB_HS) == TM_USBH_Result_Ok &&
			TM_USBH_IsDeviceReady(TM_USB_HS) == TM_USBH_Result_Ok
		) {
			/* Something is connected */
			
			/* Check for HID */
			if (TM_USBH_HID_GetConnected(TM_USB_HS) == TM_USBH_HID_Keyboard) {
				if (!HS_Printed) {
					printf("HS: Keyboard connected!\n");
					
					HS_Printed = 1;
				}
			} else if (TM_USBH_HID_GetConnected(TM_USB_HS) == TM_USBH_HID_Mouse) {
				if (!HS_Printed) {
					printf("HS: Mouse connected!\n");
					
					HS_Printed = 1;
				}
			} else {
				/* No HID connected */
				if (!HS_Printed) {
					printf("HS: Unknown connected!\n");
					
					HS_Printed = 1;
				}
			}
		} else {
			/* Device not connected */
			if (HS_Printed) {
				printf("HS: Device disconnected!\n");
				
				HS_Printed = 0;
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

