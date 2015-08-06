/**
 * Keil project example for USB CDC device (Virtual COM Port)
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

/* Flags */
uint8_t FS_Printed = 0, HS_Printed = 0;
uint8_t mounted = 0;
	
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
	
	/* Init VCP on FS port */
	TM_USBD_CDC_Init(TM_USB_FS);
	
	/* Init VCP on HS port */
	TM_USBD_CDC_Init(TM_USB_HS);
	
	/* Start VCP */
	TM_USBD_Start(TM_USB_FS);
	TM_USBD_Start(TM_USB_HS);
	
	while (1) {
		/* Process USB CDC device, send remaining data if needed */
		//TM_USBD_CDC_Process(TM_USB_Both);
	}
}

