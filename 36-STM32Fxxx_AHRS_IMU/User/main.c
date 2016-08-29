/**
 * Keil project example for AHRS IMU alhorithm
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
#include "tm_stm32_ahrs_imu.h"

/* AHRS/IMU structure */
TM_AHRSIMU_t AHRSIMU;

int main(void) {	
	/* Initialize system */
	SystemInit();
	
	/* Delay init */
	TM_DELAY_Init();
    
    /* Init structure with 100hZ sample rate, 0.1 beta and 3.5 inclination (3.5 degrees is inclination in Ljubljana, Slovenia) on July, 2016 */
    TM_AHRSIMU_Init(&AHRSIMU, 100, 0.1f, 3.5f);
	
    /* The best way is to init IRQ on MCU and set motion sensor to toggle pin at data rate you set on initialization */
    /* Check fake interrupt function */
    
    while (1) {
        
	}
}

/*
My fake sensor has:

1. Gyro with +-2000 degrees/s with 16-bits
    +- 2000 at 16 bits means -32768 is -2000 and 32767 is 2000 degrees

2. Accelerometer with +-2 G and 16 bits
    +- 2 at 16 bits means -32768 is -2 and 32767 is 2 gees

3. Magnetic outputs uT directly
*/
void IRQ_Func(void) {
    float gx, gy, gz, ax, ay, az, mx, my, mz;
    
    /* Read data from sensor and format them into variables above directly from registers */
    
    /* Convert data to gees, deg/sec and microTesla respectively */
    gx = (gx * 2000.0f) / 32768.0f;
    gy = (gy * 2000.0f) / 32768.0f;
    gz = (gz * 2000.0f) / 32768.0f;
    
    ax = (ax * 2.0f) / 32768.0f;
    ay = (ay * 2.0f) / 32768.0f;
    az = (az * 2.0f) / 32768.0f;
    
    //...
    
    /* Call update function */
    /* This function must be called periodically in inteervals set by sample rate on initialization process */
    TM_AHRSIMU_UpdateAHRS(&AHRSIMU, AHRSIMU_DEG2RAD(gx), AHRSIMU_DEG2RAD(gy), AHRSIMU_DEG2RAD(gz), ax, ay, az, mx, my, mz);
    
    /* Read new roll, pitch and yaw values */
    printf("Roll: %f; Pitch: %f, Yaw: %f\r\n", AHRSIMU.Roll, AHRSIMU.Pitch, AHRSIMU.Yaw);
}

