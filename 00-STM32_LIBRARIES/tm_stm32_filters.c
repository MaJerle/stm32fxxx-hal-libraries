/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_filters.h"

/* Private functions */
static
void TM_FILTER_FIR_F32_Free(TM_FILTER_FIR_F32_t* instance) {
	/* Free state buffer */
	if (instance->f->pState != NULL && instance->Flags.F.MallocStates) {
		LIB_FREE_FUNC(instance->f->pState);
	}
	
	/* Free arm instance */
	if (instance->f != NULL) {
		LIB_FREE_FUNC(instance->f);
	}
	
	/* Free main instance */
	if (instance != NULL) {
		LIB_FREE_FUNC(instance);
	}
}

static
void TM_FILTER_IIR_F32_Free(TM_FILTER_IIR_F32_t* instance) {
	/* Free state buffer */
	if (instance->f->pState != NULL && instance->Flags.F.MallocStates) {
		LIB_FREE_FUNC(instance->f->pState);
	}
	
	/* Free arm instance */
	if (instance->f != NULL) {
		LIB_FREE_FUNC(instance->f);
	}
	
	/* Free main instance */
	if (instance != NULL) {
		LIB_FREE_FUNC(instance);
	}
}

TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_Init(size_t coeff_size, const float32_t* coeffs, float32_t* StateBuffer, size_t block_size) {
	TM_FILTER_FIR_F32_t* instance;
	
	/* Allocate memory for TM_FILTER_FIR_F32_t */
	instance = (TM_FILTER_FIR_F32_t *) LIB_ALLOC_FUNC(sizeof(TM_FILTER_FIR_F32_t));
	/* Check value */
	if (instance == NULL) {
		return NULL;
	}
	
	/* Reset structure */
	memset(instance, 0, sizeof(TM_FILTER_FIR_F32_t));
	
	/* Save */
	instance->BlockSize = block_size;
	instance->StatesCount = coeff_size + block_size - 1;
	
	/* Allocate memory for arm_fir_instance_f32 */
	instance->f = (arm_fir_instance_f32 *) LIB_ALLOC_FUNC(sizeof(arm_fir_instance_f32));
	
	/* Check value */
	if (instance->f == NULL) {
		TM_FILTER_FIR_F32_Free(instance);
		return NULL;
	}
	
	/* Reset structure */
	memset(instance->f, 0, sizeof(arm_fir_instance_f32));
	
	/* Allocate state buffer */
	if (StateBuffer == NULL) {
		/* Allocate memory for states */
		instance->f->pState = (float32_t *) LIB_ALLOC_FUNC(instance->StatesCount * sizeof(float32_t));
		
		/* Check value */
		if (instance->f->pState == NULL) {
			TM_FILTER_FIR_F32_Free(instance);
			return NULL;
		}
		instance->Flags.F.MallocStates = 1;
	} else {
		instance->f->pState = StateBuffer;
	}
	
	/* Call ARM FIR initialization */
	arm_fir_init_f32(instance->f, coeff_size, (float32_t *)coeffs, instance->f->pState, block_size);
	
	/* Return pointer */
	return instance;
}

TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_Process(TM_FILTER_FIR_F32_t* instance, float32_t* In, float32_t* Out) {
	/* Check input values */
	if (instance == NULL || In == NULL || Out == NULL || instance->f == NULL) {
		return NULL;
	}
		
	/* Call ARM FIR function */
	arm_fir_f32(instance->f, In, Out, instance->BlockSize);
	
	/* Return instance value */
	return instance;
}

TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_ProcessAll(TM_FILTER_FIR_F32_t* instance, float32_t* In, float32_t* Out, size_t count) {
	uint32_t blocks;
	
	/* Check input values */
	if (instance == NULL || In == NULL || Out == NULL || instance->f == NULL) {
		return NULL;
	}
	
	/* Get number of blocks to process */
	blocks = count / instance->BlockSize;
	
	while (blocks-- > 0U) {
		/* Call ARM FIR function */
		arm_fir_f32(instance->f, In, Out, instance->BlockSize);
		
		/* Increase pointers */
		In += instance->BlockSize;
		Out += instance->BlockSize;
	}
	
	/* Get remaining bytes */
	blocks = count % instance->BlockSize;
	
	while (blocks-- > 0U) {
		/* Call ARM FIR function */
		arm_fir_f32(instance->f, In, Out, 1);
		
		/* Increase pointers */
		In++;
		Out++;
	}
	
	/* Return instance value */
	return instance;
}

