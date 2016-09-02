/**
 * Keil project example for I2C - Multiple write and read bytes from SLAVE
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
uint8_t data_array[6];
	
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
	
	/* Read 5 bytes, device address = MPU6050_ADDRESS, start from register address = 0x1A */
	TM_I2C_ReadMulti(I2C1, MPU6050_ADDRESS, 0x1A, data_array, 5);
	
	/* Format data array to write to device */
	data_array[0] = 0x00; /* Data 0 */
	data_array[1] = 0x00; /* Data 1 */
	data_array[2] = 0x00; /* Data 2 */
	data_array[3] = 0x00; /* Data 3 */
	data_array[4] = 0x00; /* Data 4 */
	
	/* Write 5 bytes via I2C, device address = MPU6050_ADDRESS, register address is 0x1A */
	/* We want to write 5 bytes */
	TM_I2C_WriteMulti(I2C1, MPU6050_ADDRESS, 0x1A, data_array, 5);
	
	while (1) {
		
	}
}
