/**	
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
 */
#include "tm_stm32f4_esp8266.h"

/* private variables */
char ESP8266_Buffer[ESP8266_REQUEST_BUFFER_LEN];

volatile uint8_t ipd_detected = 0;
volatile int8_t connectionID = -1;
volatile uint32_t incomingbytes = 0;
volatile uint32_t baudrate;

/* Private methods */
TM_ESP8266_Result_t TM_ESP8266_INT_Test(void);
TM_ESP8266_Result_t TM_ESP8266_INT_CheckFirmware(void);
uint8_t TM_ESP8266_INT_StringStartsWith(char* string, const char* str);
TM_ESP8266_Result_t TM_ESP8266_INT_ReturnWithStatus(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Result_t status);
void TM_ESP8266_INT_InitPins(void);
uint8_t TM_ESP866_INT_GetNumFromHex(char c);
TM_ESP8266_Result_t TM_ESP8266_INT_CheckIncomingData(TM_ESP8266_t* ESP8266_Data, char* buffer);

TM_ESP8266_Result_t TM_ESP8266_Init(TM_ESP8266_t* ESP8266_Data, uint32_t USART_Baudrate) {
	/* Initialize ESP pins */
	TM_ESP8266_INT_InitPins();
	
	/* Initialize USART */
	TM_USART_Init(ESP8266_USART, ESP8266_USART_PINSPACK, USART_Baudrate);
	TM_USART_Init(ESP8266_DEBUG_USART, ESP8266_DEBUG_USART_PP, USART_Baudrate);
	
	/* Clear buffer for ESP8266 */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Initialize delay */
	TM_DELAY_Init();
	
	/* Reset */
	ESP8266_RESET_SET;

	/* Delay some time for module to be stable */
	Delayms(100);
	
	/* Release reset */
	ESP8266_RESET_RELEASE;
	
	/* Delay some time for module to be stable */
	Delayms(500);
	
	/* Test device if is connected */
	if (TM_ESP8266_INT_Test() != TM_ESP8266_Result_Ok) {
		return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_DeviceError);
	}
	
	/* Check firmware */
	if (TM_ESP8266_INT_CheckFirmware() != TM_ESP8266_Result_Ok) {
		return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_FirmwareNotValid);
	}
	
	/* Delay some time */
	Delayms(100);
	
	/* Reset device */
	if (TM_ESP8266_Reset(ESP8266_Data) != TM_ESP8266_Result_Ok) {
		return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_ResetError);
	}
	
	/* Set to default */
	ESP8266_Data->Connections = 0;
	ESP8266_Data->MultiConnections = 0;
	ESP8266_Data->IncomingBytes[0] = 0;
	ESP8266_Data->Request[0][0] = 0;
	
	/* Return OK */
	return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
}

TM_ESP8266_Result_t TM_ESP8266_Reset(TM_ESP8266_t* ESP8266_Data) {
	uint32_t time;
	
	/* Clear buffer for ESP8266 first */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send reset command */
	ESP8266_USART_SEND(ESP8266_AT_RESET);
	
	/* Long delay here */
	Delayms(1500);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 10) {
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Save new time */
			time = TM_DELAY_Time();
			/* Waiting for "OK" */
			if (TM_ESP8266_INT_StringStartsWith(ESP8266_Buffer, "OK")) {
				break;
			} else {
				
			}
		}
	}
	
	/* Clear buffer for ESP8266 */
	//TM_USART_ClearBuffer(ESP8266_USART);

	/* Return status */
	return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
}

