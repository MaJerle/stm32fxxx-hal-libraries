/**
 * Keil project example for I2C - Custom pins for I2C
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
#include "tm_stm32_i2c.h"

/* MPU-6050 device address */
#define MPU6050_ADDRESS     0xD0

/* Byte value read from external device */
uint8_t read;
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
  
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init delay */
	TM_DELAY_Init();
	
	/* Init I2C, use custom pins, callback function will be caleed */
	/* For STM32F4xx and STM32F7xx lines */
	TM_I2C_Init(I2C1, TM_I2C_PinsPack_Custom, 100000);
	
	while (1) {
		
	}
}

/* Called if TM_I2C_PinsPack_Custom is selected when initializing I2C */
void TM_I2C_InitCustomPinsCallback(I2C_TypeDef* I2Cx, uint16_t AlternateFunction) {
	/* Check for proper I2C */
	if (I2Cx == I2C1) {
		/* Init pins PB8 and PB9 for I2C1 */
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low, AlternateFunction);
	}
}
