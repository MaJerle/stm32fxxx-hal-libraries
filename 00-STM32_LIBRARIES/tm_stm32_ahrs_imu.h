/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.net/2016/07/hal-library-36-ahrs-imu-algorithms-calculating-roll-pitch-yaw-axes/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   AHRS and IMU algorithm library by Sebastian Madgwick and ported as library
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
#ifndef TM_AHRSIMU_H
#define TM_AHRSIMU_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_AHRSIMU
 * @brief    AHRS and IMU algorithm library by Sebastian Madgwick and ported - http://stm32f4-discovery.net/2016/07/hal-library-36-ahrs-imu-algorithms-calculating-roll-pitch-yaw-axes/
 * @{
 *
 * This library calculates Roll, Pitch and Yaw angles by using AHRS or IMU algorithms developed by Sebastian Madgwick.
 * More info about algorithm is available on official website: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
 *
 * \par How it works
 *
 * By providing sample rate, beta and inclination values on initialization process, you have to later call one of update functions,
 * which depends on how many parameters and sensors you have available in your system. For more info, check update functions.
 *
 * Those update functions (one of them) must be called in the sample rate intervals you set on initialization to get proper values and to achieve filtering.
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 */

#include "stdint.h"
#include "math.h"
    
/**
 * @defgroup TM_AHRSIMU_Macros
 * @brief    Library defines
 * @{
 */

#define AHRSIMU_PI              3.141592653f            /* PI definition */
#define AHRSIMU_RAD2DEG(x)      ((x) * 57.2957795f)     /* Radians to degrees converter */
#define AHRSIMU_DEG2RAD(x)      ((x) * 0.0174532925f)   /* Radians to degrees converter */

/**
 * @}
 */
 
/**
 * @defgroup TM_AHRSIMU_Typedefs
 * @brief    Library Typedefs
 * @{
 */
 
/**
 * @brief   Main working AHRS IMU structure
 */
typedef struct _TM_AHRSIMU_t {
    float Roll;             /* Roll angle value. This parameter is in units of degrees */
    float Pitch;            /* Pitch angle value. This parameter is in units of degrees */
    float Yaw;              /* Yaw angle value. This parameter is in units of degrees */
    float Inclination;      /* Inclination in units of degrees */
    
    float _beta;
    float _q0, _q1, _q2, _q3;
    float _sampleRate;
} TM_AHRSIMU_t;

/**
 * @}
 */

/**
 * @defgroup TM_AHRSIMU_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * \brief  Initializes ARHS or IMU algorithm for motion calculations
 * \param  *AHRSIMU: Pointer to \ref TM_AHRSIMU_t empty structure
 * \param  sampleRate: Sample rate frequency for updating data
 * \param  beta: Gain for calculations and speed to stabilize. A value less than 0.2 is a good value but it mostly depends on applicaiton.
 * \param  inclination: Magnetic inclination for position on earth in units of degrees. This value depends on GPS coordinates on earth.
 * \retval None
 */
void TM_AHRSIMU_Init(TM_AHRSIMU_t* AHRSIMU, float sampleRate, float beta, float inclination);

/**
 * \brief  Updates AHRS algorithm with new values and calculates roll, pitch and yaw angles.
 * \param  *AHRSIMU: Pointer to \ref TM_AHRSIMU_t empty structure
 * \param  gx, gy, gz: Gyro data for X, Y and Z axis respectively. These parameters must be in units of rad/s. Use \ref AHRSIMU_DEG2RAD if your gyro outputs data in deg/s.
 * \param  ax, ay, az: Accelerometer data for X, Y and Z axis respectively. These parameters must be in units of gees.
 * \param  mx, my, mz: Magnetic data for X, Y and Z axis respectively. These parameters must be in units of uT (micro Tesla).
 *            When all parameters are zero (magnetic sensor not used), \ref TM_AHRSIMU_UpdateIMU is automatically called.
 * \retval None
 */
void TM_AHRSIMU_UpdateAHRS(TM_AHRSIMU_t* AHRSIMU, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

/**
 * \brief  Updates IMU algorithm with new values and calculates roll, pitch and yaw angles.
 * \param  *AHRSIMU: Pointer to \ref TM_AHRSIMU_t empty structure
 * \param  gx, gy, gz: Gyro data for X, Y and Z axis respectively. These parameters must be in units of rad/s. Use \ref AHRSIMU_DEG2RAD if your gyro outputs data in deg/s.
 * \param  ax, ay, az: Accelerometer data for X, Y and Z axis respectively. These parameters must be in units of gees.
 * \retval None
 */
void TM_AHRSIMU_UpdateIMU(TM_AHRSIMU_t* AHRSIMU, float gx, float gy, float gz, float ax, float ay, float az);

/**
 * \brief  Sets new gain value for processing
 * \param  *AHRSIMU: Pointer to \ref TM_AHRSIMU_t empty structure
 * \param  beta: New beta value
 * \retval None
 */
#define TM_AHRSIMU_SetBeta(AHRSIMU, beta)       ((AHRSIMU)->_beta = (beta))

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