TM_ESP8266_Result_t TM_ESP8266_ServerEnable(TM_ESP8266_t* ESP8266_Data, uint32_t port) {
	uint32_t time;
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Enable CIPMUX */
	sprintf(ESP8266_Buffer, "%s=%d", ESP8266_AT_SET_MUL_CONN, 1);
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 100) {
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for "OK" */
			if (
				strstr(ESP8266_Buffer, "OK") ||
				strstr(ESP8266_Buffer, "no change")
			) {
				/* Multiple connections activated */
				ESP8266_Data->MultiConnections = 1;
				
				break;
			}
		}
	}
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Enable CIPSERVER */
	sprintf(ESP8266_Buffer, "%s=1,%d", ESP8266_AT_SET_SERVER, port);
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		/* 10ms timeout */
		if (TM_DELAY_Time() - time > 100) {
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for "OK" */
			if (
				strncmp(ESP8266_Buffer, "OK", 2) == 0 ||
				strncmp(ESP8266_Buffer, "no change", 9) == 0
			) {
				/* Some delay first */
				Delayms(200);
				
				/* Get IPs */
				TM_ESP8266_GetIPsMACs(ESP8266_Data);
				
				/* Return OK */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			} else if (strncmp(ESP8266_Buffer, "ERROR", 5) == 0) {
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Error);
			}  else if (strstr(ESP8266_Buffer, "busy")) {
				/* Send command again */
				sprintf(ESP8266_Buffer, "%s=1,%d", ESP8266_AT_SET_SERVER, port);
				ESP8266_USART_SEND(ESP8266_Buffer);
			}
		}
	}
}

TM_ESP8266_Result_t TM_ESP8266_WifiMode(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_WifiMode_t WifiMode) {
	uint32_t time;
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send command */
	sprintf(ESP8266_Buffer, "%s=%d", ESP8266_AT_WIFI_MODE, (uint8_t)WifiMode);
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 100) {
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for valid firmware */
			if (
				strstr(ESP8266_Buffer, "no change") ||
				strstr(ESP8266_Buffer, "OK")
			) {
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			}
		}
	}
}

TM_ESP8266_Result_t TM_ESP8266_CheckIncomingData(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Mode_t mode) {
	/* Get new string from buffer */
	while (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
		/* Check data */
		TM_ESP8266_INT_CheckIncomingData(ESP8266_Data, ESP8266_Buffer);
	}
	
	//ESP8266_DEBUG_SEND("no new data_bottom");
	
	/* Return no data */
	return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_NoNewData);
}

TM_ESP8266_Result_t TM_ESP8266_SendLine(TM_ESP8266_t* ESP8266_Data, uint8_t connection_id, char* str) {
	uint32_t time;
	
	/* Check if we are connected */
	//if (!ESP8266_Data->Connected) {
		/* We are not connected */
	//	return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Error);;
	//}
	
	if (!(ESP8266_Data->Connections & (1 << connection_id))) {
		return TM_ESP8266_Result_Error;
	}
	
	/* Send command */
	sprintf(ESP8266_Buffer, "%s=%d,%d", ESP8266_AT_SEND, connection_id, strlen(str) + 2);
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Wait the same response as it was on send */
	//while (!TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer)));
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 2000) {
			/* Disable connection */
			ESP8266_Data->Connections &= ~(1 << connection_id);
			
			/* Return timeout error */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			
			/* Analyze data when full string arrive */
			TM_ESP8266_INT_CheckIncomingData(ESP8266_Data, ESP8266_Buffer);
			
		} else if (TM_USART_FindCharacter(ESP8266_USART, '>')) {
			ESP8266_DEBUG_SEND("> detected\n");
			
			/* Break and continue */
			break;
		}
	}
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send string */
	ESP8266_USART_SEND((char *)str);
	
	/* Wait the same response as it was on send */
