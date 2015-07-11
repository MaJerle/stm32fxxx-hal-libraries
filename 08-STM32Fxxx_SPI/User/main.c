/**.
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
#include "tm_stm32_spi.h"
#include "string.h"

uint8_t Transmit[15], Receive[15];

int main(void) {
	uint8_t i;
	
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();

	/* Init SPI */
#if defined(STM32F7_DISCOVERY)
	/* Use custom for STM32F7-Discovery, callback function will be called for SPI GPIO initialization */
	TM_SPI_Init(SPI2, TM_SPI_PinsPack_Custom);
#else
	/* Use PB13,14,15 pins for SPI */
	TM_SPI_Init(SPI2, TM_SPI_PinsPack_2);
#endif
	
	/* Send multi bytes */
	for (i = 0; i < 15; i++) {
		Transmit[i] = i;
		
		/* Check for receive */
		Receive[i] = TM_SPI_Send(SPI2, Transmit[i]);
	}
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Memory compare */
	if (memcmp(Transmit, Receive, 15) == 0) {
		/* Turn on GREEN LED = Everything OK */
		TM_DISCO_LedOn(LED_GREEN);
	} else {
		/* Turn on RED LED = SPI error */
		TM_DISCO_LedOn(LED_RED);
	}
	
	while (1) {
		/* Do nothing */
	}
}

/* Custom SPI initialization */
void TM_SPI_InitCustomPinsCallback(SPI_TypeDef* SPIx, uint16_t AlternateFunction) {
	/* SPI callback */
	if (SPIx == SPI2) {
		/* Pins on STM32F7-Discovery on Arduino header */
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, AlternateFunction);
		TM_GPIO_InitAlternate(GPIOI, GPIO_PIN_1, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, AlternateFunction);
	}
}
