/**
 * Keil project example for I2C - DOES NOT WORK YET!
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
#include "tm_stm32_i2c.h"

uint8_t byte;
	
int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init I2C */
	TM_I2C_Init(I2C3, TM_I2C_PinsPack_1, 100000);
	
	/* Read one byte */
	byte = TM_I2C_Read(I2C3, 0x82, 0x00);
	
	TM_I2C_Write(I2C3, 0x82, 0x41, 0x9A);
	
	/* Read one byte */
	byte = TM_I2C_Read(I2C3, 0x82, 0x41);
	
	if (byte == 0x9A) {
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		TM_DISCO_LedOn(LED_RED);
	}
	
	while (1) {
		
	}
}

