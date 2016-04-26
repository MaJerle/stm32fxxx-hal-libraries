/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/08/hal-library-20-fatfs-for-stm32fxxx/
 * @version v1.1
 * @ide     Keil uVision
 * @license MIT
 * @brief   Fatfs implementation for STM32Fxxx devices
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_FATFS_H
#define TM_FATFS_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_FATFS
 * @brief    FATFS implementation for STM32Fxxx devices - http://stm32f4-discovery.com/2015/08/hal-library-20-fatfs-for-stm32fxxx/
 * @{
 *
 * FatFs implementation for STM32F4xx devices
 *
 * This library uses Chan's Fatfs implementation.
 * 
 * This library is only for communication. To work with files, you have to look at Chan's FatFs manual, link below:
 * http://elm-chan.org/fsw/ff/00index_e.html
 *
 * You can work with SPI or SDIO protocol to interface SDCARD.
 *
 * \par SDCARD pinouts
 *
 * Library works with SPI or SDIO mode. Also, when in SDIO mode, you can set to 1- or 4-bit mode.
 * By default, SDIO with 4-bit mode is used, so you will look into right column on table below.
 * 
 * SD CARD PINS
 * 	   
\verbatim
   _________________
  / 1 2 3 4 5 6 7 8 |  NR   |SDIO INTERFACE                               |SPI INTERFACE
 /                  |       |NAME     STM32Fxxx     DESCRIPTION           |NAME   STM32Fxxx   DESCRIPTION 
/ 9                 |       |         4-BIT  1-BIT                        | 
|                   |       |                                             | 
|                   |   1   |CD/DAT3  PC11   -      Connector data line 3 |CS     PB5         Chip select for SPI 
|                   |   2   |CMD      PD2    PD2    Command/Response line |MOSI   PA7         Data input for SPI 
|                   |   3   |VSS1     GND    GND    GND                   |VSS1   GND         GND 
|   SD CARD Pinout  |   4   |VDD      3.3V   3.3V   3.3V Power supply     |VDD    3.3V        3.3V Power supply 
|                   |   5   |CLK      PC12   PC12   Clock                 |SCK    PA5         Clock for SPI 
|                   |   6   |VSS2     GND    GND    GND                   |VSS2   GND         GND 
|                   |   7   |DAT0     PC8    PC8    Connector data line 0 |MISO   PA6         Data output for SPI 
|                   |   8   |DAT1     PC9    -      Connector data line 1 |-      -           - 
|___________________|   9   |DAT2     PC10   -      Connector data line 2 |-      -           - 
\endverbatim
 *	
 * \par SDIO Communication
 * 
 * By default, SDIO with 4-bit communication is used.
 * If you want to use SDIO 1-bit communication, set defines below in your defines.h file:
 * 
\code
//Set SDIO with 1-bit communication
#define FATFS_SDIO_4BIT   0
\endcode
 * 
 * For SDIO communication, you will need at least these files:
 *
\verbatim
- tm_stm32_fatfs.h
- tm_stm32_fatfs.c
- fatfs/diskio.h
- fatfs/diskio.c
- fatfs/ff.h
- fatfs/ff.c
- fatfs/ffconf.h
- fatfs/integer.h
- fatfs/option/syscall.c
- fatfs/option/unicode.c
- fatfs/drivers/fatfs_sd_sdio.h
- fatfs/drivers/fatfs_sd_sdio.c
\endverbatim 
 *
 * \par SPI Communication
 * 
 * Or, if you want to use SPI communication, you have to add lines below in your defines.h file
 *	
\code
//Enable SPI communication, disable SDIO
#define FATFS_USE_SDIO   0
\endcode
 *	
 * Files, needed for SPI:
 *
\verbatim
- tm_stm32_fatfs.h
- tm_stm32_fatfs.c
- fatfs/diskio.h
- fatfs/diskio.c
- fatfs/ff.h
- fatfs/ff.c
- fatfs/ffconf.h
- fatfs/integer.h
- fatfs/option/syscall.c
- fatfs/option/unicode.c
- fatfs/drivers/fatfs_sd.h
- fatfs/drivers/fatfs_sd.c
\endverbatim 
 * 
 * \par Overwriting default pinout
 * 
 * SDIO interface pins are fixed, and can not be changed.
 * If you want to change SPI pins, you have to set these defines in your defines.h file:
 * 
\code
//Set your SPI, for corresponding pins look at TM SPI library
#define FATFS_SPI               SPI1
#define FATFS_SPI_PINSPACK      TM_SPI_PinsPack_1
	
//Set your CS pin for SPI			
#define FATFS_CS_PORT           GPIOB
#define FATFS_CS_PIN            GPIO_PIN_5
\endcode
 *
 * \par Write protect and Card detect pins
 *
 * Library has support for Card detect and Write protect pins.
 *
 * It is the same for any communication used and is disabled by default.
 * 
\verbatim
NAME    STM32Fxxx   DESCRIPTION
	
CD      PB6         Card detect pin. Pin low when card detected
CD      PB7         Card write protect pin. Pin low when card write is enabled
\endverbatim
 *
 * Like I said before, these 2 pins are disabled by default. If you want to use it, you have to add 2 lines in your defines.h file:
 *
\code
//Enable Card detect pin
#define FATFS_USE_DETECT_PIN               1

//Enable Write protect pin
#define FATFS_USE_WRITEPROTECT_PIN         1
\endcode
 * 
 * WP and CD pins are now enabled with default configuration.
 *
 * Add lines below to your defines.h file only if you want to overwrite default pin settings:
 *
\code
//Default CD pin			
#define FATFS_DETECT_PORT          GPIOB
#define FATFS_DETECT_PIN           GPIO_PIN_6

//Default WP pin			
#define FATFS_WRITEPROTECT_PORT    GPIOB
#define FATFS_WRITEPROTECT_PIN     GPIO_PIN_7
\endcode
 *
 * \par Timing function for files
 * 
 * FatFs uses function get_fattime() for time, to set timestamp for files, when they were edited or created.
 * 
 * By default, function returns 0, but if you want to create your own function, you have to set defines in defines.h file:
 * 
\code
//Use custom get_fattime() function
#define FATFS_CUSTOM_FATTIME	1
\endcode
 *
 * And then somewhere in your project, add function like below:
 * 
\code
//Use custom get_fattime function
//Implement RTC get time here if you need it
DWORD get_fattime (void) {
	//Get your time from RTC or something like that
	
return  ((DWORD)(2014 - 1980) << 25)  // Year 2014
        | ((DWORD)7 << 21)            // Month 7
        | ((DWORD)10 << 16)           // Mday 10
        | ((DWORD)16 << 11)           // Hour 16
        | ((DWORD)0 << 5)             // Min 0
        | ((DWORD)0 >> 1);            // Sec 0
}
\endcode
 *
 * \par Use FATFS with USB MSC Host library.
 * 
 * You can use this library also with @ref TM_USBH_MSC Library if you want to ready USB Flash keys when needed. 
 *
 * First, files you will need for implementation based on FATFS are:
 *
\verbatim
- tm_stm32_fatfs.h
- tm_stm32_fatfs.c
- fatfs/diskio.h
- fatfs/diskio.c
- fatfs/ff.h
- fatfs/ff.c
- fatfs/ffconf.h
- fatfs/integer.h
- fatfs/option/syscall.c
- fatfs/option/unicode.c
- fatfs/drivers/fatfs_usb.h
- fatfs/drivers/fatfs_usb.c
- Entire USB MSC Host stack and TM USB library
\endverbatim 
 *
 * \par Operate with USB MSC Host libraries.
 *
 * Because my USB MSC Host library supports both USB modes at the same time, I've made 2 different names for logical drivers.
 * So, when you wanna mount/read/write/etc, you have 2 possible drive names:
 *
\code
//Mount USB Flash memory connected on USB FS port
f_mount(&fatfs_FS, "USBFS:", 1);

//Mount USB Flash memory connected on USB HS port
f_mount(&fatfs_HS, "USBHS:", 1);
\endcode
 *
 * Like SDCARD has "SD:" name, here are 2 different names, which allows you flexibility in your code.
 * This also means, that you can use SDCARD and 2 USB flash drives at the same time without any problems, just specifying drive name
 * when performing read and write operations.
 *
 * \par Search for files
 *
 * I've added support for easy search function for files/folders on your FATFS related device. It works with any device (SDCARD, USB, etc),
 * because you specify starting path for search which is specified by FATFS.
 *
 * There are 2 functions for search, one is @ref TM_FATFS_Search which you should call when you want to do a search operations in your FATFS structure
 *
 * Second is @ref TM_FATFS_SearchCallback which is called anytime file/folder is found on system so user can do his job with file.
 *
\code
int user_func(void) {
	//Create working buffer
	char working_buffer[200];
	
	FRESULT res;
	TM_FATFS_Search_t FindStructure;
	
	//mount first
	
	if ((res = TM_FATFS_Search("SD:", working_buffer, sizeof(working_buffer), &FindStructure)) == FR_OK) {
		//Search was OK
	} else if (res == FR_NOT_ENOUGH_CORE) {
		//Not enough buffer memory for full search operation
	}
	
	//unmount
}

uint8_t TM_FATFS_SearchCallback(char* path, uint8_t is_file, TM_FATFS_Search_t* FindStructure) {
	//Check for file/folder
	if (is_file) {
		printf("File: %s", path);
	} else {
		printf("Folder: %s", path);
	}

	//Allow next search
	return 1;
}
\endcode
 *
 * Check documentation for these 2 functions for more info.
 * 
 * \par Changelog
 *
\verbatim
 Version 1.1
  - April 24, 2016
  - Added support for FATFS R0.12
  - Added support for SPI DMA when using fatfs in SPI mode on STM32F4xx and STM32F7xx devices

 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - attributes.h
 - TM DMA
 - TM SPI           (only when SPI)
 - TM SPI DMA       (only when SPI on STM32F4xx and STM32F7xx)
 - TM DELAY         (only when SPI)
 - TM GPIO
 - FatFS by Chan    (R0.11a)
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "attributes.h"
#include "tm_stm32_gpio.h"
#include "ff.h"
#include "diskio.h"
#include "string.h"
#include "stdlib.h"

/**
 * @defgroup TM_FATFS_Macros
 * @brief    Library defines
 * @{
 */

/** 
 * @brief  Default truncate buffer size in bytes
 * @note   If you have in plan to truncate from beginning large files a lot of times,
 *         then you should think about increasing this value as far as possible.
 *         With larger buffer size you will get faster response with truncating.
 *         Suggested value is multiply by 512
 */
#ifndef FATFS_TRUNCATE_BUFFER_SIZE
#define FATFS_TRUNCATE_BUFFER_SIZE	256
#endif

/* Memory allocation function */
#ifndef LIB_ALLOC_FUNC
#define LIB_ALLOC_FUNC    malloc
#endif

/* Memory free function */
#ifndef LIB_FREE_FUNC
#define LIB_FREE_FUNC     free
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_FATFS_Typedefs
 * @brief    Library typedefs
 * @{
 */

/**
 * @brief  FATFS size structure
 */
typedef struct {
	uint32_t Total; /*!< Total size of memory */
	uint32_t Free;  /*!< Free size of memory */
} TM_FATFS_Size_t;

/**
 * @brief  FATFS find structure
 */
typedef struct {
	uint32_t FoldersCount; /*!< Number of folders in last search operation */
	uint32_t FilesCount;   /*!< Number of files in last search operation */
} TM_FATFS_Search_t;

/**
 * @}
 */

/**
 * @defgroup TM_FATFS_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief   Gets total and free memory sizes of any drive
 * @param   *str: Pointer to string for drive to be checked
 * @param   *SizeStruct: Pointer to empty @ref TM_FATFS_Size_t structure to store data about memory
 * @retval  FRESULT structure members. If data are valid, FR_OK is returned
 * @example Get memory sizes of USB device: TM_FATFS_GetDriveSize("USBFS:", &SizeStruct);
 */
FRESULT TM_FATFS_GetDriveSize(char* str, TM_FATFS_Size_t* SizeStruct);

/**
 * @brief  Truncates beginning of file
 *
 * Example:
 *	- You have a file, its content is: "abcdefghijklmnoprstuvwxyz",
 *	- You want to truncate first 5 bytes,
 *	- Call @ref TM_FATFS_TruncateBeginning(&opened_file, 5);
 *	- You will get new file data: "fghijklmnoprstuvwxyz"
 
 * @param  *fil: Pointer to already opened file
 * @param  index: Number of characters that will be truncated from beginning
 * @note   If index is more than file size, everything will be truncated, but file will not be deleted
 * @retval Member of @ref FRESULT enumeration
 */
FRESULT TM_FATFS_TruncateBeginning(FIL* fil, uint32_t index);

/**
 * @brief  Checks card detect pin (if activated) if card is inserted
 * @note   Pin must be set low in order to get card inserted, otherwise card is not inserted
 * @note   Card detect pin must be activated in order to get this functionality to work
 * @param  None
 * @retval Card detected status:
 *            - 0: Card is not inserted
 *            - > 0: Card is inserted
 */
uint8_t TM_FATFS_CheckCardDetectPin(void);

/**
 * @brief  Searches on SD card for files and folders
 * @note   It will search recursive till the end of everything or if tmp_buffer is full
 * @param  *Folder: Folder start location where search will be performed
 * @param  *tmp_buffer: Pointer to empty buffer where temporary data for filepath will be stored. It's size must be larger than bigest filepath on FATFS.
 *            Set this parameter to NULL and function will use @ref LIB_ALLOC_FUNC() to allocate memory for tmp buffer of size @arg tmp_buffer_size
 * @param  tmp_buffer_size: Number of bytes reserver for tmp_buffer so we won't overlaps buffer
 * @param  *FindStructure: Pointer to @ref TM_FATFS_Search_t structure
 * @retval Member of @ref FRESULT enumeration
 */
FRESULT TM_FATFS_Search(char* Folder, char* tmp_buffer, uint16_t tmp_buffer_size, TM_FATFS_Search_t* FindStructure);

/**
 * @brief  Search procedure callback function with filename result
 * @param  *path: Full path and file/folder name from search operation
 * @param  is_file: Is item a file or directory:
 *            - 0: Item is folder
 *            - > 0: Item is file
 * @param  Pointer to @ref TM_FATFS_Search_t structure which was passed to @ref TM_FATFS_Search with updated data
 * @retval Search status:
 *            - 0: Stop search operation
 *            - > 0: Continue with search
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
uint8_t TM_FATFS_SearchCallback(char* path, uint8_t is_file, TM_FATFS_Search_t* FindStructure);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