/*	Delay(500);
	while (!TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer)));
	Delay(500);*/
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 3000) {
			/* Disable active connection */
			ESP8266_Data->Connections &= ~(1 << connection_id);
			
			/* Return timeout error */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			time = TM_DELAY_Time();
			ESP8266_DEBUG_SEND("SendLine_string detected\n");
			ESP8266_DEBUG_SEND(ESP8266_Buffer);
			/* Waiting for valid response */
			if (
				strstr(ESP8266_Buffer, ESP8266_TEXT_SEND_OK) ||
				strstr(ESP8266_Buffer, ESP8266_TEXT_SEND_READY)
			) {
				ESP8266_DEBUG_SEND("SendLine_SEND_OK\n");
				/* Return */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			} else if (strstr(ESP8266_Buffer, ESP8266_TEXT_BUSY)) {
				/* Send again */
				ESP8266_USART_SEND((char *)str);
				
				/* Increase time */
				time = TM_DELAY_Time();
			}
			
			/* Analyze incoming data */
			TM_ESP8266_INT_CheckIncomingData(ESP8266_Data, ESP8266_Buffer);
		}
	}
}

TM_ESP8266_Result_t TM_ESP8266_CloseConnection(TM_ESP8266_t* ESP8266_Data, uint8_t connection_id) {
	uint32_t time;
	char response[20];

	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	if (!(ESP8266_Data->Connections & (1 << connection_id))) {
		return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Error);
	}
	
	/* Generate string */
	sprintf(ESP8266_Buffer, "%s=%d", ESP8266_AT_CLOSE, connection_id);
	
	/* Send command */
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Format expected result */
	sprintf(response, "%u,CLOSED", connection_id);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 1000) {
			/* Not connected anymore after this command */
			ESP8266_Data->Connections &= ~(1 << connection_id);
			
			/* Return timeout error */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for valid response */
			if (
				strstr(ESP8266_Buffer, response) ||
				strstr(ESP8266_Buffer, ESP8266_TEXT_OK)
			) {
				/* Not connected anymore after this command */
				ESP8266_Data->Connections &= ~(1 << connection_id);
				
				/* Disconnected OK */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			} else if (strncmp(ESP8266_Buffer, "link is not", 11) == 0) {
				/* Not connected anymore after this command */
				ESP8266_Data->Connections &= ~(1 << connection_id);
				
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Error);
			} else {
				/* Check incoming data */
				TM_ESP8266_INT_CheckIncomingData(ESP8266_Data, ESP8266_Buffer);
			}
		}
	}
}

TM_ESP8266_Result_t TM_ESP8266_SetBaudrate(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Baudrate_t baud) {
	uint32_t time;
	
	/* Generate string */
	sprintf(ESP8266_Buffer, "%s=%d", ESP8266_AT_SET_BAUDRATE, (uint32_t)baud);
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send command */
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 10) {
			/* Timeout error */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for valid firmware */
			if (strncmp(ESP8266_Buffer, "OK", 2) == 0) {
				/* Set baudrate */
				ESP8266_Data->Baudrate = (uint32_t)baud;
				
				/* Return OK */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			} else if (strncmp(ESP8266_Buffer, "ERROR", 5) == 0) {
				/* Return ERROR */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Error);
			}
		}
	}
}


