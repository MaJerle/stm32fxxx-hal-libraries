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
#include "tm_stm32_usb_device.h"

/* Variables for HCD and main for USB */
USBD_HandleTypeDef hUSBDevice_FS;
USBD_HandleTypeDef hUSBDevice_HS;

/* Create variable with pointers */
static USBD_HandleTypeDef* hUSBDevices[3] = {&hUSBDevice_FS, &hUSBDevice_HS};

TM_USBD_Result_t TM_USBD_Start(TM_USBD_t USB_Mode) {
#ifdef USB_USE_FS
	/* Start FS mode */
	if (USB_Mode == TM_USBD_FS || USB_Mode == TM_USBD_Both) {
		/* Start FS */
		USBD_Start(&hUSBDevice_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Start HS mode */
	if (USB_Mode == TM_USBD_HS || USB_Mode == TM_USBD_Both) {
		/* Start HS */
		USBD_Start(&hUSBDevice_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBD_Result_Ok;
}

TM_USBD_Result_t TM_USBD_Stop(TM_USBD_t USB_Mode) {
#ifdef USB_USE_FS
	/* STOP FS mode */
	if (USB_Mode == TM_USBD_FS || USB_Mode == TM_USBD_Both) {
		/* STOP FS */
		USBD_Stop(&hUSBDevice_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Process HS mode */
	if (USB_Mode == TM_USBD_HS || USB_Mode == TM_USBD_Both) {
		/* STOP HS */
		USBD_Stop(&hUSBDevice_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBD_Result_Ok;
}


USBD_HandleTypeDef* TM_USBD_GetUSBPointer(TM_USBD_t USB_Mode) {
	/* Return pointer to USB handle */
	return hUSBDevices[(uint8_t)USB_Mode];
}
