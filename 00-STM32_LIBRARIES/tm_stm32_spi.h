/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/07/hal-library-08-spi-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   SPI library for STM32Fxxx
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
#ifndef TM_SPI_H
#define TM_SPI_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_SPI
 * @brief    SPI library for STM32Fxxx - http://stm32f4-discovery.com/2015/07/hal-library-08-spi-for-stm32fxxx/
 * @{
 *
 * It supports all 6 SPIs in master with 3 Lines Full Duplex mode
 *
 * \par Default SPI settings
 *
 * All six SPIs work the same principle by default:
 *	- Master mode
 *	- 8 data bits
 *	- Clock polarity low, data sampled at first edge, SPI mode 0
 *	- Prescaler set to 32
 *	- Firstbit MSB
 *	- Software SS pin configure
 *	- Direction is full duplex 3 wires
 *	
 *	\par Pinout
 *
\verbatim
        |PINS PACK 1            |PINS PACK 2            |PINS PACK 3           |PINS PACK 4
SPIX    |MOSI   MISO    SCK     |MOSI   MISO    SCK     |MOSI   MISO    SCK    |MOSI   MISO    SCK
        |                                                                           
SPI1    |PA7    PA6     PA5     |PB5    PB4     PB3     |                      |
SPI2    |PC3    PC2     PB10    |PB15   PB14    PB13    |PI3    PI2     PI0    |PB15   PB14    PI1
SPI3    |PB5    PB4     PB3     |PC12   PC11    PC10    |                      |
SPI4    |PE6    PE5     PE2     |PE14   PE13    PE12    |                      |
SPI5    |PF9    PF8     PF7     |PF11   PH7     PH6     |                      |
SPI6    |PG14   PG12    PG13    |
\endverbatim
 *
 * In case these pins are not good for you, you can use
 * TM_SPI_PinsPack_Custom in function and callback function will be called,
 * where you can initialize your custom pinout for your SPI peripheral
 *	
 * Possible changes to each SPI. Set this defines in your defines.h file.
 *	
 * Change x with 1-6, to match your SPI
 *
\code
//Default prescaler
#define TM_SPIx_PRESCALER   SPI_BAUDRATEPRESCALER_32
//Specify datasize
#define TM_SPIx_DATASIZE    SPI_DATASIZE_8BIT
//Specify which bit is first
#define TM_SPIx_FIRSTBIT    SPI_FIRSTBIT_MSB
//Mode, master or slave
#define TM_SPIx_MASTERSLAVE SPI_MODE_MASTER
//Specify mode of operation, clock polarity and clock phase
#define TM_SPIx_MODE        TM_SPI_Mode_0
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
 - attributes.h
 - TM GPIO
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "attributes.h"
#include "tm_stm32_gpio.h"

/**
 * @defgroup TM_SPI_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  SPI modes selection
 */
typedef enum {
	TM_SPI_Mode_0 = 0x00, /*!< Clock polarity low, clock phase 1st edge */
	TM_SPI_Mode_1,        /*!< Clock polarity low, clock phase 2nd edge */
	TM_SPI_Mode_2,        /*!< Clock polarity high, clock phase 1st edge */
	TM_SPI_Mode_3         /*!< Clock polarity high, clock phase 2nd edge */
} TM_SPI_Mode_t;

/**
 * @brief  SPI PinsPack enumeration to select pins combination for SPI
 */
typedef enum {
	TM_SPI_PinsPack_1 = 0x00, /*!< Select PinsPack1 from Pinout table for specific SPI */
	TM_SPI_PinsPack_2,        /*!< Select PinsPack2 from Pinout table for specific SPI */
	TM_SPI_PinsPack_3,        /*!< Select PinsPack3 from Pinout table for specific SPI */
	TM_SPI_PinsPack_4,        /*!< Select PinsPack4 from Pinout table for specific SPI */
	TM_SPI_PinsPack_Custom    /*!< Select custom pins for specific SPI, callback will be called, look @ref TM_SPI_InitCustomPinsCallback */
} TM_SPI_PinsPack_t;

/**
 * @brief  Daza size enumeration
 */
typedef enum {
	TM_SPI_DataSize_8b = 0x00, /*!< SPI in 8-bits mode */
	TM_SPI_DataSize_16b        /*!< SPI in 16-bits mode */        
} TM_SPI_DataSize_t;

/**
 * @}
 */
 
 /**
 * @defgroup TM_SPI_Macros
 * @brief    Library defines
 * @{
 */

//----- SPI1 options start -------
//Options can be overwriten in defines.h file
#ifndef TM_SPI1_PRESCALER
#define TM_SPI1_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef TM_SPI1_DATASIZE
#define TM_SPI1_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef TM_SPI1_FIRSTBIT
#define TM_SPI1_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef TM_SPI1_MASTERSLAVE
#define TM_SPI1_MASTERSLAVE	SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef TM_SPI1_MODE
#define TM_SPI1_MODE		TM_SPI_Mode_0
#endif
//----- SPI1 options end -------

//----- SPI2 options start -------
//Options can be overwriten in defines.h file
#ifndef TM_SPI2_PRESCALER
#define TM_SPI2_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef TM_SPI2_DATASIZE
#define TM_SPI2_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef TM_SPI2_FIRSTBIT
#define TM_SPI2_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef TM_SPI2_MASTERSLAVE
#define TM_SPI2_MASTERSLAVE SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef TM_SPI2_MODE
#define TM_SPI2_MODE		TM_SPI_Mode_0
#endif
//----- SPI2 options end -------

//----- SPI3 options start -------
//Options can be overwriten in defines.h file
#ifndef TM_SPI3_PRESCALER
#define TM_SPI3_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef TM_SPI3_DATASIZE
#define TM_SPI3_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef TM_SPI3_FIRSTBIT
#define TM_SPI3_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef TM_SPI3_MASTERSLAVE
#define TM_SPI3_MASTERSLAVE SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef TM_SPI3_MODE
#define TM_SPI3_MODE		TM_SPI_Mode_0
#endif
//----- SPI3 options end -------

//----- SPI4 options start -------
//Options can be overwriten in defines.h file
#ifndef TM_SPI4_PRESCALER
#define TM_SPI4_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef TM_SPI4_DATASIZE
#define TM_SPI4_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef TM_SPI4_FIRSTBIT
#define TM_SPI4_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef TM_SPI4_MASTERSLAVE
#define TM_SPI4_MASTERSLAVE	SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef TM_SPI4_MODE
#define TM_SPI4_MODE		TM_SPI_Mode_0
#endif
//----- SPI4 options end -------

//----- SPI5 options start -------
//Options can be overwriten in defines.h file
#ifndef TM_SPI5_PRESCALER
#define TM_SPI5_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef TM_SPI5_DATASIZE
#define TM_SPI5_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef TM_SPI5_FIRSTBIT
#define TM_SPI5_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef TM_SPI5_MASTERSLAVE
#define TM_SPI5_MASTERSLAVE SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef TM_SPI5_MODE
#define TM_SPI5_MODE		TM_SPI_Mode_0
#endif
//----- SPI5 options end -------

//----- SPI6 options start -------
//Options can be overwriten in defines.h file
#ifndef TM_SPI6_PRESCALER
#define TM_SPI6_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef TM_SPI6_DATASIZE
#define TM_SPI6_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef TM_SPI6_FIRSTBIT
#define TM_SPI6_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef TM_SPI6_MASTERSLAVE
#define TM_SPI6_MASTERSLAVE SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef TM_SPI6_MODE
#define TM_SPI6_MODE		TM_SPI_Mode_0
#endif
//----- SPI6 options end -------

/**
 * @brief  Check SPI busy status
 */
#define SPI_IS_BUSY(SPIx)                   (((SPIx)->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0)

/**
 * @brief  SPI wait till end
 */
#define SPI_WAIT_TX(SPIx)                   while ((SPIx->SR & SPI_FLAG_TXE) == 0 || (SPIx->SR & SPI_FLAG_BSY))
#define SPI_WAIT_RX(SPIx)                   while ((SPIx->SR & SPI_FLAG_RXNE) == 0 || (SPIx->SR & SPI_FLAG_BSY))

/**
 * @brief  Checks if SPI is enabled
 */
#define SPI_CHECK_ENABLED(SPIx)             if (!((SPIx)->CR1 & SPI_CR1_SPE)) {return;}

/**
 * @brief  Checks if SPI is enabled and returns value from function if not 
 */
#define SPI_CHECK_ENABLED_RESP(SPIx, val)   if (!((SPIx)->CR1 & SPI_CR1_SPE)) {return (val);}

/**
 * @}
 */

/**
 * @defgroup TM_SPI_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes SPIx peripheral with custom pinspack and default other settings
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  pinspack: Pinspack you will use from default SPI table. This parameter can be a value of @ref TM_SPI_PinsPack_t enumeration
 * @retval None
 */
void TM_SPI_Init(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack);

/**
 * @brief  Initializes SPIx peripheral with custom pinspack and SPI mode and default other settings
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  pinspack: Pinspack you will use from default SPI table. This parameter can be a value of @ref TM_SPI_PinsPack_t enumeration
 * @param  SPI_Mode: SPI mode you will use. This parameter can be a value of @ref TM_SPI_Mode_t enumeration
 * @retval None
 */
void TM_SPI_InitWithMode(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack, TM_SPI_Mode_t SPI_Mode);

/**
 * @brief  Initializes SPIx peripheral with custom settings
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  pinspack: Pinspack you will use from default SPI table. This parameter can be a value of @ref TM_SPI_PinsPack_t enumeration
 * @param  SPI_BaudRatePrescaler: SPI baudrate prescaler. This parameter can be a value of @ref SPI_BaudRatePrescaler
 * @param  SPI_Mode_t: SPI mode you will use. This parameter can be a value of @ref TM_SPI_Mode_t enumeration
 * @param  SPI_Mode: SPI mode you will use:
 *            - SPI_MODE_MASTER: SPI in master mode (default)
 *            - SPI_MODE_SLAVE: SPI in slave mode
 * @param  SPI_FirstBit: select first bit for SPI
 *            - SPI_FIRSTBIT_MSB: MSB is first bit (default)
 *            - SPI_FIRSTBIT_LSB: LSB is first bit
 * @retval None
 */
void TM_SPI_InitFull(SPI_TypeDef* SPIx, TM_SPI_PinsPack_t pinspack, uint16_t SPI_BaudRatePrescaler, TM_SPI_Mode_t SPI_Mode_t, uint16_t SPI_Mode, uint16_t SPI_FirstBit);

/**
 * @brief  Calculates bits for SPI prescaler register to get minimal prescaler value for SPI peripheral
 * @note   SPI has 8 prescalers available, 2,4,8,...,128,256
 * @note   This function will return you a bits you must set in your CR1 register.
 *
 * @note   Imagine, you can use 20MHz max clock in your system, your system is running on 168MHz, and you use SPI on APB2 bus.
 *         On 168 and 180MHz devices, APB2 works on Fclk/2, so 84 and 90MHz.
 *         So, if you calculate this, prescaler will need to be 84MHz / 20MHz = 4.xx, but if you use 4 prescaler, then you will be over 20MHz.
 *         You need 8 prescaler then. This function will calculate this.
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6.
 *            Different SPIx works on different clock and is important to know for which SPI you need prescaler.
 * @param  MAX_SPI_Frequency: Max SPI frequency you can use. Function will calculate the minimum prescaler you need for that.
 *
 * @retval Bits combination for SPI_CR1 register, with aligned location already, prepared to set parameter for @ref TM_SPI_InitFull() function.
 */
uint16_t TM_SPI_GetPrescalerFromMaxFrequency(SPI_TypeDef* SPIx, uint32_t MAX_SPI_Frequency);

/**
 * @brief  Sets data size for SPI at runtime
 * @note   You can select either 8 or 16 bits data array. 
 * @param  *SPIx: Pointer to SPIx peripheral where data size will be set
 * @param  DataSize: Datasize which will be used. This parameter can be a value of @ref TM_SPI_DataSize_t enumeration
 * @retval Status of data size before changes happen
 */
TM_SPI_DataSize_t TM_SPI_SetDataSize(SPI_TypeDef* SPIx, TM_SPI_DataSize_t DataSize);

/**
 * @brief  Sends single byte over SPI
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  data: 8-bit data size to send over SPI
 * @retval Received byte from slave device
 */
static __INLINE uint8_t TM_SPI_Send(SPI_TypeDef* SPIx, uint8_t data) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED_RESP(SPIx, 0);
	
	/* Wait for previous transmissions to complete if DMA TX enabled for SPI */
	SPI_WAIT_TX(SPIx);
	
	/* Fill output buffer with data */
	SPIx->DR = data;
	
	/* Wait for transmission to complete */
	SPI_WAIT_RX(SPIx);
	
	/* Return data from buffer */
	return SPIx->DR;
}

