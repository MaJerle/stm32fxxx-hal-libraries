/**
 * Keil project example for MPU6050 2 devices at a time
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
#include "tm_stm32_mpu6050.h"

/* MPU6050 working structures */
TM_MPU6050_t MPU6050_1;
TM_MPU6050_t MPU6050_2;

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* For pinouts, check TM_MPU6050 library */
	
	/* Try to init first MPU6050, device address is 0xD0, AD0 pin is set to low */
	if (TM_MPU6050_Init(&MPU6050_1, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok) {
		/* Green LED on */
		TM_DISCO_LedOn(LED_GREEN);
	}
	
	/* Try to init second MPU6050, device address is 0xD1, AD0 pin is set to high */
	if (TM_MPU6050_Init(&MPU6050_2, TM_MPU6050_Device_1, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok) {
		/* Green LED on */
		TM_DISCO_LedOn(LED_GREEN);
	}
	
	/* Set new gyro sens for device 1 */
	TM_MPU6050_SetGyroscope(&MPU6050_1, TM_MPU6050_Gyroscope_1000s);
	
	/* Set new acce sens for device 2 */
	TM_MPU6050_SetAccelerometer(&MPU6050_2, TM_MPU6050_Accelerometer_2G);
	
	while (1) {
		/* Read everything from device 1 */
		TM_MPU6050_ReadAll(&MPU6050_1);
		
		/* Raw data are available for use as needed */
		//MPU6050.Accelerometer_X;
		//MPU6050.Accelerometer_Y;
		//MPU6050.Accelerometer_Z;
		//MPU6050.Gyroscope_X;
		//MPU6050.Gyroscope_Y;
		//MPU6050.Gyroscope_Z;
		//MPU6050.Temperature;
		
		/* Read everything from device 2 */
		TM_MPU6050_ReadAll(&MPU6050_2);
		
		/* Raw data are available for use as needed */
		//MPU6050.Accelerometer_X;
		//MPU6050.Accelerometer_Y;
		//MPU6050.Accelerometer_Z;
		//MPU6050.Gyroscope_X;
		//MPU6050.Gyroscope_Y;
		//MPU6050.Gyroscope_Z;
		//MPU6050.Temperature;
		
		/* Delay a little */
		Delayms(1);
	}
}

