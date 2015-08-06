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
#include "tm_stm32_usb_device_cdc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USBD_VID                      0x0483
#define USBD_PID                      0x5740
#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "STMicroelectronics"
#define USBD_PRODUCT_HS_STRING        "STM32 Virtual ComPort in HS Mode"
#define USBD_PRODUCT_FS_STRING        "STM32 Virtual ComPort in FS Mode"
#define USBD_CONFIGURATION_HS_STRING  "VCP Config"
#define USBD_INTERFACE_HS_STRING      "VCP Interface"
#define USBD_CONFIGURATION_FS_STRING  "VCP Config"
#define USBD_INTERFACE_FS_STRING      "VCP Interface"

#define DEVICE_ID1                    (0x1FFF7A10)
#define DEVICE_ID2                    (0x1FFF7A14)
#define DEVICE_ID3                    (0x1FFF7A18)

#define USB_SIZ_STRING_SERIAL         0x1A

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t *USBD_VCP_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#ifdef USB_SUPPORT_USER_STRING_DESC
uint8_t *USBD_VCP_USRStringDesc (USBD_SpeedTypeDef speed, uint8_t idx, uint16_t *length);  
#endif /* USB_SUPPORT_USER_STRING_DESC */  

/* Private variables ---------------------------------------------------------*/
USBD_DescriptorsTypeDef VCP_Desc = {
	USBD_VCP_DeviceDescriptor,
	USBD_VCP_LangIDStrDescriptor, 
	USBD_VCP_ManufacturerStrDescriptor,
	USBD_VCP_ProductStrDescriptor,
	USBD_VCP_SerialStrDescriptor,
	USBD_VCP_ConfigStrDescriptor,
	USBD_VCP_InterfaceStrDescriptor,  
};

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
	#pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
	0x12,                       /* bLength */
	USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
	0x00,                       /* bcdUSB */
	0x02,
	0x00,                       /* bDeviceClass */
	0x00,                       /* bDeviceSubClass */
	0x00,                       /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
	LOBYTE(USBD_VID),           /* idVendor */
	HIBYTE(USBD_VID),           /* idVendor */
	LOBYTE(USBD_PID),           /* idVendor */
	HIBYTE(USBD_PID),           /* idVendor */
	0x00,                       /* bcdDevice rel. 2.00 */
	0x02,
	USBD_IDX_MFC_STR,           /* Index of manufacturer string */
	USBD_IDX_PRODUCT_STR,       /* Index of product string */
	USBD_IDX_SERIAL_STR,        /* Index of serial number string */
	USBD_MAX_NUM_CONFIGURATION  /* bNumConfigurations */
}; /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
	USB_LEN_LANGID_STR_DESC,         
	USB_DESC_TYPE_STRING,       
	LOBYTE(USBD_LANGID_STRING),
	HIBYTE(USBD_LANGID_STRING), 
};

uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] = {
	USB_SIZ_STRING_SERIAL,      
	USB_DESC_TYPE_STRING,    
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

/* Private functions ---------------------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void Get_SerialNum(void);

/* Buffer structure for received data */
#ifdef USB_USE_FS
TM_BUFFER_t USBD_CDC_Buffer_FS_RX;
TM_BUFFER_t USBD_CDC_Buffer_FS_TX;
uint8_t USBD_CDC_Buffer_Data_FS_RX[USBD_CDC_RECEIVE_BUFFER_SIZE_FS];
uint8_t USBD_CDC_Buffer_Data_FS_TX[USBD_CDC_TRANSMIT_BUFFER_SIZE_FS];
#endif
#ifdef USB_USE_HS
TM_BUFFER_t USBD_CDC_Buffer_HS_RX;
TM_BUFFER_t USBD_CDC_Buffer_HS_TX;
uint8_t USBD_CDC_Buffer_Data_HS_RX[USBD_CDC_RECEIVE_BUFFER_SIZE_HS];
uint8_t USBD_CDC_Buffer_Data_HS_TX[USBD_CDC_TRANSMIT_BUFFER_SIZE_HS];
#endif

/************************************************/
/*            USER PUBLIC FUNCTIONS             */
/************************************************/
TM_USBD_Result_t TM_USBD_CDC_Init(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Init FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Init FS */
		USBD_Init(&hUSBDevice_FS, &VCP_Desc, USB_ID_FS);

		/* Add Supported Class */
		USBD_RegisterClass(&hUSBDevice_FS, USBD_CDC_CLASS);

		/* Add CDC Interface Class */
		USBD_CDC_RegisterInterface(&hUSBDevice_FS, &USBD_CDC_fops[0]);
		
		/* Init buffers for TX and RX */
		TM_BUFFER_Init(&USBD_CDC_Buffer_FS_RX, USBD_CDC_RECEIVE_BUFFER_SIZE_FS, USBD_CDC_Buffer_Data_FS_RX, 0);
		TM_BUFFER_Init(&USBD_CDC_Buffer_FS_TX, USBD_CDC_TRANSMIT_BUFFER_SIZE_FS, USBD_CDC_Buffer_Data_FS_TX, 0);
	}
#endif
	
#ifdef USB_USE_HS
	/* Init HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Init HS */
		USBD_Init(&hUSBDevice_HS, &VCP_Desc, USB_ID_HS);

		/* Add Supported Class */
		USBD_RegisterClass(&hUSBDevice_HS, USBD_CDC_CLASS);

		/* Add CDC Interface Class */
		USBD_CDC_RegisterInterface(&hUSBDevice_HS, &USBD_CDC_fops[1]);
		
		/* Init buffers for TX and RX */
		TM_BUFFER_Init(&USBD_CDC_Buffer_HS_RX, USBD_CDC_RECEIVE_BUFFER_SIZE_HS, USBD_CDC_Buffer_Data_HS_RX, 0);
		TM_BUFFER_Init(&USBD_CDC_Buffer_HS_TX, USBD_CDC_TRANSMIT_BUFFER_SIZE_HS, USBD_CDC_Buffer_Data_HS_TX, 0);
	}
