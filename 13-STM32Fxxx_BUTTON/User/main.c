/**
 * Keil project example for button handling
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
#include "tm_stm32_button.h"

/* Button pointer */
TM_BUTTON_t* MyButton;

/* Button callback function */
static void BUTTON_Callback(TM_BUTTON_t* ButtonPtr, TM_BUTTON_PressType_t PressType);

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button, PORT, PIN, STATE when PRESSED */
	MyButton = TM_BUTTON_Init(TM_DISCO_BUTTON_PORT, TM_DISCO_BUTTON_PIN, TM_DISCO_BUTTON_PRESSED, BUTTON_Callback);

	/* Init all your buttons you have in project if needed */
	//MyButton2 = TM_BUTTON_Init(TM_DISCO_BUTTON_PORT, TM_DISCO_BUTTON_PIN, TM_DISCO_BUTTON_PRESSED, BUTTON_Callback);
	//MyButton3 = TM_BUTTON_Init(TM_DISCO_BUTTON_PORT, TM_DISCO_BUTTON_PIN, TM_DISCO_BUTTON_PRESSED, BUTTON_Callback);
	
	/* Set time how button is detected, 30 ms for normal press, 2000 ms for long press */
	TM_BUTTON_SetPressTime(MyButton, 30, 2000);
	
	while (1) {
		/* Update all buttons */
		TM_BUTTON_Update();
	}
}

/* Implement handler function */
static void BUTTON_Callback(TM_BUTTON_t* ButtonPtr, TM_BUTTON_PressType_t PressType) {
	/* Normal press detected */
	if (PressType == TM_BUTTON_PressType_Normal) {
		/* Set LEDS ON */
		TM_DISCO_LedOn(LED_ALL);
	} else if (PressType == TM_BUTTON_PressType_Long) {
		/* Set LEDS OFF */
		TM_DISCO_LedOff(LED_ALL);
	}
}
