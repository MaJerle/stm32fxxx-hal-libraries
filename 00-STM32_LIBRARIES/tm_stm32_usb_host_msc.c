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
#include "tm_stm32_usb_host_msc.h"

TM_USBH_Result_t TM_USBH_MSC_Init(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Init HID class for FS */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Add class for HID to FS */
		USBH_RegisterClass(&hUSBHost_FS, USBH_MSC_CLASS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Init HID class for HS */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Add class for HID to HS */
		USBH_RegisterClass(&hUSBHost_HS, USBH_MSC_CLASS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_Result_t TM_USBH_MSC_IsConnected(TM_USB_t USB_Mode) {
	USBH_HandleTypeDef* USBHandle = TM_USBH_GetUSBPointer(USB_Mode);
	
	/* Check if device is connected and MSC class is active */
	if (USBHandle->device.is_connected && USBH_GetActiveClass(USBHandle) == USB_MSC_CLASS) {
		/* MSC is connected */
		return TM_USBH_Result_Ok;
	}
	
	/* Return error */
	return TM_USBH_Result_Error;
}

TM_USBH_Result_t TM_USBH_MSC_IsReady(TM_USB_t USB_Mode) {
	USBH_HandleTypeDef* USBHandle = TM_USBH_GetUSBPointer(USB_Mode);
	
	/* Check if device is ready */
	if (USBH_MSC_IsReady(USBHandle)) {
		/* MSC is ready */
		return TM_USBH_Result_Ok;
	}
	
	/* Return error */
	return TM_USBH_Result_Error;
}
