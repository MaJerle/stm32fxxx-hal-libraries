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
#include "tm_stm32_usb_host_hid.h"

TM_USBH_Result_t TM_USBH_HID_Init(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Init HID class for FS */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Add class for HID to FS */
		USBH_RegisterClass(&hUSBHost_FS, USBH_HID_CLASS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Init HID class for HS */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Add class for HID to HS */
		USBH_RegisterClass(&hUSBHost_HS, USBH_HID_CLASS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_HID_t TM_USBH_HID_GetConnected(TM_USB_t USB_Mode) {
	HID_TypeTypeDef type;
	USBH_HandleTypeDef* USBHandle = TM_USBH_GetUSBPointer(USB_Mode);
	static TM_USBH_HID_t curr;
	
	/* Check if current class is HID */
	if (USBH_GetActiveClass(USBHandle) != USB_HID_CLASS) {
		/* HID is not active */
		return TM_USBH_HID_None;
	}
	
	/* Get keyboard type */
	type = USBH_HID_GetDeviceType(USBHandle);
	
	/* Check for keyboard */
	if (type == HID_KEYBOARD) {
		curr = TM_USBH_HID_Keyboard;
	} else if (type == HID_MOUSE) {
		curr = TM_USBH_HID_Mouse;
	} else {
		curr = TM_USBH_HID_None;
	}
	
	/* No device detected */
	return curr;
}

TM_USBH_HID_t TM_USBH_HID_GetKeyboard(TM_USB_t USB_Mode, TM_USBH_HID_Keyboard_t* KeyboardStruct) {
	HID_KEYBD_Info_TypeDef* k_pinfo; 
	uint8_t i;
	
	/* Get keyboard informations */
	k_pinfo = USBH_HID_GetKeybdInfo(TM_USBH_GetUSBPointer(USB_Mode));

	/* Check for ASCII value */
	if (k_pinfo != NULL) {
		/* Get ASCII value */
		KeyboardStruct->C = USBH_HID_GetASCIICode(k_pinfo);
		
		/* Copy everything */
		for (i = 0; i < 6; i++) {
			KeyboardStruct->Keys[i] = k_pinfo->keys[i];
		}
		
		/* Control pins */
		KeyboardStruct->Special.S.LAlt = k_pinfo->lalt ? 1 : 0;
		KeyboardStruct->Special.S.LShift = k_pinfo->lshift ? 1 : 0;
		KeyboardStruct->Special.S.LCtrl = k_pinfo->lctrl ? 1 : 0;
		KeyboardStruct->Special.S.LGUI = k_pinfo->lgui ? 1 : 0;
		KeyboardStruct->Special.S.RAlt = k_pinfo->ralt ? 1 : 0;
		KeyboardStruct->Special.S.RShift = k_pinfo->rshift ? 1 : 0;
		KeyboardStruct->Special.S.RCtrl = k_pinfo->rctrl ? 1 : 0;
		KeyboardStruct->Special.S.RGUI = k_pinfo->rgui ? 1 : 0;
		KeyboardStruct->Status = k_pinfo->state;
		
		/* Set flags if needed */
		KeyboardStruct->Special.S.Alt = KeyboardStruct->Special.S.LAlt | KeyboardStruct->Special.S.RAlt;
		KeyboardStruct->Special.S.Shift = KeyboardStruct->Special.S.LShift | KeyboardStruct->Special.S.RShift;
		KeyboardStruct->Special.S.Ctrl = KeyboardStruct->Special.S.LCtrl | KeyboardStruct->Special.S.RCtrl;
		KeyboardStruct->Special.S.GUI = KeyboardStruct->Special.S.LGUI | KeyboardStruct->Special.S.RGUI;
	
		/* Keyboard connected and OK */
		return TM_USBH_HID_Keyboard;
	}
	
	/* Return ERROR */
	return TM_USBH_HID_None;
}

TM_USBH_HID_t TM_USBH_HID_GetMouse(TM_USB_t USB_Mode, TM_USBH_HID_Mouse_t* MouseStruct) {
	HID_MOUSE_Info_TypeDef* k_minfo;

	/* Get keyboard informations */
	k_minfo = USBH_HID_GetMouseInfo(TM_USBH_GetUSBPointer(USB_Mode));

	/* Check for ASCII value */
	if (k_minfo != NULL) {
		MouseStruct->AbsoluteX += (int8_t)k_minfo->x;
		MouseStruct->AbsoluteY += (int8_t)k_minfo->y;
		MouseStruct->RelativeX = (int8_t)k_minfo->x;
		MouseStruct->RelativeY = (int8_t)k_minfo->y;
		
		/* Copy buttons */
		MouseStruct->Buttons[0] = k_minfo->buttons[0];
		MouseStruct->Buttons[1] = k_minfo->buttons[1];
		MouseStruct->Buttons[2] = k_minfo->buttons[2];
		
		/* Keyboard connected */
		return TM_USBH_HID_Mouse;
	}
	
	/* Return ERROR */
	return TM_USBH_HID_None;
}
