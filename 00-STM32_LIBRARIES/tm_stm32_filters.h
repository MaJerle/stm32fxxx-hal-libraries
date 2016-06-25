/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Library template 
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
#ifndef TM_FILTERS_H
#define TM_FILTERS_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_FILTERS
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
#include "arm_math.h"
#include "stdlib.h"
#include "defines.h"

/**
 * @defgroup TM_LIB_Macros
 * @brief    Library defines
 * @{
 */

/* Library allocations */
#ifndef LIB_ALLOC_FUNC
#define LIB_ALLOC_FUNC       malloc
#endif
#ifndef LIB_FREE_FUNC
#define LIB_FREE_FUNC        free
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_LIB_Typedefs
 * @brief    Library Typedefs
 * @{
 */

 
/**
 * @brief  FIR Filter structure
 */
typedef struct _TM_FILTER_FIR_F32_t {
	arm_fir_instance_f32* f;         /*!< Pointer to arm_fir_instance structure */
	size_t BlockSize;                /*!< Block size of data to process at one filter call */
	size_t StatesCount;              /*!< Number of states used for temporary data */
	union {
		struct {
			uint8_t MallocStates:1;  /*!< Set to 1 if malloc was used for state data allocation */
		} F;
		uint8_t FlagsValues;
	} Flags;
} TM_FILTER_FIR_F32_t;

/**
 * @brief  IIR Filter structure
 */
typedef struct _TM_FILTER_IIR_F32_t {
	arm_biquad_casd_df1_inst_f32* f; /*!< Pointer to arm_iir_lattice_f32 structure */
	size_t BlockSize;                /*!< Block size of data to process at one filter call */
	union {
		struct {
			uint8_t MallocStates:1;  /*!< Set to 1 if malloc was used for state data allocation */
		} F;
		uint8_t FlagsValues;
	} Flags;
} TM_FILTER_IIR_F32_t;

/**
 * @}
 */

/**
 * @defgroup TM_LIB_Functions
 * @brief    Library Functions
 * @{
 */
/**
 * @brief  Creates and initializes ARM FIR filter using F32 coefficients
 * @note   Malloc is used to initialize proper size of heap memory
 * @param  coeff_size: Number of cofficients for FIR filtering.
 *           When possible, use coeff_size multiply of 8 elements, because filtering process is designed for this coeff size to be fastest
 * @param  *coeffs: Pointer to coefficients FIR filter array coeff_size length
 * @param  *StateBuffer: Pointer to state buffer. Set to null to let malloc allocate memory in heap
 * @param  block_size: size of block for FIR filtering. Number of elements for filtering at a time.
 *           When possible, use block_size multiply of 8 samples, because filtering process is designed for this block size to be fastest
 * @retval Pointer to @ref TM_FILTER_FIR_F32_t instance or NULL if allocation failed
 */
TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_Init(size_t coeff_size, const float32_t* coeffs, float32_t* StateBuffer, size_t block_size);

/**
 * @brief  Process data through ARM FIR F32 filter
 * @param  *instance: Pointer to @ref TM_FILTER_FIR_F32_t instance
 * @param  *In: Input data array to process in filter. Length must be the same as block_size parameter in initialization
 * @param  *Out: Output data array to store filtered values. Length must be the same as block_size parameter in initialization
 * @retval Pointer to @ref TM_FILTER_FIR_F32_t instance
 */
TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_Process(TM_FILTER_FIR_F32_t* instance, float32_t* In, float32_t* Out);

/**
 * @brief  Process all input data through ARM FIR F32 filter
 * @param  *instance: Pointer to @ref TM_FILTER_FIR_F32_t instance
 * @param  *In: Input data array to process in filter
 * @param  *Out: Output data array to store filtered values
 * @param  count: Number of elements in input array and output array. Length must be multiply of block size set on initialization
 * @retval Pointer to @ref TM_FILTER_FIR_F32_t instance
 */
TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_ProcessAll(TM_FILTER_FIR_F32_t* instance, float32_t* In, float32_t* Out, size_t count);

/**
 * @brief  Clear filter's state buffer, set values to 0
 * @param  instance: Pointer to TM_FILTER_FIR_F32_t instance
 * @retval Pointer to @ref TM_FILTER_FIR_F32_t structure
 */
TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_Clear(TM_FILTER_FIR_F32_t* instance);

/**
 * @brief  Deallocates ARM FIR filter with all allocated buffers
 * @param  Pointer to @ref TM_FILTER_FIR_F32_t which will be used for deinit
 * @retval None 
 */
void TM_FILTER_FIR_F32_DeInit(TM_FILTER_FIR_F32_t* instance);

/**
 * @brief  Creates and initializes ARM IIR biquad direct form 1 filter using F32 coefficients
 * @param  numStages: Number of 2nd order stages with b0, b1, b2 (numerators), a0, a1 (denumerators) coefficients
 * @param  *coeffs: Pointer to coeffs array. Coefficients length must be 5 * numStages parameter.
 *          Parameters are stored in format: {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
 *          where first number represents stage number and second coefficient number for A and B coefficients.
 * @param  *StateBuffer: Pointer to state buffer. It's length must be for at least 4 * numStages elements. Set to null to let malloc allocate memory in heap
 * @param  block_size: Number of samples to use when processing filter using @ref IIR_Filter_Process_F32 function
 *           When possible, use block_size multiply of 8 samples, because filtering process is designed for this block size to be fastest
 * @retval Pointer to allocated @ref TM_FILTER_IIR_F32_t structure or NULL if allocation failed
 */
TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_Init(size_t numStages, const float32_t* coeffs, float32_t* StateBuffer, size_t block_size);

/**
 * @brief  Processes ARM IIR biquad direct form 1 filter using F32 coefficients
 * @param  *instance: Pointer to @ref TM_FILTER_IIR_F32_t structure with filled input buffer with input data
 * @param  *In: Input data array to process in filter. Length must be the same as block_size parameter in initialization
 * @param  *Out: Output data array to store filtered values. Length must be the same as block_size parameter in initialization
 * @retval Pointer to @ref TM_FILTER_IIR_F32_t structure 
 */
TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_Process(TM_FILTER_IIR_F32_t* instance, float32_t* In, float32_t* Out);

/**
 * @brief  Processes ARM IIR biquad direct form 1 filter using F32 coefficients
 * @param  *instance: Pointer to @ref TM_FILTER_IIR_F32_t structure with filled input buffer with input data
 * @param  *In: Input data array to process in filter
 * @param  *Out: Output data array to store filtered values
 * @param  count: Number of elements in input array and output array. Length must be multiply of block size set on initialization
 * @retval Pointer to @ref TM_FILTER_IIR_F32_t structure 
 */
TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_ProcessAll(TM_FILTER_IIR_F32_t* instance, float32_t* In, float32_t* Out, size_t count);

/**
 * @brief  Clears all filter states and set them to zero
 * @param  *instance: Pointer to @ref TM_FILTER_IIR_F32_t structure where states will be set to zero
 * @retval Pointer to @ref TM_FILTER_IIR_F32_t structure
 */
TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_Clear(TM_FILTER_IIR_F32_t* instance);

/**
 * @brief  Deallocates ARM IIR biquad direct form 1 filter with state buffer
 * @param  Pointer to @ref TM_FILTER_IIR_F32_t which will be used for deinit
 * @retval None 
 */
void TM_FILTER_IIR_F32_DeInit(TM_FILTER_IIR_F32_t* instance);

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
