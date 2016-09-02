/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-16-i2c-for-stm32fxxx-devices/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   I2C library for STM32Fxxx
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
#ifndef TM_I2C_H
#define TM_I2C_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_I2C
 * @brief    I2C library for STM32Fxxx - http://stm32f4-discovery.net/2015/07/hal-library-16-i2c-for-stm32fxxx-devices/
 * @{
 *
 *	\par Pinout
 *	
\verbatim
       |PINSPACK 1   |PINSPACK 2   |PINSPACK 3   |PINSPACK 4
I2CX   |SCL   SDA    |SCL   SDA    |SCL   SDA    |SCL   SDA
       |             |             |             |
I2C1   |PB6   PB7    |PB8   PB9    |PB6   PB9    |
I2C2   |PB10  PB11   |PF1   PF0    |PH4   PH5    |
I2C3   |PA8   PC9    |PH7   PH8    |-     -      |
I2C4   |PD12  PD13   |PF1   PF0    |PF14  PF15   |PH11  PH12
\endverbatim
 *
 * I2C4 is not available on all devices. Please check if it is available for your device before using it!
 *
 * \par Custom pinout
 *
 * In case these pins are not good for you, you can use
 * @ref TM_I2C_PinsPack_Custom in @ref TM_I2C_Init() function and callback function will be called,
 * where you can initialize your custom pinout for your I2C peripheral
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
 * @defgroup TM_I2C_Macros
 * @brief    Library defines
 * @{
 */

/* Clock values */
#define TM_I2C_CLOCK_STANDARD         100000  /*!< I2C Standard speed */
#define TM_I2C_CLOCK_FAST_MODE        400000  /*!< I2C Fast mode speed */
#define TM_I2C_CLOCK_FAST_MODE_PLUS   1000000 /*!< I2C Fast mode plus speed */
#define TM_I2C_CLOCK_HIGH_SPEED       3400000 /*!< I2C High speed */

 /**
 * @}
 */
 
/**
 * @defgroup TM_I2C_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  I2C pinspack enumeration
 */
typedef enum {
	TM_I2C_PinsPack_1 = 0x00, /*!< Use Pinspack1 from Pinout table for I2Cx */
	TM_I2C_PinsPack_2,        /*!< Use Pinspack2 from Pinout table for I2Cx */
	TM_I2C_PinsPack_3,        /*!< Use Pinspack3 from Pinout table for I2Cx */
	TM_I2C_PinsPack_4,        /*!< Use Pinspack4 from Pinout table for I2Cx */
	TM_I2C_PinsPack_Custom    /*!< Use custom pins for I2Cx */
} TM_I2C_PinsPack_t;

/**
 * @brief  I2C result enumeration
 */
typedef enum {
	TM_I2C_Result_Ok = 0x00, /*!< Everything OK */
	TM_I2C_Result_Error      /*!< An error has occurred */
} TM_I2C_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_I2C_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes I2C peripheral
 * @param  *I2Cx: Pointer to I2Cx peripheral you will use for iintialization
 * @param  pinspack: Pinspack used for GPIO initialization. This parameter can be a value of @ref TM_I2C_PinsPack_t enumeration
 * @param  clockSpeed: Clock speed in units of Hz for I2C communication
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_Init(I2C_TypeDef* I2Cx, TM_I2C_PinsPack_t pinspack, uint32_t clockSpeed);

/**
 * @brief  Reads single byte from device
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  register_address: Register address from where read will be done
 * @param  *data: Pointer to variable where data will be stored from read operation
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_Read(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t register_address, uint8_t* data);

/**
 * @brief  Reads multiple bytes from device
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  register_address: Register address from where read operation will start
 * @param  *data: Pointer to variable where data will be stored from read operation
 * @param  count: Number of elements to read from device
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t register_address, uint8_t* data, uint16_t count);

/**
 * @brief  Reads I2C data without specifying register address
 * @note   This can be used if your sensors just sends data, without any registers
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  *data: Pointer to variable where data will be stored from read operation
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_ReadNoRegister(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t* data);

/**
 * @brief  Reads multiple bytes from device without specifying register address
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  *data: Pointer to variable where data will be stored from read operation
 * @param  count: Number of elements to read from device
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_ReadMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t* data, uint16_t count);

/**
 * @brief  Reads single byte from device with 16-bit register address
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  register_address: Register address from where read will be done
 * @param  *data: Pointer to variable where data will be stored from read operation
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_Read16(I2C_TypeDef* I2Cx, uint8_t device_address, uint16_t register_address, uint8_t* data);

/**
 * @brief  Writes single byte to device
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  register_address: Register address where you want to write data
 * @param  data: Data to be written to device
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_Write(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t register_address, uint8_t data);

/**
 * @brief  Writes multiple data to device
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  register_address: Register address where data will be written
 * @param  *data: Data to be written to device.
 * @param  count: Number of elements to write starting at register register_address
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t device_address, uint16_t register_address, uint8_t *data, uint16_t count);

/**
 * @brief  Writes single byte to device without specifying register address, can be used for command write
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  data: Data to be written to device
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_WriteNoRegister(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t data);

/**
 * @brief  Writes multiple data to device without register address
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  *data: Pointer to data array t obe written to device. Array length is the same as number of elements you want to write
 * @param  count: Number of elements to write
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_WriteMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t device_address, uint8_t* data, uint16_t count);

/**
 * @brief  Writes single byte in a 16-bit length register address
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  register_address: Register address where data will be written
 * @param  data: Data byte to write
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_Write16(I2C_TypeDef* I2Cx, uint8_t device_address, uint16_t register_address, uint8_t data);


/**
 * @brief  Checks if device is connected to I2C port and ready to use
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address);

/**
 * @brief  Callback for custom pins initialization.
 * 
 *         When you call TM_I2C_Init() function, and if you pass TM_I2C_PinsPack_Custom to function,
 *         then this function will be called where you can initialize custom pins for I2C peripheral.
 * @param  *I2Cx: I2C for which initialization will be set
 * @param  AlternateFunction: Alternate function which should be used for GPIO initialization
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_I2C_InitCustomPinsCallback(I2C_TypeDef* I2Cx, uint16_t AlternateFunction);

/**
 * @brief  Gets pointer to I2C handle structure for specific I2C
 * @param  *I2Cx: Pointer to I2Cx used for handle
 * @retval Pointer to I2C Handle structure
 */
I2C_HandleTypeDef* TM_I2C_GetHandle(I2C_TypeDef* I2Cx);

/**
 * @brief  Writes and receives amount of data via I2C using repeated start condition
 * @param  *I2Cx: Pointer to I2Cx peripheral to be used in communication
 * @param  device_address: 7-bit, left aligned device address used for communication
 * @param  write_register_address: Register address to start writing to
 * @param  *write_data: Pointer to data array where data for write are stored
 * @param  write_count: Number of elements to write
 * @param  read_register_address: Register address where reading will start
 * @param  *read_data: Pointer to array where data will be saved
 * @param  read_count: Number of elements to read
 * @retval Member of @ref TM_I2C_Result_t enumeration
 */
TM_I2C_Result_t TM_I2C_WriteReadRepeatedStart(
	I2C_TypeDef* I2Cx,
	uint8_t device_address, 
	uint8_t write_register_address, 
	uint8_t* write_data,
	uint16_t write_count, 
	uint8_t read_register_address, 
	uint8_t* read_data,
	uint16_t read_count
);

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

