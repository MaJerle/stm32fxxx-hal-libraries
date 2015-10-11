/**-
 * Keil project example for Digital to Analog converter
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
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
#include "tm_stm32_dac.h"
	
int main(void) {
	uint16_t values[2] = {0, 0};
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init DAC channel 1 = PA4 */
	TM_DAC_Init(TM_DAC_Channel_1);
	
	/* Init DAC channel 2 = PA5 */
	TM_DAC_Init(TM_DAC_Channel_2);
	
	while (1) {
		/* Toggle ALL leds */
		if (TM_DELAY_Time() > 200) {	
			/* Toggle leds */
			TM_DISCO_LedToggle(LED_ALL);
		}
		
		/* Increase channel 1 value */
		values[0]++;
		
		/* Decrease channel 2 value */
		values[1]--;
		
		/* Check if channel 1 is overflowed 12 bit and set it to zero */
		if (values[0] > 0x0FFF) {
			values[0] = 0;
		}
		
		/* Check if channel 2 is less than zero (overflow to 0xFFFF) and set to to max 12 bit value */
		if (values[1] > 0x0FFF) {
			values[1] = 0x0FFF;
		}
		
		/* Set DAC channel 1 = PA4 */
		TM_DAC_SetValue(TM_DAC_Channel_1, values[0]);
		
		/* Set DAC channel 2 = PA5 */
		TM_DAC_SetValue(TM_DAC_Channel_2, values[1]);
		
		/* Delay 1ms */
		Delayms(1);
	}
}

