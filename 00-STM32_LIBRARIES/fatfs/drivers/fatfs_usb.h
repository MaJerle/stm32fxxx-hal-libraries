/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED_USB
#define _DISKIO_DEFINED_USB

#include "diskio.h"
#include "integer.h"

#include "stm32fxxx_hal.h"
#include "defines.h"

#include "tm_stm32_usb.h"
#include "tm_stm32_usb_host.h"
#include "usbh_msc.h"

/* USB timeout max value */
#ifndef FATFS_USB_TIMEOUT
#define FATFS_USB_TIMEOUT	50000
#endif

/*---------------------------------------*/
/* Prototypes for disk control functions */
extern DSTATUS TM_FATFS_USBFS_disk_initialize(void);
extern DSTATUS TM_FATFS_USBFS_disk_status(void);
extern DRESULT TM_FATFS_USBFS_disk_read(BYTE* buff, DWORD sector, UINT count);
extern DRESULT TM_FATFS_USBFS_disk_write(const BYTE* buff, DWORD sector, UINT count);
extern DRESULT TM_FATFS_USBFS_disk_ioctl(BYTE cmd, void* buff);
extern DSTATUS TM_FATFS_USBHS_disk_initialize(void);
extern DSTATUS TM_FATFS_USBHS_disk_status(void);
extern DRESULT TM_FATFS_USBHS_disk_read(BYTE* buff, DWORD sector, UINT count);
extern DRESULT TM_FATFS_USBHS_disk_write(const BYTE* buff, DWORD sector, UINT count);
extern DRESULT TM_FATFS_USBHS_disk_ioctl(BYTE cmd, void* buff);

#endif