TM_ESP8266_Result_t TM_ESP8266_GetIPsMACs(TM_ESP8266_t* ESP8266_Data) {
	uint32_t time;
	uint8_t i, j;
	
	/* Generate string */
	sprintf(ESP8266_Buffer, ESP8266_AT_GETIP);
	
	/* Send command */
	ESP8266_USART_SEND(ESP8266_Buffer);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		/* 100ms */
		if (TM_DELAY_Time() - time > 100) {
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* If first character of string is number, then we have IP address */
			if (TM_ESP8266_INT_StringStartsWith(ESP8266_Buffer, "+CIFSR:APIP")) {
				j = 13;
				for (i = 0; i < 4; i++) {
					ESP8266_Data->APIP[i] = 0;
					while (ESP8266_IS_DIGIT(ESP8266_Buffer[j])) {
						ESP8266_Data->APIP[i] = 10 * ESP8266_Data->APIP[i] + ESP8266_C2N(ESP8266_Buffer[j++]);
					}
					j++;
				}				
			} else if (TM_ESP8266_INT_StringStartsWith(ESP8266_Buffer, "+CIFSR:STAIP")) {
				j = 14;
				for (i = 0; i < 4; i++) {
					ESP8266_Data->STAIP[i] = 0;
					while (ESP8266_IS_DIGIT(ESP8266_Buffer[j])) {
						ESP8266_Data->STAIP[i] = 10 * ESP8266_Data->STAIP[i] + ESP8266_C2N(ESP8266_Buffer[j++]);
					}
					j++;
				}				
			} else if (TM_ESP8266_INT_StringStartsWith(ESP8266_Buffer, "+CIFSR:APMAC")) {
				j = 14;
				for (i = 0; i < 6; i++) {
					ESP8266_Data->APMAC[i] = 16 * TM_ESP866_INT_GetNumFromHex(ESP8266_Buffer[j++]);
					ESP8266_Data->APMAC[i] += TM_ESP866_INT_GetNumFromHex(ESP8266_Buffer[j++]);
					j++;
				}
			} else if (TM_ESP8266_INT_StringStartsWith(ESP8266_Buffer, "+CIFSR:STAMAC")) {
				j = 15;
				for (i = 0; i < 6; i++) {
					ESP8266_Data->STAMAC[i] = 16 * TM_ESP866_INT_GetNumFromHex(ESP8266_Buffer[j++]);
					ESP8266_Data->STAMAC[i] += TM_ESP866_INT_GetNumFromHex(ESP8266_Buffer[j++]);
					j++;
				}
			} else if (TM_ESP8266_INT_StringStartsWith(ESP8266_Buffer, "OK")) {
				/* Return OK */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			}
		}
	}
}

TM_ESP8266_Result_t TM_ESP8266_SetIP_MAC(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Set_IP_MAC_t type, uint8_t* data) {
	char buffer[50];
	switch (type) {
		case TM_ESP8266_Set_AP_IP:
			sprintf(buffer, "AT+CIPAP=\"%u,%u,%u,%u\"", data[0], data[1], data[2], data[3]);
			break;
		case TM_ESP8266_Set_STA_IP:
			sprintf(buffer, "AT+CIPSTA=\"%u,%u,%u,%u\"", data[0], data[1], data[2], data[3]);
			break;
		case TM_ESP8266_Set_AP_MAC:
			sprintf(buffer, "AT+CIPAPMAC=\"%02x:%02x:%02x:%02x:%02x:%02x\"", data[0], data[1], data[2], data[3], data[4], data[5]);
			break;
		case TM_ESP8266_Set_STA_MAC:
			sprintf(buffer, "AT+CIPSTAMAC=\"%02x:%02x:%02x:%02x:%02x:%02x\"", data[0], data[1], data[2], data[3], data[4], data[5]);
			break;
		default:
			/* Return error */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Error);
	}
	
	/* Clear receive buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send to device */
	ESP8266_USART_SEND(buffer);
	
	/* Return OK */
	return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
}

TM_ESP8266_Result_t TM_ESP8266_ClientAvailable(TM_ESP8266_t* ESP8266_Data) {
	char buf[100];
	if (ESP8266_Data->Connections != 0) {
		
		sprintf(buf, "Connections_Available: %02X\n", ESP8266_Data->Connections);
		ESP8266_DEBUG_SEND(buf);
		
		
		return TM_ESP8266_Result_NewClientConnected;
	}
	
	return TM_ESP8266_Result_NoNewData;
}

TM_ESP8266_Result_t TM_ESP8266_DisableEcho(TM_ESP8266_t* ESP8266_Data) {
	/* Disable echo, wait for OK, 10ms timeout */
	return TM_ESP8266_CustomCommand(ESP8266_Data, ESP8266_AT_ECHO0, ESP8266_TEXT_OK, 10);
}

