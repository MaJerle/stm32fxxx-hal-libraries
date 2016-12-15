/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_disco.h"

/* Button pressed status for onpressed/onreleased events */
static volatile uint8_t TM_INT_DISCO_ButtonPressed = 0;

/* Check for functions use */
#if defined(DISCO_USE_FUNCTIONS)

/* Led structure */
typedef struct {
	GPIO_TypeDef* Port;
	uint16_t Pin;
} DISCO_Led_t;

/* Create variable with all leds */
static DISCO_Led_t DISCO_Leds[] = {
	{
#if defined(DISCO_LED_GREEN_PORT)
		DISCO_LED_GREEN_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_GREEN,
	},
	{
#if defined(DISCO_LED_RED_PORT)
		DISCO_LED_RED_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_RED,
	},
	{
#if defined(DISCO_LED_ORANGE_PORT)
		DISCO_LED_ORANGE_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_ORANGE,
	},
	{
#if defined(DISCO_LED_BLUE_PORT)
		DISCO_LED_BLUE_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_BLUE,
	}
};

#endif

void TM_DISCO_LedInit(void) {
#if defined(DISCO_USE_FUNCTIONS)
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Set pin as output */
		TM_GPIO_Init(DISCO_Leds[i].Port, DISCO_Leds[i].Pin, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	}
#else
	/* Set pins as output */
	TM_GPIO_Init(DISCO_LED_PORT, LED_ALL, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
#endif
	
	/* Turn leds off */
	TM_DISCO_LedOff(LED_ALL);
}

void TM_DISCO_ButtonInit(void) {
	/* Set pin as input */
	TM_GPIO_Init(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, DISCO_BUTTON_PULL, TM_GPIO_Speed_Low);
}

uint8_t TM_DISCO_ButtonOnPressed(void) {
	/* If button is now pressed, but was not already pressed */
	if (TM_DISCO_ButtonPressed()) {
		if (!TM_INT_DISCO_ButtonPressed) {
			/* Set flag */
			TM_INT_DISCO_ButtonPressed = 1;
			
			/* Return button onpressed */
			return 1;
		}
	} else {
		/* Clear flag */
		TM_INT_DISCO_ButtonPressed = 0;
	}
	
	/* Button is not pressed or it was already pressed before */
	return 0;
}

uint8_t TM_DISCO_ButtonOnReleased(void) {
	/* If button is now released, but was not already released */
	if (!TM_DISCO_ButtonPressed()) {
		if (TM_INT_DISCO_ButtonPressed) {
			/* Set flag */
			TM_INT_DISCO_ButtonPressed = 0;
			
			/* Return button onreleased */
			return 1;
		}
	} else {
		/* Set flag */
		TM_INT_DISCO_ButtonPressed = 1;
	}
	
	/* Button is not released or it was already released before */
	return 0;
}

/* Check for functions use */
#if defined(DISCO_USE_FUNCTIONS)

void TM_DISCO_LedToggle(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
			TM_GPIO_TogglePinValue(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
		}
	}
}

void TM_DISCO_LedOn(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
#if defined(DISCO_SWAP_LOGIC)
			TM_GPIO_SetPinLow(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#else
			TM_GPIO_SetPinHigh(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#endif
		}
	}
}

void TM_DISCO_LedOff(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
#if defined(DISCO_SWAP_LOGIC)
			TM_GPIO_SetPinHigh(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#else
			TM_GPIO_SetPinLow(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#endif
		}
	}
}

uint16_t TM_DISCO_LedIsOn(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
			/* Check first LED */
#if defined(DISCO_SWAP_LOGIC)
			return !TM_GPIO_GetOutputPinValue(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#else
			return TM_GPIO_GetOutputPinValue(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#endif
		}
	}
	
	/* Led not valid */
	return 0;
}

#endif
