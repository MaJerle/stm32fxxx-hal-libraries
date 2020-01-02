/**
 * @author  Tilen MAJERLE
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Library template 
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen MAJERLE

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
#ifndef TM_LIBRARY_H
#define TM_LIBRARY_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_LIBNAME
 * @brief    Library description here
 * @{
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
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_i2c.h"
#include "tm_stm32_delay.h"

/**
 * @defgroup TM_LIB_Macros
 * @brief    Library defines
 * @{
 */

#ifndef MPU9250_I2C
#define MPU9250_I2C             I2C1
#define MPU9250_I2C_PP          TM_I2C_PinsPack_2
#endif

#define MPU9250_I2C_CLOCK       400000

#define MPU9250_I2C_ADDR        0xD0  
#define MPU9250_I2C_ADDR_MAG    (0x0C << 1)

/**
 * @}
 */
 
/**
 * @defgroup TM_LIB_Typedefs
 * @brief    Library Typedefs
 * @{
 */

typedef enum _TM_MPU9250_Result_t {
    TM_MPU9250_Result_Ok = 0x00,
    TM_MPU9250_Result_Error,
    TM_MPU9250_Result_DeviceNotConnected
} TM_MPU9250_Result_t;

typedef enum _TM_MPU9250_Device_t {
    TM_MPU9250_Device_0 = 0x00,
    TM_MPU9250_Device_1 = 0x02
} TM_MPU9250_Device_t;

typedef enum _TM_MPU9250_AcceSens_t {
    TM_MPU9250_AcceSens_2G = 0x00,
    TM_MPU9250_AcceSens_4G = 0x01,
    TM_MPU9250_AcceSens_8G = 0x02,
    TM_MPU9250_AcceSens_16G = 0x03
} TM_MPU9250_AcceSens_t;

typedef enum _TM_MPU9250_GyroSens_t {
    TM_MPU9250_GyroSens_250DPS = 0x00,
    TM_MPU9250_GyroSens_500DPS = 0x01,
    TM_MPU9250_GyroSens_1000DPS = 0x02,
    TM_MPU9250_GyroSens_2000DPS = 0x03
} TM_MPU9250_GyroSens_t;

typedef enum _TM_MPU9250_MagSens_t {
    TM_MPU9250_MagSens_14Bit = 0x00,    // 0.6 mG per LSB
    TM_MPU9250_MagSens_16Bit            // 0.15 mG per LSB
} TM_MPU9250_MagSens_t;

typedef struct _TM_MPU9250_t {
    float Ax, Ay, Az;         /*!< Accelerometer raw data */
    float Gx, Gy, Gz;         /*!< Gyroscope raw data */
    float Mx, My, Mz;         /*!< Magnetometer raw data */
    int16_t Ax_Raw, Ay_Raw, Az_Raw;         /*!< Accelerometer raw data */
    int16_t Gx_Raw, Gy_Raw, Gz_Raw;         /*!< Gyroscope raw data */
    int16_t Mx_Raw, My_Raw, Mz_Raw;         /*!< Magnetometer raw data */
    
    float AMult, GMult, MMult;
    
    uint8_t I2C_Addr;
    uint8_t I2C_Addr_Mag;
} TM_MPU9250_t;

/**
 * @}
 */

/**
 * @defgroup TM_LIB_Functions
 * @brief    Library Functions
 * @{
 */

TM_MPU9250_Result_t TM_MPU9250_Init(TM_MPU9250_t* MPU9250, TM_MPU9250_Device_t dev);

TM_MPU9250_Result_t TM_MPU9250_ReadAcce(TM_MPU9250_t* MPU9250);
TM_MPU9250_Result_t TM_MPU9250_ReadGyro(TM_MPU9250_t* MPU9250);
TM_MPU9250_Result_t TM_MPU9250_ReadMag(TM_MPU9250_t* MPU9250);
TM_MPU9250_Result_t TM_MPU9250_DataReady(TM_MPU9250_t* MPU9250);

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
