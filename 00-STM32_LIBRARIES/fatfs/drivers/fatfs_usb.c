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
static DSTATUS TM_FATFS_USB_disk_status_lowlevel(USBH_HandleTypeDef* USBHandle, uint8_t pdrv);
static DRESULT TM_FATFS_USB_disk_read_lowlevel( BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle, uint8_t pdrv);
static DRESULT TM_FATFS_USB_disk_write_lowlevel(const BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle, uint8_t pdrv);
static DRESULT TM_FATFS_USB_disk_ioctl_lowlevel (BYTE cmd, void *buff, USBH_HandleTypeDef* USBHandle, uint8_t pdrv);

/*-----------------------------------------------------------------------*/
/* Initialize USB                                                        */
/*-----------------------------------------------------------------------*/
DSTATUS TM_FATFS_USBFS_disk_initialize(void) {
	/* Return OK */
	return RES_OK;
}
DSTATUS TM_FATFS_USBHS_disk_initialize(void) {
	/* Return OK */
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS TM_FATFS_USBFS_disk_status(void) {
	return TM_FATFS_USB_disk_status_lowlevel(&hUSBHost_FS, PDRV_FS);
}
DSTATUS TM_FATFS_USBHS_disk_status(void) {
	return TM_FATFS_USB_disk_status_lowlevel(&hUSBHost_HS, PDRV_HS);
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT TM_FATFS_USBFS_disk_read(BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_read_lowlevel(buff, sector, count, &hUSBHost_FS, PDRV_FS);
}
DRESULT TM_FATFS_USBHS_disk_read(BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_read_lowlevel(buff, sector, count, &hUSBHost_HS, PDRV_HS);
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT TM_FATFS_USBFS_disk_write(const BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_write_lowlevel(buff, sector, count, &hUSBHost_FS, PDRV_FS);
}
DRESULT TM_FATFS_USBHS_disk_write(const BYTE *buff, DWORD sector, UINT count) {
	return TM_FATFS_USB_disk_write_lowlevel(buff, sector, count, &hUSBHost_HS, PDRV_HS);
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT TM_FATFS_USBFS_disk_ioctl (BYTE cmd, void *buff) {
	return TM_FATFS_USB_disk_ioctl_lowlevel(cmd, buff, &hUSBHost_FS, PDRV_FS);
}
DRESULT TM_FATFS_USBHS_disk_ioctl (BYTE cmd, void *buff) {
	return TM_FATFS_USB_disk_ioctl_lowlevel(cmd, buff, &hUSBHost_HS, PDRV_HS);
}
#endif

/********************************************************************/
/*                         LOW LEVEL DRIVERS                        */
/********************************************************************/
static DSTATUS TM_FATFS_USB_disk_status_lowlevel(USBH_HandleTypeDef* USBHandle, uint8_t pdrv) {
	DRESULT res = RES_ERROR;

	/* Check if MSC is ready */
	if (USBH_MSC_UnitIsReady(USBHandle, pdrv)) {
		res = RES_OK;
	} else {
		res = RES_ERROR;
	}

	return res;
}

static DRESULT TM_FATFS_USB_disk_read_lowlevel(BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle, uint8_t pdrv) {
	DRESULT res = RES_ERROR;
	MSC_LUNTypeDef info;
	USBH_StatusTypeDef  status = USBH_OK;
	DWORD scratch [_MAX_SS / 4];

	/* DMA Alignment issue, do single up to aligned buffer */
	if ((DWORD)buff & 3) { 
		while ((count--) && (status == USBH_OK)) {
			status = USBH_MSC_Read(USBHandle, pdrv, sector + count, (uint8_t *)scratch, 1);
			if (status == USBH_OK) {
				memcpy(&buff[count * _MAX_SS], scratch, _MAX_SS);
			} else {
				break;
			}
		}
	} else {
		status = USBH_MSC_Read(USBHandle, pdrv, sector, buff, count);
	}

	if (status == USBH_OK) {
		res = RES_OK;
	} else {
		USBH_MSC_GetLUNInfo(USBHandle, pdrv, &info); 

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
static DRESULT TM_FATFS_USB_disk_write_lowlevel(const BYTE *buff, DWORD sector, UINT count, USBH_HandleTypeDef* USBHandle, uint8_t pdrv) {
	DRESULT res = RES_ERROR; 
	MSC_LUNTypeDef info;
	USBH_StatusTypeDef  status = USBH_OK;  
	DWORD scratch [_MAX_SS / 4];  

	/* DMA Alignment issue, do single up to aligned buffer */
	if ((DWORD)buff & 3) {
		while (count--) {
			memcpy(scratch, &buff[count * _MAX_SS], _MAX_SS);

			status = USBH_MSC_Write(USBHandle, pdrv, sector + count, (BYTE *)scratch, 1);
			if (status == USBH_FAIL) {
				break;
			}
		}
	} else {
		status = USBH_MSC_Write(USBHandle, pdrv, sector, (BYTE *)buff, count);
	}

	if (status == USBH_OK) {
		res = RES_OK;
	} else {
		USBH_MSC_GetLUNInfo(USBHandle, pdrv, &info); 

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

static DRESULT TM_FATFS_USB_disk_ioctl_lowlevel (BYTE cmd, void *buff, USBH_HandleTypeDef* USBHandle, uint8_t pdrv) {
	DRESULT res = RES_OK;
	MSC_LUNTypeDef info;

	switch (cmd) {
		/* Make sure that no pending write process */  
		case CTRL_SYNC:		
			res = RES_OK;
			break;

		/* Get number of sectors on the disk (DWORD) */ 
		case GET_SECTOR_COUNT:	
			if (USBH_MSC_GetLUNInfo(USBHandle, pdrv, &info) == USBH_OK) {
				*(DWORD *)buff = info.capacity.block_nbr;
				res = RES_OK;
			} else {
				res = RES_ERROR;
			}
			break;

		/* Get R/W sector size (WORD) */
		case GET_SECTOR_SIZE :	
			if (USBH_MSC_GetLUNInfo(USBHandle, pdrv, &info) == USBH_OK) {
				*(DWORD *)buff = info.capacity.block_size;
				res = RES_OK;
			} else {
				res = RES_ERROR;
			}
			break;

		/* Get erase block size in unit of sector (DWORD) */  
		case GET_BLOCK_SIZE:	
			if (USBH_MSC_GetLUNInfo(USBHandle, pdrv, &info) == USBH_OK) {
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