/**
 * @brief  Sends and receives multiple bytes over SPIx
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  *dataOut: Pointer to array with data to send over SPI
 * @param  *dataIn: Pointer to array to to save incoming data
 * @param  count: Number of bytes to send/receive over SPI
 * @retval None
 */
void TM_SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint8_t* dataIn, uint32_t count);

/**
 * @brief  Writes multiple bytes over SPI
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  *dataOut: Pointer to array with data to send over SPI
 * @param  count: Number of elements to send over SPI
 * @retval None
 */
void TM_SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint32_t count);

/**
 * @brief  Receives multiple data bytes over SPI
 * @note   Selected SPI must be set in 16-bit mode
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  *dataIn: Pointer to 8-bit array to save data into
 * @param  dummy: Dummy byte  to be sent over SPI, to receive data back. In most cases 0x00 or 0xFF
 * @param  count: Number of bytes you want read from device
 * @retval None
 */
void TM_SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t *dataIn, uint8_t dummy, uint32_t count);

/**
 * @brief  Sends single byte over SPI
 * @note   Selected SPI must be set in 16-bit mode
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  data: 16-bit data size to send over SPI
 * @retval Received 16-bit value from slave device
 */
static __INLINE uint16_t TM_SPI_Send16(SPI_TypeDef* SPIx, uint8_t data) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED_RESP(SPIx, 0);
	
	/* Wait for previous transmissions to complete if DMA TX enabled for SPI */
	SPI_WAIT_TX(SPIx);
	
	/* Fill output buffer with data */
	SPIx->DR = data;
	
	/* Wait for transmission to complete */
	SPI_WAIT_RX(SPIx);
	
	/* Return data from buffer */
	return SPIx->DR;
}