TM_ESP8266_Result_t TM_ESP8266_EnableEcho(TM_ESP8266_t* ESP8266_Data) {
	/* Enable echo, wait for OK, 10ms timeout */
	return TM_ESP8266_CustomCommand(ESP8266_Data, ESP8266_AT_ECHO1, ESP8266_TEXT_OK, 10);
}

TM_ESP8266_Result_t TM_ESP8266_CustomCommand(TM_ESP8266_t* ESP8266_Data, char* command, char* response, uint16_t timeout) {
	uint32_t time;
	/* Send command */
	ESP8266_USART_SEND(command);
	
	/* Wait response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > timeout) {
			/* Timeout error */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_TimeoutError);
		}
		
		/* String arrived */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
		
			/* Analyze it */
			TM_ESP8266_INT_CheckIncomingData(ESP8266_Data, ESP8266_Buffer);
			
			/* If we have valid response */
			if (strstr(ESP8266_Buffer, response)) {
				/* Return ok */
				return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_Ok);
			}
		}
	}
}


/* Private */
void TM_ESP8266_INT_InitPins(void) {
	/* Init reset pin */
	TM_GPIO_Init(ESP8266_RESET_PORT, ESP8266_RESET_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
}

TM_ESP8266_Result_t TM_ESP8266_INT_Test(void) {
	uint32_t time;
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send AT command */
	ESP8266_USART_SEND(ESP8266_AT);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 1000) {
			return TM_ESP8266_Result_TimeoutError;
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for "OK" */
			if (strncmp(ESP8266_Buffer, "OK", 2) == 0) {
				break;
			}
		}
	}
	
	/* Return OK */
	return TM_ESP8266_Result_Ok;
}

TM_ESP8266_Result_t TM_ESP8266_INT_CheckFirmware(void) {
	uint32_t time;
	
	/* Clear buffer */
	//TM_USART_ClearBuffer(ESP8266_USART);
	
	/* Send AT command */
	ESP8266_USART_SEND(ESP8266_AT_FIRMWARE);
	
	/* Wait for response */
	time = TM_DELAY_Time();
	while (1) {
		if (TM_DELAY_Time() - time > 10) {
			return TM_ESP8266_Result_TimeoutError;
		}
		/* Get string */
		/* If more than 0 characters received */
		if (TM_USART_Gets(ESP8266_USART, ESP8266_Buffer, sizeof(ESP8266_Buffer))) {
			/* Waiting for valid firmware */
			if (strstr(ESP8266_Buffer, ESP8266_FIRMWARE)) {
				return TM_ESP8266_Result_Ok;
			} else if (strncmp(ESP8266_Buffer, "OK", 2) == 0) {
				return TM_ESP8266_Result_Error;
			}
		}
	}
}

