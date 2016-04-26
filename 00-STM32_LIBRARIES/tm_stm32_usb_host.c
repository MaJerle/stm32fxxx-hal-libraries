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
#include "tm_stm32_usb_host.h"

/* Variables for HCD and main for USB */
#ifdef USB_USE_FS
USBH_HandleTypeDef hUSBHost_FS;
#endif
#ifdef USB_USE_HS
USBH_HandleTypeDef hUSBHost_HS;
#endif

/* Device ready to use and class active status */
static uint8_t USBH_DeviceReady[2] = {0, 0};

/* Create variable with pointers */
static USBH_HandleTypeDef* hUSBHosts[3] = {
#ifdef USB_USE_FS
	&hUSBHost_FS,
#else
	0,
#endif
#ifdef USB_USE_HS
	&hUSBHost_HS,
#else
	0,
#endif
	0
};

/* Private functions */
static void USBH_ProcessCallback(USBH_HandleTypeDef* hUSB, uint8_t id);

TM_USBH_Result_t TM_USBH_Init(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Init FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Init FS */
		USBH_Init(&hUSBHost_FS, USBH_ProcessCallback, USB_ID_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Init HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Init HS */
		USBH_Init(&hUSBHost_HS, USBH_ProcessCallback, USB_ID_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_Result_t TM_USBH_Start(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Start FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Start FS */
		USBH_Start(&hUSBHost_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Start HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Start HS */
		USBH_Start(&hUSBHost_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_Result_t TM_USBH_Restart(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Restart FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Restart FS */
		USBH_ReEnumerate(&hUSBHost_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Restart HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Restart HS */
		USBH_ReEnumerate(&hUSBHost_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_Result_t TM_USBH_Stop(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* STOP FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* STOP FS */
		USBH_Stop(&hUSBHost_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Process HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* STOP HS */
		USBH_Stop(&hUSBHost_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_Result_t TM_USBH_Process(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Process FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Process FS */
		USBH_Process(&hUSBHost_FS);
	}
#endif
	
#ifdef USB_USE_HS
	/* Process HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Process HS */
		USBH_Process(&hUSBHost_HS);
	}
#endif
	
	/* Return OK */
	return TM_USBH_Result_Ok;
}

TM_USBH_Result_t TM_USBH_IsConnected(TM_USB_t USB_Mode) {
	/* Check if any device connected to USB */
	if (hUSBHosts[(uint8_t)USB_Mode]->device.is_connected) {
		/* Return OK */
		return TM_USBH_Result_Ok;
	}
	
	/* Return OK */
	return TM_USBH_Result_Error;
}

TM_USBH_Result_t TM_USBH_IsDeviceReady(TM_USB_t USB_Mode) {
	if (
		hUSBHosts[(uint8_t)USB_Mode]->device.is_connected && /*!< Device is connected */
		USBH_DeviceReady[hUSBHosts[(uint8_t)USB_Mode]->id]   /*!< Class is ready */
	) {
		/* Return OK */
		return TM_USBH_Result_Ok;
	}
	
	/* Return error */
	return TM_USBH_Result_Error;
}

USBH_HandleTypeDef* TM_USBH_GetUSBPointer(TM_USB_t USB_Mode) {
	/* Return pointer to USB handle */
	return hUSBHosts[(uint8_t)USB_Mode];
}

uint16_t TM_USBH_GetVID(TM_USB_t USB_Mode) {
	/* Return vendor ID */
	return hUSBHosts[(uint8_t)USB_Mode]->device.DevDesc.idVendor;
}

uint16_t TM_USBH_GetPID(TM_USB_t USB_Mode) {
	/* Return product ID */
	return hUSBHosts[(uint8_t)USB_Mode]->device.DevDesc.idProduct;
}

/* Private functions */	
static void USBH_ProcessCallback(USBH_HandleTypeDef* hUSB, uint8_t id) {
	/* TO-DO: Call user function */
	/* Reset first */
	switch (id) { 
		case HOST_USER_SELECT_CONFIGURATION:
			break;
		case HOST_USER_DISCONNECTION:
			USBH_DeviceReady[hUSB->id] = 0;
			break;
		case HOST_USER_CLASS_ACTIVE:
			USBH_DeviceReady[hUSB->id] = 1;
			break;
		case HOST_USER_CONNECTION:
			break;
		default:
			break; 
	}
}