/**
 * @brief  Sends and receives multiple bytes over SPIx in 16-bit SPI mode
 * @note   Selected SPI must be set in 16-bit mode
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  *dataOut: Pointer to array with data to send over SPI
 * @param  *dataIn: Pointer to array to to save incoming data
 * @param  count: Number of 16-bit values to send/receive over SPI
 * @retval None
 */
void TM_SPI_SendMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint16_t* dataIn, uint32_t count);

/**
 * @brief  Writes multiple data via SPI in 16-bit SPI mode
 * @note   Selected SPI must be set in 16-bit mode
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  *dataOut: Pointer to 16-bit array with data to send over SPI
 * @param  count: Number of elements to send over SPI
 * @retval None
 */
void TM_SPI_WriteMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint32_t count);

/**
 * @brief  Receives multiple data bytes over SPI in 16-bit SPI mode
 * @note   Selected SPI must be set in 16-bit mode
 * @param  *SPIx: Pointer to SPIx peripheral you will use, where x is between 1 to 6
 * @param  *dataIn: Pointer to 16-bit array to save data into
 * @param  dummy: Dummy 16-bit value to be sent over SPI, to receive data back. In most cases 0x00 or 0xFF
 * @param  count: Number of 16-bit values you want read from device
 * @retval None
 */
void TM_SPI_ReadMulti16(SPI_TypeDef* SPIx, uint16_t* dataIn, uint16_t dummy, uint32_t count);

/**
 * @brief  Init custom SPI pins for your SPIx. This is callback function and will be called from my library if needed.
 * @note   When you call TM_SPI_Init() function, and if you pass TM_SPI_PinsPack_Custom to function,
 *         then this function will be called where you can initialize custom pins for SPI peripheral
 *
 * @note   You have to initialize MOSI, MISO and SCK pin
 *
 * @param  *SPIx: Pointer to SPIx peripheral for which you have to set your custom pin settings
 * @param  AlternateFunction: Alternate function number which should be used for GPIO pins
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_SPI_InitCustomPinsCallback(SPI_TypeDef* SPIx, uint16_t AlternateFunction);

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

