/**
 * Keil project example for USB CDC device (Virtual COM Port) and USB HID host 
 * on the same USB port, with "on-the-fly" USB change mode when needed.
 *
 * USB HS is used for testing!
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
#include "tm_stm32_usb_device.h"
#include "tm_stm32_usb_device_cdc.h"
#include "tm_stm32_usb_host.h"
#include "tm_stm32_usb_host_hid.h"
#include "tm_stm32_lcd.h"

/* Process functions */
static void CDC_Device(void);
static void HID_Host(void);
	
int main(void) {
	/* Init system */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
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
	
	while (1) {
		/* HID Host */
		HID_Host();
		
		/* CDC device */
		CDC_Device();
	}
}

/* CDC device example */
static void CDC_Device(void) {
	uint8_t printed = 0;
	char ch;
	
	/* Init USB device */
	
	/* Init USB CDC DEVICE on HS port */
	TM_USBD_CDC_Init(TM_USB_HS);
	
	/* Send debug */
	printf("USB HS configured as CDC device!\n");
	
	/* Start USB CDC device */
	TM_USBD_Start(TM_USB_HS);
	
	/* Send debug */
	printf("USB HS started!\n");

	do {
		/* Process CDC device for outgoing data if anything to send */
		TM_USBD_CDC_Process(TM_USB_HS);
		
		/* Check if we are ready to use, if drivers are OK installed on computer */
		if (TM_USBD_IsDeviceReady(TM_USB_HS) == TM_USBD_Result_Ok) {
			/* We are ready */
			TM_DISCO_LedOn(LED_GREEN);
			
			/* Print status */
			if (!printed) {
				printf("USB CDC is ready to receive and transmit!\n");
				
				printed = 1;
			}
			
			/* Check if anything received */
			if (TM_USBD_CDC_Getc(TM_USB_HS, &ch)) {
				/* Send character back to terminal */
				TM_USBD_CDC_Putc(TM_USB_HS, ch);
			}
		} else {
			/* We are not ready */
			TM_DISCO_LedOff(LED_GREEN);
			
			/* Print status */
			if (printed) {
				printf("USB CDC is not ready to receive and transmit!\n");
				
				printed = 0;
			}
		}
	} while (!TM_DISCO_ButtonPressed());
	
	/* We are not ready */
	TM_DISCO_LedOff(LED_GREEN);
	
	/* Stop Device mode */
	TM_USBD_Stop(TM_USB_HS);
	
	/* Delay first */
	Delayms(100);
	
	/* Wait till button released */
	while (TM_DISCO_ButtonPressed());
}

/* HID host example */
static void HID_Host(void) {
	uint8_t printed = 0;
	
	/* Init HOST on HS mode */
	TM_USBH_Init(TM_USB_HS);
	
	/* Init USB HOST HID on FS port */
	TM_USBH_HID_Init(TM_USB_HS);
	
	/* Send debug */
	printf("USB HS configured as HID host!\n");
	
	/* Start USB HID host */
	TM_USBH_Start(TM_USB_HS);
	
	/* Send debug */
	printf("USB HS started!\n");
	
	do {
		/* Process HID host */
		TM_USBH_Process(TM_USB_HS);
		
		/* Check if anything connected */
		if (TM_USBH_IsDeviceReady(TM_USB_HS) == TM_USBH_Result_Ok) {
			if (!printed) {
				/* Connected device is HID and is ready */
				printf("Connected device is HID and is ready!\n");
				
				printed = 1;
			}
	
			/* Device connected */
			TM_DISCO_LedOn(LED_GREEN);
		} else {
			/* Check if disconnected */
			if (printed) {
				/* Device just disconnected */
				printf("Device disconnected!\n");
				
				printed = 0;
			}
	
			/* Device not connected */
			TM_DISCO_LedOff(LED_GREEN);
		}
	} while (!TM_DISCO_ButtonPressed());
	
	/* Device not connected */
	TM_DISCO_LedOff(LED_GREEN);
	
	/* Stop Host mode */
	TM_USBH_Stop(TM_USB_HS);
	
	/* Delay first */
	Delayms(100);
	
	/* Wait till button released */
	while (TM_DISCO_ButtonPressed());
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

