/**
 * @author  Tilen MAJERLE
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-14-fast-fourier-transform-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   FFT library for float 32 and Cortex-M4/7 little endian MCUs
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
#ifndef TM_FFT_H
#define TM_FFT_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_FFT
 * @brief    FFT library for STM32Fxxx devices - http://stm32f4-discovery.net/2015/07/hal-library-14-fast-fourier-transform-for-stm32fxxx/
 * @{
 *
 * This library allows you to calculate FFT in your signal.
 * 
 * For more info about FFT and how it works on Cortex-M4, you should take a look at ARM DSP documentation.
 *
 * \par ARM dependencies
 *
 * Some libraries are needed in order to start with ARM CMSIS MATH Library.
 *
\verbatim
- arm_const_structs.c, available in “CMSIS\DSP_Lib\Source\CommonTables”
- arm_cortexM4lf_math.lib for ARM compiler, available in “CMSIS\Lib\ARM”
- CMSIS folder can be found if you download these libraries directly from ARM.com or if you download Standard Perihperal Drivers for STM32F4xx from ST.com site.
\endverbatim
 *
 * \par ARM MATH
 *
 * ARM MATH is a library provided from ARM and is the same for all Cortex families, except that you have to provide some informations to library.
 * 
 * In your global compiler defines, you should add these 2 lines:
\code
//Use ARM MATH for Cortex-M4
#define ARM_MATH_CM4
//Use ARM MATH for Cortex-M7
#define ARM_MATH_CM7
//Use ARM MATH for Cortex-M0
#define ARM_MATH_CM0
//Use ARM MATH for Cortex-M0+
#define ARM_MATH_CM0PLUS

//Set define if FPU presents on device. On F4xx and F7xx it is available
#define __FPU_PRESENT   1
\endcode
 * \par FFT Samples count
 *
 * ARM FFT library allows you to use specific number of samples for data calculation.
 *
 * These values can be every number which is <b>power of 2</b> from <b>2^4 and 2^12</b>. So, 9 different FFT length options. This number is passed into function when you initialize FFT with @ref TM_FFT_Init_F32.
 *
 * \par FFT input/output buffers
 *
 * FFT works in a way that you first fill input buffer with samples and then you process them and you got samples in output buffer.
 * Complex (CFFT) Fast Fourier Transform, which is also used behind the scenes in my library so uses real and imaginary part in input buffer and only real part is calculated to output buffer.
 *
 * For this reason, input buffer <b>HAVE TO</b> be <b>2 * FFT_Size</b> in length and output buffer HAVE TO be <b>FFT_Size</b> in length where FFT_Size is the same as FFT_Samples count explained above.
 *
 * Library is able to use @ref malloc() to allocate memory for you.
 * You just need to enable this feature when you initialize FFT module and everything will be done for you.
 *
 * \note  You have to make sure that you have enough HEAP memory available for malloc, otherwise malloc will fail and you will not get anything.
 *
 * For example, if you have <b>512</b> length FFT size, then input buffer must be <b>2 * 512 = 1024</b> samples of float 32 and output buffer is 512 samples of float 32.
 * In common, this is <b>1536</b> samples of float32 which is 4-bytes long in memory.
 * Together this would be 6144 Bytes of HEAP memory.
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
 - ARM MATH
 - ARM CONST STRUCTS
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"

#include "arm_math.h"
#include "arm_const_structs.h"

/**
 * @defgroup TM_FFT_Macros
 * @brief    Library defines
 * @{
 */
 
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
 * @defgroup TM_FFT_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  FFT main structure for 32-bit float
 */
typedef struct {
	float32_t* Input;               /*!< Pointer to data input buffer. Its length must be 2 * FFT_Size */
	float32_t* Output;              /*!< Pointer to data output buffer. Its length must be FFT_Size */
	uint16_t FFT_Size;              /*!< FFT size in units of samples. This parameter can be a value of 2^n where n is between 4 and 12 */
	uint8_t UseMalloc;              /*!< Set to 1 when malloc is used for memory allocation for buffers. Meant for private use */
	uint16_t Count;                 /*!< Number of samples in buffer when using @ref TM_FFT_AddToBuffer function. Meant for private use */
	const arm_cfft_instance_f32* S; /*!< Pointer to @ref arm_cfft_instance_f32 structure. Meant for private use */
	float32_t MaxValue;             /*!< Max value in FTT result after calculation */
	uint32_t MaxIndex;              /*!< Index in output array where max value happened */
} TM_FFT_F32_t;

/**
 * @}
 */