TM_FILTER_FIR_F32_t* TM_FILTER_FIR_F32_Clear(TM_FILTER_FIR_F32_t* instance) {
	/* No filter selected */
	if (instance == NULL) {
		return instance;
	}
	
	/* Set all to 0 */
	memset(instance->f->pState, 0, instance->StatesCount * sizeof(float32_t));
	
	/* Return instance */
	return instance;
}

void TM_FILTER_FIR_F32_DeInit(TM_FILTER_FIR_F32_t* instance) {
	/* Free allocated memory for filter */
	TM_FILTER_FIR_F32_Free(instance);
}

TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_Init(size_t numStages, const float32_t* coeffs, float32_t* StateBuffer, size_t block_size) {
	TM_FILTER_IIR_F32_t* instance;
	
	/* Allocate memory for TM_FILTER_IIR_F32_t */
	instance = (TM_FILTER_IIR_F32_t *) LIB_ALLOC_FUNC(sizeof(TM_FILTER_IIR_F32_t));
	
	/* Check value */
	if (instance == NULL) {
		return NULL;
	}
	
	/* Reset structure */
	memset(instance, 0, sizeof(TM_FILTER_IIR_F32_t));
	
	/* Save block size */
	instance->BlockSize = block_size;
	
	/* Allocate memory for arm_fir_instance_f32 */
	instance->f = (arm_biquad_casd_df1_inst_f32 *) LIB_ALLOC_FUNC(sizeof(arm_biquad_casd_df1_inst_f32));
	
	/* Check value */
	if (instance->f == NULL) {
		TM_FILTER_IIR_F32_Free(instance);
		return NULL;
	}
	
	/* Reset structure */
	memset(instance->f, 0, sizeof(arm_biquad_casd_df1_inst_f32));
	
	/* Allocate states buffer */
	if (StateBuffer == NULL) {
		/* Allocate memory for states */
		instance->f->pState = (float32_t *) LIB_ALLOC_FUNC((4 * numStages) * sizeof(float32_t));
		
		/* Check value */
		if (instance->f->pState == NULL) {
			TM_FILTER_IIR_F32_Free(instance);
			return NULL;
		}
		instance->Flags.F.MallocStates = 1;
	} else {
		instance->f->pState = StateBuffer;
	}
	
	/* Call ARM FIR initialization */
	arm_biquad_cascade_df1_init_f32(instance->f, numStages, (float32_t *)coeffs, instance->f->pState);
	
	/* Return pointer */
	return instance;
}

TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_Process(TM_FILTER_IIR_F32_t* instance, float32_t* In, float32_t* Out) {
	/* Check if OK */
	if (instance == NULL || In == NULL || Out == NULL || instance->f == NULL) {
		return NULL;
	}
	
	/* Call ARM FIR function */
	arm_biquad_cascade_df1_f32(instance->f, In, Out, instance->BlockSize);
	
	/* Return instance value */
	return instance;
}

TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_ProcessAll(TM_FILTER_IIR_F32_t* instance, float32_t* In, float32_t* Out, size_t count) {
	uint32_t blocks = 0;
	
	/* Check if OK */
	if (instance == NULL || In == NULL || Out == NULL || instance->f == NULL) {
		return NULL;
	}
	
	/* Get number of blocks to process */
	blocks = count / instance->BlockSize;
	
	while (blocks-- > 0U) {
		/* Call ARM FIR function */
		arm_biquad_cascade_df1_f32(instance->f, In, Out, instance->BlockSize);
		
		/* Increase pointers */
		In += instance->BlockSize;
		Out += instance->BlockSize;
	}
	
	/* Get remaining bytes */
	blocks = count % instance->BlockSize;
	
	while (blocks-- > 0U) {
		/* Call ARM FIR function */
		arm_biquad_cascade_df1_f32(instance->f, In, Out, 1);
		
		/* Increase pointers */
		In++;
		Out++;
	}
	
	/* Return instance value */
	return instance;
}

TM_FILTER_IIR_F32_t* TM_FILTER_IIR_F32_Clear(TM_FILTER_IIR_F32_t* instance) {
	/* No filter selected */
	if (instance == 0) {
		return instance;
	}
	
	/* Set all to 0 */
	memset(instance->f->pState, 0, 4 * instance->f->numStages * sizeof(float32_t));
	
	/* Return instance */
	return instance;
}

void TM_FILTER_IIR_F32_DeInit(TM_FILTER_IIR_F32_t* instance) {
	/* Free allocated memory for filter */
	TM_FILTER_IIR_F32_Free(instance);
}
