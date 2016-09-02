/**
 * Keil project example for Analog to Digital converter
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_adc.h"

/* Result value */
uint16_t result;

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init ADC1, Channel 1 = PA1 */
	TM_ADC_Init(ADC1, TM_ADC_Channel_1);
	
	while (1) {
		/* Read value from ADC, 12 bit result is returned */
		result = TM_ADC_Read(ADC1, TM_ADC_Channel_1);
		
		/* Print result if needed to USART */
		
	}
}

