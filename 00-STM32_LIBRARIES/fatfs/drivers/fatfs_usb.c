/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
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
#include "fatfs_usb.h"

/* Driver values for USB ports */
#define PDRV_FS    0
#define PDRV_HS    0

/* Function implementations */
static DSTATUS TM_FATFS_USB_disk_initialize_lowlevel(USBH_HandleTypeDef* USBHandle);
static DSTATUS TM_FATFS_USB_disk_status_lowlevel(USBH_HandleTypeDef* USBHandle);
static DRESULT TM_FATFS_USB_disk_read_lowlevel( BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle);
static DRESULT TM_FATFS_USB_disk_write_lowlevel(const BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle);
static DRESULT TM_FATFS_USB_disk_ioctl_lowlevel (BYTE cmd, void *buff, USBH_HandleTypeDef* USBHandle);

/*-----------------------------------------------------------------------*/
/* Initialize USB                                                        */
/*-----------------------------------------------------------------------*/
DSTATUS TM_FATFS_USBFS_disk_initialize(void) {
	return TM_FATFS_USB_disk_initialize_lowlevel(&hUSBHost_FS);
}
DSTATUS TM_FATFS_USBHS_disk_initialize(void) {
	return TM_FATFS_USB_disk_initialize_lowlevel(&hUSBHost_HS);
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS TM_FATFS_USBFS_disk_status(void) {
	return TM_FATFS_USB_disk_status_lowlevel(&hUSBHost_FS);
}
DSTATUS TM_FATFS_USBHS_disk_status(void) {
	return TM_FATFS_USB_disk_status_lowlevel(&hUSBHost_HS);
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT TM_FATFS_USBFS_disk_read(BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_read_lowlevel(buff, sector, count, &hUSBHost_FS);
}
DRESULT TM_FATFS_USBHS_disk_read(BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_read_lowlevel(buff, sector, count, &hUSBHost_HS);
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT TM_FATFS_USBFS_disk_write(const BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_write_lowlevel(buff, sector, count, &hUSBHost_FS);
}
DRESULT TM_FATFS_USBHS_disk_write(const BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_write_lowlevel(buff, sector, count, &hUSBHost_HS);
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT TM_FATFS_USBFS_disk_ioctl (BYTE cmd, void *buff) {
	return TM_FATFS_USB_disk_ioctl_lowlevel(cmd, buff, &hUSBHost_FS);
}
DRESULT TM_FATFS_USBHS_disk_ioctl (BYTE cmd, void *buff) {
	return TM_FATFS_USB_disk_ioctl_lowlevel(cmd, buff, &hUSBHost_HS);
}
#endif

/********************************************************************/
/*                         LOW LEVEL DRIVERS                        */
/********************************************************************/
static DSTATUS TM_FATFS_USB_disk_initialize_lowlevel(USBH_HandleTypeDef* USBHandle) {
	return RES_OK;
}

static DSTATUS TM_FATFS_USB_disk_status_lowlevel(USBH_HandleTypeDef* USBHandle) {
	MSC_HandleTypeDef *MSC_Handle;
	
	/* Check active class */
	if (USBHandle->pActiveClass != USBH_MSC_CLASS) {
		return RES_ERROR;
	}
	
	/* Create MSC handle */
	MSC_Handle = (MSC_HandleTypeDef *) USBHandle->pActiveClass->pData;
	
	/* Check if MSC is ready */
	if (USBH_MSC_UnitIsReady(USBHandle, MSC_Handle->current_lun)) {
		return RES_OK;
	}

	return RES_ERROR;
}

static DRESULT TM_FATFS_USB_disk_read_lowlevel(BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle) {
	DRESULT res = RES_ERROR;
	MSC_LUNTypeDef info;
	USBH_StatusTypeDef  status = USBH_OK;
	DWORD scratch [_MAX_SS / 4];
	MSC_HandleTypeDef *MSC_Handle;
	
	/* Check active class */
	if (USBHandle->pActiveClass != USBH_MSC_CLASS) {
		return RES_ERROR;
	}

	/* Create MSC handle */
	MSC_Handle = (MSC_HandleTypeDef *) USBHandle->pActiveClass->pData;

	/* DMA Alignment issue, do single up to aligned buffer */
	if ((DWORD)buff & 3) { 
		while ((count--) && (status == USBH_OK)) {
			status = USBH_MSC_Read(USBHandle, MSC_Handle->current_lun, sector + count, (uint8_t *)scratch, 1);
			if (status == USBH_OK) {
				memcpy(&buff[count * _MAX_SS], scratch, _MAX_SS);
			} else {
				break;
			}
		}
	} else {
		status = USBH_MSC_Read(USBHandle, MSC_Handle->current_lun, sector, buff, count);
	}

	if (status == USBH_OK) {
		res = RES_OK;
	} else {
		USBH_MSC_GetLUNInfo(USBHandle, MSC_Handle->current_lun, &info); 

		switch (info.sense.asc) {
			case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
			case SCSI_ASC_MEDIUM_NOT_PRESENT:
			case SCSI_ASC_NOT_READY_TO_READY_CHANGE: 
				USBH_ErrLog("USB Disk is not ready!");  
				res = RES_NOTRDY;
				break; 

			default:
				res = RES_ERROR;
				break;
		}
	}

	return res;
}

#if _USE_WRITE
static DRESULT TM_FATFS_USB_disk_write_lowlevel(const BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle) {
	DRESULT res = RES_ERROR; 
	MSC_LUNTypeDef info;
	USBH_StatusTypeDef  status = USBH_OK;  
	DWORD scratch [_MAX_SS / 4];
	MSC_HandleTypeDef *MSC_Handle;
	
	/* Check active class */
	if (USBHandle->pActiveClass != USBH_MSC_CLASS) {
		return RES_ERROR;
	}

	/* Create MSC handle */
	MSC_Handle = (MSC_HandleTypeDef *) USBHandle->pActiveClass->pData;

	/* DMA Alignment issue, do single up to aligned buffer */
	if ((DWORD)buff & 3) {
		while (count--) {
			memcpy(scratch, &buff[count * _MAX_SS], _MAX_SS);

			status = USBH_MSC_Write(USBHandle, MSC_Handle->current_lun, sector + count, (BYTE *)scratch, 1);
			if (status == USBH_FAIL) {
				break;
			}
		}
	} else {
		status = USBH_MSC_Write(USBHandle, MSC_Handle->current_lun, sector, (BYTE *)buff, count);
	}

	if (status == USBH_OK) {
		res = RES_OK;
	} else {
		USBH_MSC_GetLUNInfo(USBHandle, MSC_Handle->current_lun, &info); 

		switch (info.sense.asc) {
			case SCSI_ASC_WRITE_PROTECTED:
				USBH_ErrLog("USB Disk is Write protected!");
				res = RES_WRPRT;
				break;

			case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
			case SCSI_ASC_MEDIUM_NOT_PRESENT:
			case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
				USBH_ErrLog("USB Disk is not ready!");      
				res = RES_NOTRDY;
				break; 

			default:
				res = RES_ERROR;
				break;
		}
	}

	return res;   
}
#endif

static DRESULT TM_FATFS_USB_disk_ioctl_lowlevel (BYTE cmd, void *buff, USBH_HandleTypeDef* USBHandle) {
	DRESULT res = RES_OK;
	MSC_LUNTypeDef info;
	MSC_HandleTypeDef *MSC_Handle;
	
	/* Check active class */
	if (USBHandle->pActiveClass != USBH_MSC_CLASS) {
		return RES_ERROR;
	}
	
	/* Create MSC handle */
	MSC_Handle = (MSC_HandleTypeDef *) USBHandle->pActiveClass->pData;
	
	switch (cmd) {
		/* Make sure that no pending write process */  
		case CTRL_SYNC:		
			res = RES_OK;
			break;

		/* Get number of sectors on the disk (DWORD) */ 
		case GET_SECTOR_COUNT:	
			if (USBH_MSC_GetLUNInfo(USBHandle, MSC_Handle->current_lun, &info) == USBH_OK) {
				*(DWORD *)buff = info.capacity.block_nbr;
				res = RES_OK;
			} else {
				res = RES_ERROR;
			}
			break;

		/* Get R/W sector size (WORD) */
		case GET_SECTOR_SIZE :	
			if (USBH_MSC_GetLUNInfo(USBHandle, MSC_Handle->current_lun, &info) == USBH_OK) {
				*(DWORD *)buff = info.capacity.block_size;
				res = RES_OK;
			} else {
				res = RES_ERROR;
			}
			break;

		/* Get erase block size in unit of sector (DWORD) */  
		case GET_BLOCK_SIZE:	
			if (USBH_MSC_GetLUNInfo(USBHandle, MSC_Handle->current_lun, &info) == USBH_OK) {
				*(DWORD *)buff = info.capacity.block_size;
				res = RES_OK;
			} else {
				res = RES_ERROR;
			}
			break;

		default:
			res = RES_PARERR;
	}

	return res;
}

