/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/10/hal-library-30-mpu6050-for-stm32fxxx
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   MPU6050 library for STM32Fxxx devices
 *	
@verbatim
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
@endverbatim
 */
#ifndef TM_MPU6050_H
#define TM_MPU6050_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_MPU6050
 * @brief    MPU6050 library for STM32Fxxx - http://stm32f4-discovery.net/2015/10/hal-library-30-mpu6050-for-stm32fxxx
 * @{
 *
 * \par Features
 *
 * Library supports basic operation with MPU6050 device:
 *
\verbatim
- Read accelerometer, gyroscope and temperature data,
- Set custom output data rate for measurements
- Enable/disable interrupts
- Up to 2 MPU devices at a time
\endverbatim
 *
 * \par MPU6050 interrupts
 *
 * When you enable interrupts using @ref TM_MPU6050_EnableInterrupts function, 
 * "DataReady" and "MotionDetected" interrupts are enabled. 
 *
 * MPU pin for interrupt detection on STM device is rising edge and triggers on any interrupt.
 *
 * You can read interrupts status register to detect which interrupt happened using @ref TM_MPU6050_ReadInterrupts function.
 *
 * \par MPU6050 data rate
 *
 * Device can output data at specific rate. It has 8-bit register with custom value to set data rate you need.
 *
 * Equation for data rate is below:
\f[
	DataRate = 
		\frac{8 MHz}{REGVAL + 1}
\f]
 * where:
 *  - 8 Mhz is Gyro internal output used for data rate
 *  - REGVAL is a value to be used in @ref TM_MPU6050_SetDataRate function
 *
 * \note  There are already some predefined constants in library for some "standard" data rates
 *
 * \par Default pinout
 * 
@verbatim
MPU6050     STM32Fxxx     Descrption
 
SCL         PB6           Clock line for I2C
SDA         PB7           Data line for I2C
IRQ         -             User selectable pin if needed. Interrupts for STM must be manually enabled by user.
VCC         3.3V
GND         GND
AD0         -             If pin is low, I2C address is 0xD0, if pin is high, the address is 0xD2
@endverbatim
 *
 * To change default pinout for I2C, you need to open defines.h file and copy/edit some defines:
 *
\code
//Set I2C used
MPU6050_I2C               I2C1  
//Set I2C pins used
MPU6050_I2C_PINSPACK      TM_I2C_PinsPack_1
\endcode
 *
 * \par Changelog
 *
@verbatim
 Version 1.0
  - First release
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM I2C
@endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_i2c.h"

/**
 * @defgroup TM_MPU6050_Macros
 * @brief    Library defines
 * @{
 */

/* Default I2C used */
#ifndef MPU6050_I2C
#define	MPU6050_I2C                    I2C1              /*!< Default I2C */
#define MPU6050_I2C_PINSPACK           TM_I2C_PinsPack_1 /*!< Default I2C pinspack. Check @ref TM_I2C for more information */
#endif

/* Default I2C clock */
#ifndef MPU6050_I2C_CLOCK
#define MPU6050_I2C_CLOCK              400000            /*!< Default I2C clock speed */
#endif

/**
 * @brief  Data rates predefined constants
 * @{
 */
#define TM_MPU6050_DataRate_8KHz       0   /*!< Sample rate set to 8 kHz */
#define TM_MPU6050_DataRate_4KHz       1   /*!< Sample rate set to 4 kHz */
#define TM_MPU6050_DataRate_2KHz       3   /*!< Sample rate set to 2 kHz */
#define TM_MPU6050_DataRate_1KHz       7   /*!< Sample rate set to 1 kHz */
#define TM_MPU6050_DataRate_500Hz      15  /*!< Sample rate set to 500 Hz */
#define TM_MPU6050_DataRate_250Hz      31  /*!< Sample rate set to 250 Hz */
#define TM_MPU6050_DataRate_125Hz      63  /*!< Sample rate set to 125 Hz */
#define TM_MPU6050_DataRate_100Hz      79  /*!< Sample rate set to 100 Hz */
/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @defgroup TM_MPU6050_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  MPU6050 can have 2 different slave addresses, depends on it's input AD0 pin
 *         This feature allows you to use 2 different sensors with this library at the same time
 */
typedef enum _TM_MPU6050_Device_t {
	TM_MPU6050_Device_0 = 0x00, /*!< AD0 pin is set to low */
	TM_MPU6050_Device_1 = 0x02  /*!< AD0 pin is set to high */
} TM_MPU6050_Device_t;

/**
 * @brief  MPU6050 result enumeration	
 */
typedef enum _TM_MPU6050_Result_t {
	TM_MPU6050_Result_Ok = 0x00,          /*!< Everything OK */
	TM_MPU6050_Result_Error,              /*!< Unknown error */
	TM_MPU6050_Result_DeviceNotConnected, /*!< There is no device with valid slave address */
	TM_MPU6050_Result_DeviceInvalid       /*!< Connected device with address is not MPU6050 */
} TM_MPU6050_Result_t;

/**
 * @brief  Parameters for accelerometer range
 */
typedef enum _TM_MPU6050_Accelerometer_t {
	TM_MPU6050_Accelerometer_2G = 0x00, /*!< Range is +- 2G */
	TM_MPU6050_Accelerometer_4G = 0x01, /*!< Range is +- 4G */
	TM_MPU6050_Accelerometer_8G = 0x02, /*!< Range is +- 8G */
	TM_MPU6050_Accelerometer_16G = 0x03 /*!< Range is +- 16G */
} TM_MPU6050_Accelerometer_t;

/**
 * @brief  Parameters for gyroscope range
 */
typedef enum _TM_MPU6050_Gyroscope_t {
	TM_MPU6050_Gyroscope_250s = 0x00,  /*!< Range is +- 250 degrees/s */
	TM_MPU6050_Gyroscope_500s = 0x01,  /*!< Range is +- 500 degrees/s */
	TM_MPU6050_Gyroscope_1000s = 0x02, /*!< Range is +- 1000 degrees/s */
	TM_MPU6050_Gyroscope_2000s = 0x03  /*!< Range is +- 2000 degrees/s */
} TM_MPU6050_Gyroscope_t;

/**
 * @brief  Main MPU6050 structure
 */
typedef struct _TM_MPU6050_t {
	/* Private */
	uint8_t Address;         /*!< I2C address of device. Only for private use */
	float Gyro_Mult;         /*!< Gyroscope corrector from raw data to "degrees/s". Only for private use */
	float Acce_Mult;         /*!< Accelerometer corrector from raw data to "g". Only for private use */
	/* Public */
	int16_t Accelerometer_X; /*!< Accelerometer value X axis */
	int16_t Accelerometer_Y; /*!< Accelerometer value Y axis */
	int16_t Accelerometer_Z; /*!< Accelerometer value Z axis */
	int16_t Gyroscope_X;     /*!< Gyroscope value X axis */
	int16_t Gyroscope_Y;     /*!< Gyroscope value Y axis */
	int16_t Gyroscope_Z;     /*!< Gyroscope value Z axis */
	float Temperature;       /*!< Temperature in degrees */
} TM_MPU6050_t;

/**
 * @brief  Interrupts union and structure
 */
typedef union _TM_MPU6050_Interrupt_t {
	struct {
		uint8_t DataReady:1;       /*!< Data ready interrupt */
		uint8_t reserved2:2;       /*!< Reserved bits */
		uint8_t Master:1;          /*!< Master interrupt. Not enabled with library */
		uint8_t FifoOverflow:1;    /*!< FIFO overflow interrupt. Not enabled with library */
		uint8_t reserved1:1;       /*!< Reserved bit */
		uint8_t MotionDetection:1; /*!< Motion detected interrupt */
		uint8_t reserved0:1;       /*!< Reserved bit */
	} F;
	uint8_t Status;
} TM_MPU6050_Interrupt_t;

/**
 * @}
 */

/**
 * @defgroup TM_MPU6050_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes MPU6050 and I2C peripheral
 * @param  *DataStruct: Pointer to empty @ref TM_MPU6050_t structure
 * @param  DeviceNumber: MPU6050 has one pin, AD0 which can be used to set address of device.
 *          This feature allows you to use 2 different sensors on the same board with same library.
 *          If you set AD0 pin to low, then this parameter should be TM_MPU6050_Device_0,
 *          but if AD0 pin is high, then you should use TM_MPU6050_Device_1
 *          
 *          Parameter can be a value of @ref TM_MPU6050_Device_t enumeration
 * @param  AccelerometerSensitivity: Set accelerometer sensitivity. This parameter can be a value of @ref TM_MPU6050_Accelerometer_t enumeration
 * @param  GyroscopeSensitivity: Set gyroscope sensitivity. This parameter can be a value of @ref TM_MPU6050_Gyroscope_t enumeration
 * @retval Initialization status:
 *            - TM_MPU6050_Result_t: Everything OK
 *            - Other member: in other cases
 */
TM_MPU6050_Result_t TM_MPU6050_Init(TM_MPU6050_t* DataStruct, TM_MPU6050_Device_t DeviceNumber, TM_MPU6050_Accelerometer_t AccelerometerSensitivity, TM_MPU6050_Gyroscope_t GyroscopeSensitivity);

/**
 * @brief  Sets gyroscope sensitivity
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure indicating MPU6050 device
 * @param  GyroscopeSensitivity: Gyro sensitivity value. This parameter can be a value of @ref TM_MPU6050_Gyroscope_t enumeration
 * @retval Member of @ref TM_MPU6050_Result_t enumeration
 */
TM_MPU6050_Result_t TM_MPU6050_SetGyroscope(TM_MPU6050_t* DataStruct, TM_MPU6050_Gyroscope_t GyroscopeSensitivity);

/**
 * @brief  Sets accelerometer sensitivity
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure indicating MPU6050 device
 * @param  AccelerometerSensitivity: Gyro sensitivity value. This parameter can be a value of @ref TM_MPU6050_Accelerometer_t enumeration
 * @retval Member of @ref TM_MPU6050_Result_t enumeration
 */
TM_MPU6050_Result_t TM_MPU6050_SetAccelerometer(TM_MPU6050_t* DataStruct, TM_MPU6050_Accelerometer_t AccelerometerSensitivity);

/**
 * @brief  Sets output data rate
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure indicating MPU6050 device
 * @param  rate: Data rate value. An 8-bit value for prescaler value
 * @retval Member of @ref TM_MPU6050_Result_t enumeration
 */
TM_MPU6050_Result_t TM_MPU6050_SetDataRate(TM_MPU6050_t* DataStruct, uint8_t rate);

/**
 * @brief  Enables interrupts
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure indicating MPU6050 device
 * @retval Member of @ref TM_MPU6050_Result_t enumeration
 */
TM_MPU6050_Result_t TM_MPU6050_EnableInterrupts(TM_MPU6050_t* DataStruct);

/**
 * @brief  Disables interrupts
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure indicating MPU6050 device
 * @retval Member of @ref TM_MPU6050_Result_t enumeration
 */
TM_MPU6050_Result_t TM_MPU6050_DisableInterrupts(TM_MPU6050_t* DataStruct);

/**
 * @brief  Reads and clears interrupts
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure indicating MPU6050 device
 * @param  *InterruptsStruct: Pointer to @ref TM_MPU6050_Interrupt_t structure to store status in
 * @retval Member of @ref TM_MPU6050_Result_t enumeration
 */
TM_MPU6050_Result_t TM_MPU6050_ReadInterrupts(TM_MPU6050_t* DataStruct, TM_MPU6050_Interrupt_t* InterruptsStruct);

/**
 * @brief  Reads accelerometer data from sensor
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure to store data to
 * @retval Member of @ref TM_MPU6050_Result_t:
 *            - TM_MPU6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
TM_MPU6050_Result_t TM_MPU6050_ReadAccelerometer(TM_MPU6050_t* DataStruct);

/**
 * @brief  Reads gyroscope data from sensor
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure to store data to
 * @retval Member of @ref TM_MPU6050_Result_t:
 *            - TM_MPU6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
TM_MPU6050_Result_t TM_MPU6050_ReadGyroscope(TM_MPU6050_t* DataStruct);

/**
 * @brief  Reads temperature data from sensor
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure to store data to
 * @retval Member of @ref TM_MPU6050_Result_t:
 *            - TM_MPU6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
TM_MPU6050_Result_t TM_MPU6050_ReadTemperature(TM_MPU6050_t* DataStruct);

/**
 * @brief  Reads accelerometer, gyroscope and temperature data from sensor
 * @param  *DataStruct: Pointer to @ref TM_MPU6050_t structure to store data to
 * @retval Member of @ref TM_MPU6050_Result_t:
 *            - TM_MPU6050_Result_Ok: everything is OK
 *            - Other: in other cases
 */
TM_MPU6050_Result_t TM_MPU6050_ReadAll(TM_MPU6050_t* DataStruct);

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
