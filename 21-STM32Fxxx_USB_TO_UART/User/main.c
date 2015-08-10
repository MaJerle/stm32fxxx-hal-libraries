/**
 * Keil project example for USB CDC device and UART (USB to UART converted)
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
#include "tm_stm32_usb_device.h"
#include "tm_stm32_usb_device_cdc.h"
#include "tm_stm32_usart.h"

/* USB CDC settings */
TM_USBD_CDC_Settings_t USB_Settings;

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
	
	/* Init VCP on HS port */
	TM_USBD_CDC_Init(TM_USB_HS);
	
	/* Start USB device mode on HS port */
	TM_USBD_Start(TM_USB_HS);
	
	while (1) {
		/* Process USB CDC device, send remaining data if needed */
		/* It is better if you call this in periodic timer, like each ms in SYSTICK handler */
		TM_USBD_CDC_Process(TM_USB_HS);
		
		/* Check if device is ready, if drivers are installed if needed on HS port */
		if (TM_USBD_IsDeviceReady(TM_USB_HS) == TM_USBD_Result_Ok) {
			/* Turn on green LED */
			TM_DISCO_LedOn(LED_GREEN);
			
			/* Check if user has changed parameters for COM port */
			TM_USBD_CDC_GetSettings(TM_USB_HS, &USB_Settings);
		
			/* Check if settings updated from user terminal */
			if (USB_Settings.Updated) {
				/* Reinit USART, only baudrate works in this example */
				TM_USART_Init(USART6, TM_USART_PinsPack_1, USB_Settings.Baudrate);
			}
		
			/* Check if anything received on HS port from user */
			while (TM_USBD_CDC_Getc(TM_USB_HS, &ch)) {
				/* One character received */
				
				/* Send character to USART */
				TM_USART_Putc(USART6, ch);
			}
			
			/* CHeck if any character received from USART */
			while (!TM_USART_BufferEmpty(USART6)) {
				/* Send data over USB CDC */
				TM_USBD_CDC_Putc(TM_USB_HS, TM_USART_Getc(USART6));
			}	
		} else {
			/* Turn off green LED */
			TM_DISCO_LedOff(LED_GREEN);
		}
	}
}

