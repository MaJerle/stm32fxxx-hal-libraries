/**
 * Keil project example for USB CDC device (Virtual COM Port)
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
#include "tm_stm32_usb_device.h"
#include "tm_stm32_usb_device_cdc.h"

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
	
	/* Init USB peripheral */
	TM_USB_Init();
	
	/* Init VCP on FS and HS ports.. */
	TM_USBD_CDC_Init(TM_USB_FS);
	TM_USBD_CDC_Init(TM_USB_HS);
	
	/* ..or use single call for both modes */
	//TM_USBD_CDC_Init(TM_USB_Both);
	
	/* Start USB device mode on FS and HS ports.. */
	TM_USBD_Start(TM_USB_FS);
	TM_USBD_Start(TM_USB_HS);
	
	/* .. or use single call for both modes */
	//TM_USBD_Start(TM_USB_Both);
	
	while (1) {
		/* Process USB CDC device, send remaining data if needed */
		/* It is better if you call this in periodic timer, like each ms in SYSTICK handler */
		TM_USBD_CDC_Process(TM_USB_Both);
		
		/* Check if device is ready, if drivers are installed if needed on FS port */
		if (TM_USBD_IsDeviceReady(TM_USB_FS) == TM_USBD_Result_Ok) {
			TM_DISCO_LedOn(LED_GREEN);
		} else {
			TM_DISCO_LedOff(LED_GREEN);
		}
		
		/* Check if user has changed parameter for COM port */
		TM_USBD_CDC_GetSettings(TM_USB_FS, &USB_FS_Settings);
		
		/* Check if updated */
		if (USB_FS_Settings.Updated) {
			/* Update settings for UART here if needed */
			TM_USBD_CDC_Puts(TM_USB_FS, "USB FS settings changed!\n");
		}
		
		/* Check if anything received on FS port */
		if (TM_USBD_CDC_Getc(TM_USB_FS, &ch)) {
			/* One character received */
			
			/* Send it back */
			TM_USBD_CDC_Putc(TM_USB_FS, ch);
		}
		
		/* Check if device is ready, if drivers are installed if needed on HS port */
		if (TM_USBD_IsDeviceReady(TM_USB_HS) == TM_USBD_Result_Ok) {
			//TM_DISCO_LedOn(LED_GREEN);
		} else {
			//TM_DISCO_LedOff(LED_GREEN);
		}		

		/* Check if any string received on HS port */
		if (TM_USBD_CDC_Gets(TM_USB_HS, string_array, sizeof(string_array))) {
			/* One character received */
			
			/* Send it back */
			TM_USBD_CDC_Puts(TM_USB_HS, string_array);
		}
	}
}