/**
 * @defgroup TM_FFT_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes and prepares FFT structure for signal operations
 * @param  *FFT: Pointer to empty @ref TM_FFT_F32_t structure for FFT
 * @param  FFT_Size: Number of samples to be used for FFT calculation
 *            This parameter can be a value of 2^n where n is between 4 and 12, so any power of 2 between 16 and 4096
 * @param  use_malloc: Set parameter to 1, if you want to use HEAP memory and @ref malloc to allocate input and output buffers
 * @note   It is recommended to use malloc for allocation, because FFT input and output buffers differs in length
 * @retval Initialization status:
 *            - 0: Initialized OK, ready to use
 *            - 1: Input FFT SIZE is not valid
 *            - 2: Malloc failed with allocating input data buffer
 *            - 3: Malloc failed with allocating output data buffer. If input data buffer is allocated, it will be free if this is returned.
 */
uint8_t TM_FFT_Init_F32(TM_FFT_F32_t* FFT, uint16_t FFT_Size, uint8_t use_malloc);

/**
 * @brief  Sets input and output buffers for FFT calculations
 * @note   Use this function only if you set @arg use_malloc parameter to zero in @ref TM_FFT_Init_F32 function
 * @param  *FFT: Pointer to @ref TM_FFT_F32_t structure where buffers will be set
 * @param  *InputBuffer: Pointer to buffer of type float32_t with FFT_Size * 2 length
 * @param  *OutputBuffer: Pointer to buffer of type float32_t with FFT_Size length
 * @retval None
 */
void TM_FFT_SetBuffers_F32(TM_FFT_F32_t* FFT, float32_t* InputBuffer, float32_t* OutputBuffer);

/**
 * @brief  Adds new sample to input buffer in FFT array 
 * @param  *FFT: Pointer to @ref TM_FFT_F32_t structure where new sample will be added
 * @param  sampleValue: A new sample to be added to buffer, real part. Imaginary part will be set to 0
 * @retval FFT calculation status:
 *            - 0: Input buffer is not full yet
 *            - > 0: Input buffer is full and samples are ready to be calculated
 */
uint8_t TM_FFT_AddToBuffer(TM_FFT_F32_t* FFT, float32_t sampleValue);

/**
 * @brief  Adds new sample (real and imaginary part if needed) to input buffer in FFT array 
 * @param  *FFT: Pointer to @ref TM_FFT_F32_t structure where new sample will be added
 * @param  Real: A new sample to be added to buffer, real part.
 * @param  Imag: A new sample to be added to buffer, imaginary part.
 * @retval FFT calculation status:
 *            - 0: Input buffer is not full yet
 *            - > 0: Input buffer is full and samples are ready to be calculated
 */
uint8_t TM_FFT_AddToBufferWithImag(TM_FFT_F32_t* FFT, float32_t Real, float32_t Imag);

/**
 * @brief  Processes and calculates FFT from InputBuffer and saves data to Output buffer
 * @note   This function also calculates max value and max index in array where max value happens
 * @param  *FFT: Pointer to @ref TM_FFT_F32_t where FFT calculation will happen
 * @retval None
 */
void TM_FFT_Process_F32(TM_FFT_F32_t* FFT);

/**
 * @brief  Free input and output buffers
 * @note   This function has sense only, if you used @ref malloc for memory allocation when you called @ref TM_FFT_Init_F32 function
 * @param  *FFT: Pointer to @ref TM_FFT_F32_t structure where buffers will be free
 * @retval None
 */
void TM_FFT_Free_F32(TM_FFT_F32_t* FFT);

/**
 * @brief  Gets max value from already calculated FFT result
 * @param  FFT: Pointer to @ref TM_FFT_F32_t structure where max value should be checked
 * @retval None
 * @note   Defined as macro for faster execution
 */
#define TM_FFT_GetMaxValue(FFT)            ((FFT)->MaxValue)

/**
 * @brief  Gets index value where max value happens from already calculated FFT result
 * @param  FFT: Pointer to @ref TM_FFT_F32_t structure where max index at max value should be checked
 * @retval None
 * @note   Defined as macro for faster execution
 */
#define TM_FFT_GetMaxIndex(FFT)            ((FFT)->MaxIndex)

/**
 * @brief  Gets FFT size in units of samples length
 * @param  FFT: Pointer to @ref TM_FFT_F32_t structure where FFT size will be checked
 * @retval FFT size in units of elements for calculation
 * @note   Defined as macro for faster execution
 */
#define TM_FFT_GetFFTSize(FFT)             ((FFT)->FFT_Size)

/**
 * @brief  Gets FFT result value from output buffer at given index
 * @param  FFT: Pointer to @ref TM_FFT_F32_t structure where FFT output sample will be returned
 * @param  index: Index in buffer where result will be returned. Valid input is between 0 and FFT_Size - 1
 * @retval Value at given index
 * @note   Defined as macro for faster execution
 */
#define TM_FFT_GetFromBuffer(FFT, index)   ((FFT)->Output[(uint16_t)(index)])

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
