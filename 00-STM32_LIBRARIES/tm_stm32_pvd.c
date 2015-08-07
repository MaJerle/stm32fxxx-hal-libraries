/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#if defined(STM32F0xx) || defined(STM32F4xx)	
		/* Call user function with status */
		TM_PVD_Handler((PWR->CSR & PWR_CSR_PVDO) ? 1 : 0);
#endif
#if defined(STM32F7xx)
		/* Call user function with status */
		TM_PVD_Handler((PWR->CSR1 & PWR_CSR1_PVDO) ? 1 : 0);
#endif
		/* Clear PWR EXTI pending bit */
		__HAL_PWR_PVD_EXTI_CLEAR_FLAG();
	}
}
