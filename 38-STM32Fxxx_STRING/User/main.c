/**
 * Keil project template
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen MAJERLE
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
#include "tm_stm32_string.h"
#include "tm_stm32_usart.h"

/* String structure */
TM_STRING_t* String;
	
int main(void) {
	uint16_t i, strposition;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init USART6, TX: PC6, RX: PC7, 921600 bauds */
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 921600);
	
	/* Create string with 10 predefined locations for strings */
	String = TM_STRING_Create(10);
	
	/* Add string to memory, allocated memory will be set depending on string length */
	TM_STRING_AddString(String, "First string");
	
	/* Add another string to memory, allocated memory will be set depending on string length */
	TM_STRING_AddString(String, "Second string");
	
	/* Send strings over USART */
	for (i = 0; i < String->Count; i++) {
		/* Print string to user */
		printf("%s\n", String->Strings[i]);
	}
	
	/* Add some strings */
	/* Add thirt string */
	TM_STRING_AddString(String, "Third string");
	
	/* Add forth string. Function will return string position in array */
	strposition = TM_STRING_AddString(String, "Forth string");
	
	/* Add fifth string */
	TM_STRING_AddString(String, "Fifth string");
	
	/* Modify string number 4 */
	TM_STRING_ReplaceString(String, strposition, "Updated forth string");
	
	/* Send strings over USART */
	for (i = 0; i < String->Count; i++) {
		/* Print string to user */
		printf("%s\n", String->Strings[i]);
	}
	
	/* Delete string on position 1 = "Second string" */
	TM_STRING_DeleteString(String, 1);
	
	/* Free entire string memory with all strings */
	TM_STRING_FreeAll(String);
	
	while (1) {
		/* Do nothing */
	}
}

/* Printf handler */
int fputc(int ch, FILE* fil) {
	/* Send over USART */
	TM_USART_Putc(USART6, ch);
	
	/* Return character */
	return ch;
}
