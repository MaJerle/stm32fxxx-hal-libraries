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
#include "tm_stm32_usb_host_hid.h"

TM_USBH_Result_t TM_USBH_HID_Init(TM_USBH_t USB_Mode) {
#ifdef USB_USE_FS
	/* Init HID class for FS */
	if (USB_Mode == TM_USBH_FS || USB_Mode == TM_USBH_Both) {
		/* Add class for HID to FS */
		USBH_RegisterClass(&hUSBHost_FS, USBH_HID_CLASS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Init HID class for HS */
	if (USB_Mode == TM_USBH_HS || USB_Mode == TM_USBH_Both) {
		/* Add class for HID to HS */
		USBH_RegisterClass(&hUSBHost_HS, USBH_HID_CLASS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_HID_t TM_USBH_HID_GetConnected(TM_USBH_t USB_Mode) {
	HID_TypeTypeDef type;
	USBH_HandleTypeDef* USBHandle = TM_USBH_GetUSBPointer(USB_Mode);
	
	/* Check if current class is HID */
	if (USBH_GetActiveClass(USBHandle) != USB_HID_CLASS) {
		/* HID is not active */
		return TM_USBH_HID_None;
	}
	
	/* Get keyboard type */
	type = USBH_HID_GetDeviceType(USBHandle);
	
	/* Check for keyboard */
	if (type == HID_KEYBOARD) {
		return TM_USBH_HID_Keyboard;
	}
	
	/* Check for mouse */
	if (type == HID_MOUSE) {
		return TM_USBH_HID_Mouse;
	}
	
	/* No device detected */
	return TM_USBH_HID_None;
}
