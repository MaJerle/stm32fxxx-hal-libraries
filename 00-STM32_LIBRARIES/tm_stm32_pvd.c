/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen MAJERLE
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
#include "tm_stm32_pvd.h"

void TM_PVD_Enable(TM_PVD_Level_t Level, TM_PVD_Trigger_t Trigger) {
	PWR_PVDTypeDef ConfigPVD;
	
	/* Enable PWR clock */
	__HAL_RCC_PWR_CLK_ENABLE();
	
	/* Set interrupt to NVIC */
	HAL_NVIC_SetPriority(PVD_IRQn, PVD_NVIC_PRIORITY, PVD_NVIC_SUBPRIORITY);
	
	/* Enable interrupt */
	HAL_NVIC_EnableIRQ(PVD_IRQn);
	
	/* Set level and mode */
	ConfigPVD.PVDLevel = (uint32_t)Level;
	ConfigPVD.Mode = Trigger;
	
	/* Config and enable PVD */
	HAL_PWR_ConfigPVD(&ConfigPVD);
	
	/* Enable PVD */
	HAL_PWR_EnablePVD();
}

void TM_PVD_Disable(void) {
	/* Disable PVD */
	HAL_PWR_DisablePVD();
	
	/* Disable EXTI interrupt for PVD */
	__HAL_PWR_PVD_EXTI_DISABLE_IT();

	/* Disable NVIC */
	NVIC_DisableIRQ(PVD_IRQn);
}

/*****************************************************************/
/*                 PVD INTERRUPT USER CALLBACK                   */
/*****************************************************************/
__weak void TM_PVD_Handler(uint8_t status) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_PVD_Handler could be implemented in the user file
	*/
}

/*****************************************************************/
/*                    PVD INTERRUPT HANDLER                      */
/*****************************************************************/
void PVD_IRQHandler(void) {
	/* Call user function if needed */
	if (__HAL_PWR_PVD_EXTI_GET_FLAG() != RESET) {
#if defined(PWR_CSR_PVDO)	
		/* Call user function with status */
		TM_PVD_Handler((PWR->CSR & PWR_CSR_PVDO) ? 1 : 0);
#endif
#if defined(PWR_CSR1_PVDO)
		/* Call user function with status */
		TM_PVD_Handler((PWR->CSR1 & PWR_CSR1_PVDO) ? 1 : 0);
#endif
		/* Clear PWR EXTI pending bit */
		__HAL_PWR_PVD_EXTI_CLEAR_FLAG();
	}
}
