/**
  ******************************************************************************
  * @file    usbd_msc_storage.c
  * @author  MCD Application Team
  * @version V2.4.1
  * @date    19-June-2015
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_storage.h"
#include "fatfs_sd_sdio.h"
#include "tm_stm32_usb_device_msc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define STORAGE_LUN_NBR                  1  
#define STORAGE_BLK_NBR                  0x10000  
#define STORAGE_BLK_SIZ                  0x200

/* Functions */
int8_t STORAGE_FS_Init(uint8_t lun);
int8_t STORAGE_FS_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_FS_IsReady(uint8_t lun);
int8_t STORAGE_FS_IsWriteProtected(uint8_t lun);
int8_t STORAGE_FS_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_FS_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_FS_GetMaxLun(void);
int8_t STORAGE_HS_Init(uint8_t lun);
int8_t STORAGE_HS_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_HS_IsReady(uint8_t lun);
int8_t STORAGE_HS_IsWriteProtected(uint8_t lun);
int8_t STORAGE_HS_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_HS_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_HS_GetMaxLun(void);

/* USB Mass storage Standard Inquiry Data */
int8_t STORAGE_Inquirydata[] = {//36
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
}; 

USBD_StorageTypeDef USBD_MSC_fops[] = {{
#if defined(USB_USE_FS)
	STORAGE_FS_Init,
	STORAGE_FS_GetCapacity,
	STORAGE_FS_IsReady,
	STORAGE_FS_IsWriteProtected,
	STORAGE_FS_Read,
	STORAGE_FS_Write,
	STORAGE_FS_GetMaxLun,
	STORAGE_Inquirydata
#else
	0,0,0,0,0,0,0,0
#endif
},{
#if defined(USB_USE_HS)
	STORAGE_HS_Init,
	STORAGE_HS_GetCapacity,
	STORAGE_HS_IsReady,
	STORAGE_HS_IsWriteProtected,
	STORAGE_HS_Read,
	STORAGE_HS_Write,
	STORAGE_HS_GetMaxLun,
	STORAGE_Inquirydata
#else
	0,0,0,0,0,0,0,0
#endif
}};

#if defined(USB_USE_FS)
int8_t STORAGE_FS_Init(uint8_t lun) {
	return TM_USBD_MSC_InitCallback(&hUSBDevice_FS, lun);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_Init(uint8_t lun) {
	return TM_USBD_MSC_InitCallback(&hUSBDevice_HS, lun);
}
#endif


#if defined(USB_USE_FS)
int8_t STORAGE_FS_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size) {
	return TM_USBD_MSC_GetCapacityCallback(&hUSBDevice_FS, lun, block_num, block_size);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size) {
	return TM_USBD_MSC_GetCapacityCallback(&hUSBDevice_HS, lun, block_num, block_size);
}
#endif


#if defined(USB_USE_FS)
int8_t STORAGE_FS_IsReady(uint8_t lun) {
	return TM_USBD_MSC_IsReadyCallback(&hUSBDevice_FS, lun);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_IsReady(uint8_t lun) {
	return TM_USBD_MSC_IsReadyCallback(&hUSBDevice_HS, lun);
}
#endif


#if defined(USB_USE_FS)
int8_t STORAGE_FS_IsWriteProtected(uint8_t lun) {
	return TM_USBD_MSC_IsWriteProtectedCallback(&hUSBDevice_FS, lun);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_IsWriteProtected(uint8_t lun) {
	return TM_USBD_MSC_IsWriteProtectedCallback(&hUSBDevice_HS, lun);
}
#endif


#if defined(USB_USE_FS)
int8_t STORAGE_FS_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
	return TM_USBD_MSC_ReadCallback(&hUSBDevice_FS, lun, buf, blk_addr, blk_len);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
	return TM_USBD_MSC_ReadCallback(&hUSBDevice_HS, lun, buf, blk_addr, blk_len);
}
#endif


#if defined(USB_USE_FS)
int8_t STORAGE_FS_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
	return TM_USBD_MSC_ReadCallback(&hUSBDevice_FS, lun, buf, blk_addr, blk_len);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
	return TM_USBD_MSC_ReadCallback(&hUSBDevice_HS, lun, buf, blk_addr, blk_len);
}
#endif


#if defined(USB_USE_FS)
int8_t STORAGE_FS_GetMaxLun(void) {
	return TM_USBD_MSC_GetMaxLunCallback(&hUSBDevice_FS);
}
#endif
#if defined(USB_USE_HS)
int8_t STORAGE_HS_GetMaxLun(void) {
	return TM_USBD_MSC_GetMaxLunCallback(&hUSBDevice_HS);
}
#endif
