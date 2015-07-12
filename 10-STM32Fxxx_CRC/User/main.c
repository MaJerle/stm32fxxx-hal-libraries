/**
 * Keil project template
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
#include "tm_stm32_crc.h"

/* 32-bit data array for CRC unit */
uint32_t data[16] = {
	0x1234, 0x1234, 0x4562, 0x321,
	0x32, 0x879, 0x987, 0x745, 
	0x74, 0x90, 0x98, 0x2432,
	0x7453, 0xAFDE, 0xAAAA, 0xFFFF
};

/* Result variable */
uint32_t result;

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Turn on all leds */
	TM_DISCO_LedOn(LED_ALL);
	
	/* Initialize CRC calculation unit */
	TM_CRC_Init();
	
	/* Calculate CRC from block of 32-bits data array, 16 elements in array, reset CRC unit before start with calculation */
	result = TM_CRC_Calculate32(data, 16, 1);
	
	while (1) {
		/* Do nothing */
	}
}

