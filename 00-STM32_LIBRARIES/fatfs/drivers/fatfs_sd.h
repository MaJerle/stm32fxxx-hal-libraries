/*-----------------------------------------------------------------------/
/  Low level disk interface module include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED_SD
#define _DISKIO_DEFINED_SD

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "diskio.h"
#include "integer.h"

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_fatfs.h"
#include "tm_stm32_spi.h"
#include "tm_stm32_delay.h"

/* SPI settings */
#ifndef FATFS_SPI
#define FATFS_SPI							SPI1
#define FATFS_SPI_PINSPACK					TM_SPI_PinsPack_1
#endif

/* CS pin settings */
#ifndef FATFS_CS_PIN		
#define FATFS_CS_PORT						GPIOB
#define FATFS_CS_PIN						GPIO_PIN_5
#endif

/* CS pin */
#define FATFS_CS_LOW						TM_GPIO_SetPinLow(FATFS_CS_PORT, FATFS_CS_PIN)
#define FATFS_CS_HIGH						TM_GPIO_SetPinHigh(FATFS_CS_PORT, FATFS_CS_PIN)

#endif

