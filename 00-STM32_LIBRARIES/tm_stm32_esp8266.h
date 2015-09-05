/**
 *	
 *
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@link		
 *	@version 	v1.0
 *	@ide		Keil uVision
 *	@license	GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *
 * ---------------
 * |
 * |
 * |
 * |
 * |
 * |
 */
#ifndef TM_ESP8266_H
#define TM_ESP8266_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 * Library dependencies
 * - STM32F4xx
 * - STM32F4xx RCC
 * - defines.h
 */
/**
 * Includes
 */
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "defines.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_gpio.h"
#include <string.h>
#include <stdio.h>

/* ESP8266 USART */
#ifndef ESP8266_USART
#define ESP8266_USART				USART1
#define ESP8266_USART_PINSPACK		TM_USART_PinsPack_1
#endif

/* Debug settings */
#define ESP8266_DEBUG_USART			USART3
#define ESP8266_DEBUG_USART_PP		TM_USART_PinsPack_1	
#define ESP8266_DEBUG_SEND(x)		TM_USART_Puts(ESP8266_DEBUG_USART, x);

/* ESP8266 Reset pin */
#ifndef ESP8266_RESET_PIN
#define ESP8266_RESET_PORT			GPIOD
#define ESP8266_RESET_PIN			GPIO_PIN_3
#endif

/* Reset commands */
#define ESP8266_RESET_SET			TM_GPIO_SetPinLow(ESP8266_RESET_PORT, ESP8266_RESET_PIN)
#define ESP8266_RESET_RELEASE		TM_GPIO_SetPinHigh(ESP8266_RESET_PORT, ESP8266_RESET_PIN)

/* Send data to ESP8266 */	
#define ESP8266_USART_SEND(x)		TM_USART_Puts(ESP8266_USART, x); TM_USART_Puts(ESP8266_USART, "\r\n")

/* Is character a digit */
#define ESP8266_IS_DIGIT(x)			((x) >= '0' && (x) <= '9')

/* Char 2 digit conversions */
#define ESP8266_C2N(a)				((a) - 48)
#define ESP8266_C2NM(a, x)			(C2N(a) * (x))

/* Supported ESP8266 FIRMWARE */
#define ESP8266_FIRMWARE			"00200.9.4"
#define ESP8266_FIRMWARE_LEN		10

/* AT commands */
#define ESP8266_AT					"AT"			/* General test */
#define ESP8266_AT_RESET			"AT+RST"		/* Restart the module */
#define ESP8266_AT_FIRMWARE			"AT+GMR"		/* Check firmware version */
#define ESP8266_AT_WIFI_MODE		"AT+CWMODE"		/* Wifi mode */
#define ESP8266_AT_JOIN_AP			"AT+CWJAP"		/* Join the AP */
#define ESP8266_AT_LIST_AP			"AT+CWLAP"		/* List the AP */
#define ESP8266_AT_QUIT_AP			"AT+CWQAP"		/* Quit the AP */
#define ESP8266_AT_SET_AP_PARAMS	"AT+CWSAP"		/* Set the parameters of AP */
#define ESP8266_AT_JOIN_DEVICES		"AT+CWLIF"		/* Check joing devices' IP */
#define ESP8266_AT_GET_CONN_STATUS	"AT+CIPSTATUS"	/* Get the connection status */
#define ESP8266_AT_START_CONN		"AT+CIPSTART"	/* Set up TCP or UDP connection */
#define ESP8266_AT_SET_TRANS_MODE	"AT+CIPMODE"	/* Set data transmission mode */
#define ESP8266_AT_SEND				"AT+CIPSEND"	/* Send data */
#define ESP8266_AT_CLOSE			"AT+CIPCLOSE"	/* Close TCP or UDP connection */
#define ESP8266_AT_GETIP			"AT+CIFSR"		/* Get IP address */
#define ESP8266_AT_SET_MUL_CONN		"AT+CIPMUX"		/* Set multiple connection */
#define ESP8266_AT_SET_SERVER		"AT+CIPSERVER"	/* Set as server */
#define ESP8266_AT_SET_SERV_TIMEOUT	"AT+CIPSTO"		/* Set the server timeout */
#define ESP8266_AT_RECEIVED_DATA	"+IPD"			/* Received data */
#define ESP8266_AT_SET_BAUDRATE		"AT+IPR"		/* Set UART baudrate */
#define ESP8266_AT_ECHO				"ATE"			/* Set ECHO setings */
#define ESP8266_AT_ECHO1			"ATE1"			/* Enable echo */
#define ESP8266_AT_ECHO0			"ATE0"			/* Disable echo */

