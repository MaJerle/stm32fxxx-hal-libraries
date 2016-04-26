/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_usb_device.h"

/* Variables for HCD and main for USB */
#ifdef USB_USE_FS
USBD_HandleTypeDef hUSBDevice_FS;
#endif
#ifdef USB_USE_HS
USBD_HandleTypeDef hUSBDevice_HS;
#endif

/* Create variable with pointers */
static USBD_HandleTypeDef* hUSBDevices[3] = {
#ifdef USB_USE_FS
	&hUSBDevice_FS, 
#else
	0,
#endif
#ifdef USB_USE_HS
	&hUSBDevice_HS,
#else
	0,
#endif
	0
};

TM_USBD_Result_t TM_USBD_Start(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Start FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Start FS */
		USBD_Start(&hUSBDevice_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Start HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Start HS */
		USBD_Start(&hUSBDevice_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBD_Result_Ok;
}

TM_USBD_Result_t TM_USBD_Stop(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* STOP FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* STOP FS */
		USBD_Stop(&hUSBDevice_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Process HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* STOP HS */
		USBD_Stop(&hUSBDevice_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBD_Result_Ok;
}

TM_USBD_Result_t TM_USBD_IsDeviceReady(TM_USB_t USB_Mode) {
	USBD_HandleTypeDef* Handle = TM_USBD_GetUSBPointer(USB_Mode);
	
	/* Check if ready */
	if (Handle && Handle->dev_state == USBD_STATE_CONFIGURED) {
		return TM_USBD_Result_Ok;
	}
	
	/* Device is not ready */
	return TM_USBD_Result_Error;
}

USBD_HandleTypeDef* TM_USBD_GetUSBPointer(TM_USB_t USB_Mode) {
	/* Return pointer to USB handle */
	return hUSBDevices[(uint8_t)USB_Mode];
}
