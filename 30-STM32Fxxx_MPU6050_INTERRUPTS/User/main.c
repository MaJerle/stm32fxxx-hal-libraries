/**
 * Keil project example for MPU6050 device with interrupts capability
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
#include "tm_stm32_mpu6050.h"
#include "tm_stm32_exti.h"

/* MPU6050 working structure */
TM_MPU6050_t MPU6050;

/* Interrupts structure */
TM_MPU6050_Interrupt_t MPU6050_Interrupts;

/* Define GPIO PORT and PIN for interrupt handling */
#define IRQ_PORT    GPIOA
#define IRQ_PIN     GPIO_PIN_0

/* Flag when we should read */
uint32_t read = 0;

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* For pinouts check TM_MPU6050 library */
	
	/* Try to init MPU6050, device address is 0xD0, AD0 pin is set to low */
	if (TM_MPU6050_Init(&MPU6050, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok) {
		/* Green LED on */
		TM_DISCO_LedOn(LED_GREEN);
	}
	
	/* Set data rate to 100 Hz */
	TM_MPU6050_SetDataRate(&MPU6050, TM_MPU6050_DataRate_100Hz);
	
	/* Enable MPU interrupts */
	TM_MPU6050_EnableInterrupts(&MPU6050);
	
	/* Enable interrupts on STM32Fxx device, rising edge */
	TM_EXTI_Attach(IRQ_PORT, IRQ_PIN, TM_EXTI_Trigger_Rising);
	
	while (1) {
		/* If IRQ happen */
		if (read) {
			/* Reset */
			read = 0;
			
			/* Read interrupts */
			TM_MPU6050_ReadInterrupts(&MPU6050, &MPU6050_Interrupts);
			
			/* Check if motion is detected */
			if (MPU6050_Interrupts.F.MotionDetection) {
				/* Toggle RED */
				TM_DISCO_LedToggle(LED_RED);
			}
			
			/* Check if data ready */
			if (MPU6050_Interrupts.F.DataReady) {
				/* Read everything from device */
				TM_MPU6050_ReadAll(&MPU6050);
				
				/* Raw data are available for use when needed */
				//MPU6050.Accelerometer_X;
				//MPU6050.Accelerometer_Y;
				//MPU6050.Accelerometer_Z;
				//MPU6050.Gyroscope_X;
				//MPU6050.Gyroscope_Y;
				//MPU6050.Gyroscope_Z;
				//MPU6050.Temperature;
			}
		}
	}
}

/* EXTI handler */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Check for PIN */
	if (GPIO_Pin == IRQ_PIN) {
		/* Read interrupts from MPU6050 */
		read = 1;
	}
}

