/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/08/hal-library-20-fatfs-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Fatfs implementation for STM32Fxxx devices
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_FATFS_H
#define TM_FATFS_H 100

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
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM SPI           (only when SPI)
 - TM DELAY         (only when SPI)
 - TM GPIO
 - FatFS by Chan
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_gpio.h"
#include "ff.h"
#include "diskio.h"

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
 */
#ifndef FATFS_TRUNCATE_BUFFER_SIZE
#define FATFS_TRUNCATE_BUFFER_SIZE	256
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
 * @example Get memory sizes of USB device:
 *             TM_FATFS_GetDriveSize("USB:", &SizeStruct);
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
 * @retval FRESULT struct members. If everything ok, FR_OK is returned
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
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

#endif

