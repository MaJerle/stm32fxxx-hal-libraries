/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
