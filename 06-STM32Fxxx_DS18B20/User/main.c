/**
 * Keil project example for DS18B20 temperature sensor
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
#include "tm_stm32_ds18b20.h"
#include "tm_stm32_onewire.h"

/* Onewire structure */
TM_OneWire_t OW;

/* Array for DS18B20 ROM number */
uint8_t DS_ROM[8];

/* Temperature variable */
float temp;

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init ONEWIRE port on PB4 pin */
	TM_OneWire_Init(&OW, GPIOB, GPIO_PIN_4);
	
	/* Check if any device is connected */
	if (TM_OneWire_First(&OW)) {
		/* Set LED GREEN */
		TM_DISCO_LedOn(LED_GREEN);
		
		/* Read ROM number */
		TM_OneWire_GetFullROM(&OW, DS_ROM);
	} else {
		/* Set LED RED */
		TM_DISCO_LedOn(LED_RED);
	}
	
	/* Start temp conversion */
	if (TM_DS18B20_Is(DS_ROM)) {
		/* Set resolution */
		TM_DS18B20_SetResolution(&OW, DS_ROM, TM_DS18B20_Resolution_12bits);
		
		/* Set high and low alarms */
		TM_DS18B20_SetAlarmHighTemperature(&OW, DS_ROM, 30);
		TM_DS18B20_SetAlarmLowTemperature(&OW, DS_ROM, 10);
		
		/* Start conversion on all sensors */
		TM_DS18B20_StartAll(&OW);
	}
	
	while (1) {
		/* Check if connected device is DS18B20 */
		if (TM_DS18B20_Is(DS_ROM)) {
			/* Everything is done */
			if (TM_DS18B20_AllDone(&OW)) {
				/* Read temperature from device */
				if (TM_DS18B20_Read(&OW, DS_ROM, &temp)) {
					/* Temp read OK, CRC is OK */
					
					/* Start again on all sensors */
					TM_DS18B20_StartAll(&OW);
					
					/* Check temperature */
					if (temp > 30) {
						TM_DISCO_LedOn(LED_RED);
					} else {
						TM_DISCO_LedOff(LED_RED);
					}
				} else {
					/* CRC failed, hardware problems on data line */
				}
			}
		}
	}
}