TM_ESP8266_Result_t TM_ESP8266_INT_CheckIncomingData(TM_ESP8266_t* ESP8266_Data, char* buffer) {	
	uint8_t i = 0;
	static uint8_t conn, in_ipd_mode = 0;
	
	/* Send to debug port */
	ESP8266_DEBUG_SEND(buffer);
	
	if (buffer[0] == '\r') {

	} 
	if (TM_ESP8266_INT_StringStartsWith(buffer, "0,CONNECT")) {
		if (in_ipd_mode && conn == 0) {
			in_ipd_mode = 0;
		}
		ESP8266_DEBUG_SEND("0 connected\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "1,CONNECT")) {
		if (in_ipd_mode && conn == 1) {
			in_ipd_mode = 0;
		}
		ESP8266_DEBUG_SEND("1 connected\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "2,CONNECT")) {
		if (in_ipd_mode && conn == 2) {
			in_ipd_mode = 0;
		}
		ESP8266_DEBUG_SEND("2 connected\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "3,CONNECT")) {
		if (in_ipd_mode && conn == 3) {
			in_ipd_mode = 0;
		}
		ESP8266_DEBUG_SEND("3 connected\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "4,CONNECT")) {
		if (in_ipd_mode && conn == 4) {
			in_ipd_mode = 0;
		}
		ESP8266_DEBUG_SEND("4 connected\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "0,CLOSED")) {
		ESP8266_DEBUG_SEND("0 disconnect\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "1,CLOSED")) {
		ESP8266_Data->Connections &= 0xFD;
		ESP8266_DEBUG_SEND("1 disconnect\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "2,CLOSED")) {
		ESP8266_Data->Connections &= 0xFB;
		ESP8266_DEBUG_SEND("2 disconnect\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "3,CLOSED")) {
		ESP8266_Data->Connections &= 0xF7;
		ESP8266_DEBUG_SEND("3 disconnect\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "4,CLOSED")) {
		ESP8266_Data->Connections &= 0xEF;
		ESP8266_DEBUG_SEND("4 disconnect\n");
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, ESP8266_AT_RECEIVED_DATA)) { /* +IPD */
		/* We are in +IPD mode */
		in_ipd_mode = 1;
					
		/* Connection ID for which are data */
		conn = ESP8266_C2N(buffer[5]);
		
		/* Start location */
		i = 7;
		
		/* Get incoming bytes count for specific request number */
		incomingbytes = 0;
		while (ESP8266_IS_DIGIT(buffer[i])) {
			incomingbytes = 10 * incomingbytes + ESP8266_C2N(buffer[i++]);
		}
		
		/* Save incoming bytes */
		ESP8266_Data->IncomingBytes[conn] = incomingbytes;
		
		/* Save incoming data */
		sprintf(ESP8266_Data->Request[conn], "%s", &buffer[i]);
		
		/* No new data */
		ESP8266_DEBUG_SEND("no new data\n");
		return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_NoNewData);
	} else if (TM_ESP8266_INT_StringStartsWith(buffer, "OK")) {
		ESP8266_DEBUG_SEND("OK Received\n");
		/* If we are in ipd_mode */
		if (in_ipd_mode) {
			/* Not anymore */
			in_ipd_mode = 0;
			
			ESP8266_DEBUG_SEND("new client connected, not in IPD anymore\n");
			
			/* New connection active and ready */
			ESP8266_Data->Connections |= 1 << conn;
			
			sprintf(buffer, "connection ready: %d\n", conn);
			ESP8266_DEBUG_SEND(buffer);
			
			
			
			/* We have new data for now */
			return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_NewClientConnected);
		}
	} else { /* If unknown data received, filter it */
		if (in_ipd_mode && buffer[0] != '\r') { /* All request data */
			/* If we have still some space for new data */
			if ((strlen(ESP8266_Data->Request[conn]) + strlen(buffer)) < ESP8266_REQUEST_BUFFER_LEN) {
				/* Append request */
				strcat(ESP8266_Data->Request[conn], buffer);
			}
			
			ESP8266_DEBUG_SEND("IN IPD MODE\n");
		} else { /* Others */
		
		}
	}
	
	/* No new data */
	return TM_ESP8266_INT_ReturnWithStatus(ESP8266_Data, TM_ESP8266_Result_NoNewData);
}

uint8_t TM_ESP8266_INT_StringStartsWith(char* string, const char* str) {
	while (*str) {
		if (*str != *string) {
			return 0;
		}
		str++;
		string++;
	}
	return 1;
}

TM_ESP8266_Result_t TM_ESP8266_INT_ReturnWithStatus(TM_ESP8266_t* ESP8266_Data, TM_ESP8266_Result_t status) {
	/* Set new status */
	ESP8266_Data->Status = status;
	/* Return status */
	return status;
}

uint8_t TM_ESP866_INT_GetNumFromHex(char c) {
	if (c >= 'A' && c <= 'F') {
		return (c - 'A') + 10;
	}
	if (c >= 'a' && c <= 'f') {
		return (c - 'a') + 10;
	}
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	
	return 0;
}
