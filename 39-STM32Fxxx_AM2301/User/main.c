/**
 * Keil project example for AM2301 (DHT-21) temperature sensor
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
#include "tm_stm32_am2301.h"

/* AM2301 data structure */
TM_AM2301_t AM2301_1;
TM_AM2301_t AM2301_2;

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init delay */
	TM_DELAY_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init 2 sensors */
	
	/* Sensor 1 on PA1 */
	TM_AM2301_Init(&AM2301_1, GPIOA, GPIO_PIN_1);
	
	/* Sensor 2 on PB3 */
	TM_AM2301_Init(&AM2301_2, GPIOB, GPIO_PIN_3);
	
	while (1) {
		/* Every 1 second read sensors */
		if (TM_DELAY_Time() >= 1000) {
			TM_DELAY_SetTime(0);
			
			/* Check if sensor 1 reading OK */
			if (TM_AM2301_Read(&AM2301_1) == TM_AM2301_Result_Ok) {
				/* Temp and humidity available here in AM2301_1 structure */
			}
			
			/* Check if sensor 2 reading OK */
			if (TM_AM2301_Read(&AM2301_2) == TM_AM2301_Result_Ok) {
				/* Temp and humidity available here in AM2301_2 structure */
			}
		}
	}
}

