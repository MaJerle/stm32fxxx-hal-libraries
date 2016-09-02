/**
 * Keil project example for NRF24L01+
 *
 * Transmitter code
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
#include "tm_stm32_nrf24l01.h"
#include "tm_stm32_usart.h"

/* My address */
uint8_t MyAddress[] = {
	0xE7,
	0xE7,
	0xE7,
	0xE7,
	0xE7
};
/* Receiver address */
uint8_t TxAddress[] = {
	0x7E,
	0x7E,
	0x7E,
	0x7E,
	0x7E
};

/* Data received and data for send */
uint8_t dataOut[32], dataIn[32];

/* NRF transmission status */
TM_NRF24L01_Transmit_Status_t transmissionStatus;

/* Buffer for strings */
char str[40];

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Initialize USART, TX: PB6, RX: PB7 */
	TM_USART_Init(USART1, TM_USART_PinsPack_2, 115200);
	
	/* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
	/* By default 2Mbps data rate and 0dBm output power */
	/* NRF24L01 goes to RX mode by default */
	TM_NRF24L01_Init(15, 32);
	
	/* Set 2MBps data rate and -18dBm output power */
	TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_M18dBm);
	
	/* Set my address, 5 bytes */
	TM_NRF24L01_SetMyAddress(MyAddress);
	
	/* Set TX address, 5 bytes */
	TM_NRF24L01_SetTxAddress(TxAddress);
	
	/* Reset counter */
	TM_DELAY_SetTime(2001);
	
	while (1) {
		/* Every 2 seconds */
		if (TM_DELAY_Time() > 2000) {
			
			/* Fill data with something */
			sprintf((char *)dataOut, "abcdefghijklmnoszxABCDEFCBDA");
			
			/* Display on USART */
			TM_USART_Puts(USART1, "pinging: ");
			
			/* Reset time, start counting microseconds */
			TM_DELAY_SetTime(0);
			
			/* Transmit data, goes automatically to TX mode */
			TM_NRF24L01_Transmit(dataOut);
			
			/* Turn on led to indicate sending */
			TM_DISCO_LedOn(LED_GREEN);
			
			/* Wait for data to be sent */
			do {
				/* Get transmission status */
				transmissionStatus = TM_NRF24L01_GetTransmissionStatus();
			} while (transmissionStatus == TM_NRF24L01_Transmit_Status_Sending);
			
			/* Turn off led */
			TM_DISCO_LedOff(LED_GREEN);
			
			/* Go back to RX mode */
			TM_NRF24L01_PowerUpRx();
			
			/* Wait received data, wait max 100ms, if time is larger, then data were probably lost */
			while (!TM_NRF24L01_DataReady() && TM_DELAY_Time() < 100);
			
			/* Format time */
			sprintf(str, "%d ms", TM_DELAY_Time());
			
			/* Show ping time */
			TM_USART_Puts(USART1, str);
			
			/* Get data from NRF2L01+ */
			TM_NRF24L01_GetData(dataIn);
			
			/* Check transmit status */
			if (transmissionStatus == TM_NRF24L01_Transmit_Status_Ok) {
				/* Transmit went OK */
				TM_USART_Puts(USART1, ": OK\n");
			} else if (transmissionStatus == TM_NRF24L01_Transmit_Status_Lost) {
				/* Message was LOST */
				TM_USART_Puts(USART1, ": LOST\n");
			} else {
				/* This should never happen */
				TM_USART_Puts(USART1, ": SENDING\n");
			}
		}
	}
}