#define ESP8266_TEXT_BUSY			"busy p"
#define ESP8266_TEXT_OK				"OK"
#define ESP8266_TEXT_SEND_OK		"SEND OK"
#define ESP8266_TEXT_SEND_READY		"ready"

#define ESP8266_REQUEST_BUFFER_LEN	1024

typedef enum {
	TM_ESP8266_Result_Ok = 0,
	TM_ESP8266_Result_Error,
	TM_ESP8266_Result_FirmwareNotValid,
	TM_ESP8266_Result_DeviceError,
	TM_ESP8266_Result_ResetError,
	TM_ESP8266_Result_TimeoutError,
	TM_ESP8266_Result_NoNewData,
	TM_ESP8266_Result_NewData,
	TM_ESP8266_Result_NewClientConnected,
	TM_ESP8266_Result_LineTooLong
} TM_ESP8266_Result_t;

typedef struct {
	uint8_t Connections;
	uint8_t MultiConnections;
	uint32_t IncomingBytes[5];
	char Request[5][ESP8266_REQUEST_BUFFER_LEN];
	uint8_t APIP[4];
	uint8_t STAIP[4];
	uint8_t APMAC[6];
	uint8_t STAMAC[6];
	uint32_t Baudrate;
	TM_ESP8266_Result_t Status;
} TM_ESP8266_t;

typedef enum {
	TM_ESP8266_WifiMode_STA = 1,
	TM_ESP8266_WifiMode_AP = 2,
	TM_ESP8266_WifiMode_Both = 3
} TM_ESP8266_WifiMode_t;

typedef enum {
	TM_ESP8266_Mode_Server,
	TM_ESP8266_Mode_Client
} TM_ESP8266_Mode_t;

typedef enum {
	TM_ESP8266_Set_AP_IP,
	TM_ESP8266_Set_STA_IP,
	TM_ESP8266_Set_AP_MAC,
	TM_ESP8266_Set_STA_MAC
} TM_ESP8266_Set_IP_MAC_t;

typedef enum {
	TM_ESP8266_Baudrate_9600 = 9600,
	TM_ESP8266_Baudrate_19200 = 19200,
	TM_ESP8266_Baudrate_38400 = 38400,
	TM_ESP8266_Baudrate_57600 = 57600,
	TM_ESP8266_Baudrate_74880 = 74880,
	TM_ESP8266_Baudrate_115200 = 115200,
	TM_ESP8266_Baudrate_230400 = 230400,
	TM_ESP8266_Baudrate_460800 = 460800,
	TM_ESP8266_Baudrate_921600 = 921600
} TM_ESP8266_Baudrate_t;

TM_ESP8266_Result_t TM_ESP8266_Init(TM_ESP8266_t* ESP8266_Data, uint32_t USART_Baudrate);
TM_ESP8266_Result_t TM_ESP8266_Reset(TM_ESP8266_t* ESP8266_Data);
TM_ESP8266_Result_t TM_ESP8266_ServerEnable(TM_ESP8266_t* ESP8266_Data, uint32_t port);
TM_ESP8266_Result_t TM_ESP8266_WifiMode(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_WifiMode_t WifiMode);
TM_ESP8266_Result_t TM_ESP8266_CheckIncomingData(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Mode_t mode);
TM_ESP8266_Result_t TM_ESP8266_SendLine(TM_ESP8266_t* ESP8266_Data, uint8_t connection_id, char* str);
TM_ESP8266_Result_t TM_ESP8266_CloseConnection(TM_ESP8266_t* ESP8266_Data, uint8_t connection_id);
TM_ESP8266_Result_t TM_ESP8266_SetBaudrate(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Baudrate_t baud);
TM_ESP8266_Result_t TM_ESP8266_GetIPsMACs(TM_ESP8266_t* ESP8266_Data);
TM_ESP8266_Result_t TM_ESP8266_SetIP_MAC(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Set_IP_MAC_t type, uint8_t* data);
TM_ESP8266_Result_t TM_ESP8266_ClientAvailable(TM_ESP8266_t* ESP8266_Data);
TM_ESP8266_Result_t TM_ESP8266_DisableEcho(TM_ESP8266_t* ESP8266_Data);
TM_ESP8266_Result_t TM_ESP8266_EnableEcho(TM_ESP8266_t* ESP8266_Data);
TM_ESP8266_Result_t TM_ESP8266_CustomCommand(TM_ESP8266_t* ESP8266_Data, char* command, char* response, uint16_t timeout);
	
/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

