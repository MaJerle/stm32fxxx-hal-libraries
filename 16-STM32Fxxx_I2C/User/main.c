/**
 * Keil project example for I2C - Single write and read bytes
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
	
	/* Init I2C, SCL = PB8, SDA = PB9, available on Arduino headers and on all discovery boards */
	/* For STM32F4xx and STM32F7xx lines */
	TM_I2C_Init(I2C1, TM_I2C_PinsPack_2, 100000);
	
	/* Read one byte, device address = MPU6050_ADDRESS, register address = 0x1A */
	TM_I2C_Read(I2C1, MPU6050_ADDRESS, 0x1A, &read);
	
	/* Write single byte via I2C, device address = MPU6050_ADDRESS, register address = 0x0A, data = 0x12 */
	TM_I2C_Write(I2C1, MPU6050_ADDRESS, 0x1A, 0x12);
	
	/* Read one byte, device address = MPU6050_ADDRESS, register address = 0x1A */
	TM_I2C_Read(I2C1, MPU6050_ADDRESS, 0x1A, &read);
	
	/* Check value */
	if (read == 0x12) {
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		/* Toggle LED, indicate wrong */
		while (1) {
			/* Toggle LED */
			TM_DISCO_LedToggle(LED_ALL);
			
			/* Delay 100ms */
			Delayms(100);
		}
	}
	
	while (1) {
		
	}
}