#endif
	
	/* Return OK */
	return TM_USBD_Result_Ok;
}

uint16_t TM_USBD_CDC_Puts(TM_USB_t USB_Mode, const char* str) {
	/* Add string to TX buffers */
	
#ifdef USB_USE_FS
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Add to FS TX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_FS_TX, (uint8_t *)str, strlen(str));
		
		/* Update sending if needed */
	}
#endif
	
#ifdef USB_USE_HS
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Add to HS TX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_HS_TX, (uint8_t *)str, strlen(str));
		
		/* Update sending if needed */
	}
#endif
}

uint16_t TM_USBD_CDC_Putc(TM_USB_t USB_Mode, char ch) {
	/* Add string to TX buffers */
	
#ifdef USB_USE_FS
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Add to FS TX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_FS_TX, (uint8_t *)&ch, 1);
		
		/* Update sending */
	}
#endif
	
#ifdef USB_USE_HS
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Add to HS TX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_HS_TX, (uint8_t *)&ch, 1);
		
		/* Update sending */
	}
#endif
}

/************************************************/
/*            BUFFER IMPLEMENTATIONS            */
/************************************************/


/************************************************/
/*               PRIVATE FUNCTIONS              */
/************************************************/
void TM_USBD_CDC_INT_AddToBuffer(USBD_HandleTypeDef* pdev, uint8_t* Values, uint16_t Num) {
#ifdef USB_USE_FS
	if (pdev->id == USB_ID_FS) {
		/* Write to FS RX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_FS_RX, Values, Num);
	}
#endif	
#ifdef USB_USE_HS
	if (pdev->id == USB_ID_HS) {
		/* Write to FS TX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_HS_RX, Values, Num);
	}
#endif
}

/************************************************/
/*       DESCRIPTION BASED IMPLEMENTATION       */
/************************************************/
/**
  * @brief  Returns the device descriptor. 
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	*length = sizeof(USBD_DeviceDesc);
	return (uint8_t*)USBD_DeviceDesc;
}

/**
  * @brief  Returns the LangID string descriptor.        
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	*length = sizeof(USBD_LangIDDesc);  
	return (uint8_t*)USBD_LangIDDesc;
}

/**
  * @brief  Returns the product string descriptor. 
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if(speed == USBD_SPEED_HIGH) {   
		USBD_GetString((uint8_t *)USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);    
	}
	return USBD_StrDesc;
}

/**
  * @brief  Returns the manufacturer string descriptor. 
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
	return USBD_StrDesc;
}

/**
  * @brief  Returns the serial number string descriptor.        
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	*length = USB_SIZ_STRING_SERIAL;

	/* Update the serial number string descriptor with the data from the unique ID */
	Get_SerialNum();

	return (uint8_t*)USBD_StringSerial;
}

/**
  * @brief  Returns the configuration string descriptor.    
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if(speed == USBD_SPEED_HIGH) {  
		USBD_GetString((uint8_t *)USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length); 
	}
	return USBD_StrDesc;  
}

/**
  * @brief  Returns the interface string descriptor.        
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if (speed == USBD_SPEED_HIGH)	{
		USBD_GetString((uint8_t *)USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
	}
	return USBD_StrDesc;  
}

/**
  * @brief  Create the serial number string descriptor 
  * @param  None 
  * @retval None
  */
static void Get_SerialNum(void) {
	uint32_t deviceserial0, deviceserial1, deviceserial2;

	deviceserial0 = *(uint32_t*)DEVICE_ID1;
	deviceserial1 = *(uint32_t*)DEVICE_ID2;
	deviceserial2 = *(uint32_t*)DEVICE_ID3;

	deviceserial0 += deviceserial2;

	if (deviceserial0 != 0) {
		IntToUnicode (deviceserial0, &USBD_StringSerial[2] ,8);
		IntToUnicode (deviceserial1, &USBD_StringSerial[18] ,4);
	}
}

/**
  * @brief  Convert Hex 32Bits value into char 
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer 
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode (uint32_t value, uint8_t *pbuf , uint8_t len) {
	uint8_t idx = 0;

	for (idx = 0; idx < len; idx++) {
		if (((value >> 28)) < 0xA) {
			pbuf[2 * idx] = (value >> 28) + '0';
		} else {
			pbuf[2 * idx] = (value >> 28) + 'A' - 10; 
		}
		
		value = value << 4;

		pbuf[ 2* idx + 1] = 0;
	}
}

