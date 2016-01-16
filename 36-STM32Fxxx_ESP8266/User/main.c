/**
 * Keil project example for ESP8266 module
 *
 * @note      Check defines.h file for configuration settings!
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
#include "tm_stm32_usart.h"
#include "tm_stm32_esp8266.h"

/* ESP8266 working structure */
TM_ESP8266_t ESP8266;
TM_ESP8266_APConfig_t ESP8266_ConfigAP;

uint8_t list_AP = 0;
uint8_t request_conn = 0;

int main(void) {
	/* Init system */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init delay */
	TM_DELAY_Init();
	
	/* Init debug USART */
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 921600);
	
	/* Display message */
	printf("ESP8266 AT commands parser\r\n");
	
	/* Init ESP module */
	while (TM_ESP8266_Init(&ESP8266) != TM_ESP8266_Result_Ok) {
		printf("Problems with reseting module!\r\n");
	}
	
	/* Set mode to STA+AP */
	while (TM_ESP8266_SetMode(&ESP8266, TM_ESP8266_Mode_STA_AP) != TM_ESP8266_Result_Ok);
	
	/* Enable multiple connections */
	while (TM_ESP8266_ServerEnable(&ESP8266, 80) != TM_ESP8266_Result_Ok);
	
	/* Module is connected OK */
	printf("Initialization finished!\r\n");
	
	/* Disconnect from wifi if connected */
	TM_ESP8266_WifiDisconnect(&ESP8266);
	
	/* Wait till finish */
	TM_ESP8266_WaitReady(&ESP8266);
	
	/* Fill structure for server */
	ESP8266.AP.Pass[0] = 0;                      /*!< No password */
	ESP8266.AP.Ecn = TM_ESP8266_Ecn_OPEN;        /*!< Use OPEN network */
	strcpy(ESP8266.AP.SSID, "#MY FIRST ESP AP"); /*!< Format name for SSID */
	
	/* Set AP */
	TM_ESP8266_SetAP(&ESP8266, &ESP8266.AP);
	
	/* Wait till finish */
	TM_ESP8266_WaitReady(&ESP8266);
	
	/* Get a list of all stations  */
	TM_ESP8266_ListWifiStations(&ESP8266);
	
	/* Wait till finishes */
	TM_ESP8266_WaitReady(&ESP8266);
	
	/* Connect to wifi and save settings */
	TM_ESP8266_WifiConnect(&ESP8266, "Your SSID", "your password");
	
	/* Wait till finish */
	TM_ESP8266_WaitReady(&ESP8266);
	
	/* Get connected devices */
	TM_ESP8266_WifiGetConnected(&ESP8266);
	
	/* Start connections */
	request_conn = 1;
	
	while (1) {
		/* Update ESP module */
		TM_ESP8266_Update(&ESP8266);
		
		/* Check for button */
		if (TM_DISCO_ButtonOnPressed()) {
			/* Starting with connection to web */
			while (TM_ESP8266_StartClientConnection(&ESP8266, "stm32f4_discovery", "stm32f4-discovery.com", 80, NULL));
			
			/* Disconnect from Wi-Fi */
			//TM_ESP8266_WifiDisconnect(&ESP8266);
			
			/* Factory reset */
			//TM_ESP8266_RestoreDefault(&ESP8266);
			
			/* Ping domain or IP */
			//TM_ESP8266_Ping(&ESP8266, "stm32f4-discovery.com");
		}
		
		/* Toggle LEDS */
		TM_DISCO_LedToggle(LED_ALL);
	}
}

/* 1ms handler */
void TM_DELAY_1msHandler() {
	
}

/* printf handler */
int fputc(int ch, FILE* fil) {
	/* Send over debug USART */
	TM_USART_Putc(USART2, ch);
	
	/* Return OK */
	return ch;
}

/* Called on initialization process */
void TM_ESP8266_Callback_Init(TM_ESP8266_t* ESP8266, TM_ESP8266_Init_t* Init) {
	/* Set USART data */
	Init->USART = USART1;
	Init->USART_Baudrate = 115200;
#ifndef STM32F4_DISCOVERY
	Init->USART_PinsPack = TM_USART_PinsPack_1;
#else
	Init->USART_PinsPack = TM_USART_PinsPack_2;
#endif
	
	/* Set buffer size */
	Init->Buffer = NULL;          /*!< If set to NULL, HEAP will be used for allocation */
	Init->BufferSize = 1024 * 16; /*!< Set buffer size */
	
	/* Set reset pin */
	Init->RESET_PORT = GPIOA;
	Init->RESET_PIN = GPIO_Pin_0;
}

/* Called when we are disconnected from WIFI */
void TM_ESP8266_Callback_WifiDisconnected(TM_ESP8266_t* ESP8266) {
	printf("Wifi is disconnected!\r\n");
}

void TM_ESP8266_Callback_WifiConnected(TM_ESP8266_t* ESP8266) {
	printf("Wifi is connected!\r\n");
}

void TM_ESP8266_Callback_WifiConnectFailed(TM_ESP8266_t* ESP8266) {
	printf("Connection to wifi network has failed. Reason %d\r\n", ESP8266->WifiConnectError);
}

void TM_ESP8266_Callback_WifiGotIP(TM_ESP8266_t* ESP8266) {
	printf("Wifi got an IP address\r\n");
	
	/* Read that IP from module */
	printf("Grabbing IP status: %d\r\n", TM_ESP8266_GetSTAIP(ESP8266));
}

void TM_ESP8266_Callback_WifiIPSet(TM_ESP8266_t* ESP8266) {
	/* We have STA IP set (IP set by router we are connected to) */
	printf("We have valid IP address: %d.%d.%d.%d\r\n", ESP8266->STAIP[0], ESP8266->STAIP[1], ESP8266->STAIP[2], ESP8266->STAIP[3]);
}

void TM_ESP8266_Callback_DHCPTimeout(TM_ESP8266_t* ESP8266) {
	printf("DHCP timeout!\r\n");
}

void TM_ESP8266_Callback_WifiDetected(TM_ESP8266_t* ESP8266, TM_ESP8266_APs_t* ESP8266_AP) {
	uint8_t i = 0;
	
	/* Print number of detected stations */
	printf("We have detected %d AP stations\r\n", ESP8266_AP->Count);
	
	/* Print each AP */
	for (i = 0; i < ESP8266_AP->Count; i++) {
		/* Print SSID for each AP */
		printf("%2d: %s\r\n", i, ESP8266_AP->AP[i].SSID);
	}
}

/************************************/
/*          SERVER CALLBACKS        */
/************************************/
void TM_ESP8266_Callback_ServerConnectionActive(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	static uint32_t i = 1;
	printf("SERVER: Connection %d client connected. %5d\r\n", Connection->Number, i++);
}

void TM_ESP8266_Callback_ServerConnectionClosed(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	static uint32_t i = 1;
	printf("SERVER: Connection %d connection closed. %5d\r\n", Connection->Number, i++);
}

void TM_ESP8266_Callback_ServerConnectionDataReceived(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer) {
	/* Someone has sent data to server */
	printf("SERVER: Connection %d received %d bytes of data\r\n", Connection->Number, Connection->BytesReceived);
	
	/* If user requested /favicon.ico file, ignore it and close connection */
	if (strstr(Buffer, "GET / ") == NULL && strstr(Buffer, "POST / ") == NULL) {
		/* Close connection */	
		printf("SERVER: Connection %d closing request: %d\r\n", Connection->Number, (uint8_t)TM_ESP8266_CloseConnection(ESP8266, Connection));
		
		return;
	}
	
	/* Request data sent */
	printf("SERVER: Connection %d requests data sent. Status: %d\r\n", Connection->Number, (uint8_t)TM_ESP8266_RequestSendData(ESP8266, Connection));
}

/* Called when we are ready to send data, just we must get them from user */
uint16_t TM_ESP8266_Callback_ServerConnectionSendData(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
	/* Send data back! */
	/* Format headers */
	sprintf(Buffer, "HTTP/1.1 200 OK\r\n");
	strcat(Buffer, "Content-Type: text/html\r\n");
	strcat(Buffer, "Connection: close\r\n");
	strcat(Buffer, "\r\n");
	
	/* Format visible text on browser */
	strcat(Buffer, "Welcome to web browser based on ESP8266 and STM32F4xx.");
	
	/* Return length of buffer */
	return strlen(Buffer);
}

/* Called when data were successfully sent as server */
void TM_ESP8266_Callback_ServerConnectionDataSent(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	/* Data were sent on connection, let's close connection */
	printf("SERVER: Connection %d data were sent. Closing connection, status: %d\r\n", Connection->Number, (uint8_t)TM_ESP8266_CloseConnection(ESP8266, Connection));
}

void TM_ESP8266_Callback_ServerConnectionDataSentError(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	/* Data were not sent on connection, let's still close connection */	
	printf("SERVER: Connection %d data were NOT sent. Closing connection, status: %d\r\n", Connection->Number, (uint8_t)TM_ESP8266_CloseConnection(ESP8266, Connection));
}

/************************************/
/*         CLIENT CALLBACKS         */
/************************************/
void TM_ESP8266_Callback_ClientConnectionConnected(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	/* We are connected to external server */
	printf("Client connected to server! Connection number: %s\r\n", Connection->Name);
	
	/* We are connected to server, request to sent header data to server */
	TM_ESP8266_RequestSendData(ESP8266, Connection);
}

/* Called when client connection fails to server */
void TM_ESP8266_Callback_ClientConnectionError(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	/* Fail with connection to server */
	printf("An error occured when trying to connect on connection: %d\r\n", Connection->Number);
}

/* Called when data are ready to be sent to server */
uint16_t TM_ESP8266_Callback_ClientConnectionSendData(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
	/* Format data to sent to server */
	sprintf(Buffer, "GET /wp-content/uploads/Parsed_files.rar HTTP/1.1\r\n");
	strcat(Buffer, "Host: stm32f4-discovery.com\r\n");
	strcat(Buffer, "Connection: close\r\n");
	strcat(Buffer, "\r\n");
	
	/* Return length of buffer */
	return strlen(Buffer);
}

/* Called when data are send successfully */
void TM_ESP8266_Callback_ClientConnectionDataSent(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	printf("Data successfully sent as client!\r\n");
}

/* Called when error returned trying to sent data */
void TM_ESP8266_Callback_ClientConnectionDataSentError(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	printf("Error while sending data on connection %d!\r\n", Connection->Number);
}

uint32_t time = 0;
void TM_ESP8266_Callback_ClientConnectionDataReceived(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer) {
	/* Data received from server back to client */
	printf("Data received from server on connection: %s; Number of bytes received: %d; %d / %d; %5.2f%%\r\n",
		Connection->Name,
		Connection->BytesReceived,
		Connection->TotalBytesReceived,
		Connection->ContentLength,
		((float)Connection->TotalBytesReceived / (float)Connection->ContentLength) * (float)100
	);
	
	/* Print message when first packet */
	if (Connection->FirstPacket) {
		/* Start counting time */
		time = TM_DELAY_Time();
		
		/* Print first message */
		printf("This is first packet received. Content length on this connection is: %d\r\n", Connection->ContentLength);
	}
}

/* Called when connection is closed */
void TM_ESP8266_Callback_ClientConnectionClosed(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	printf("Client connection closed, connection: %d; Total bytes received: %d; Content-Length header: %d\r\n",
		Connection->Number, Connection->TotalBytesReceived, Connection->ContentLength
	);
	
	/* Calculate time */
	time = TM_DELAY_Time() - time;
	
	/* Print time we need to get data back from server */
	printf("Time for data: %u ms; speed: %d kb/s\r\n", time, Connection->TotalBytesReceived / time);
}

/* Called when timeout is reached on connection to server */
void TM_ESP8266_Callback_ClientConnectionTimeout(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection) {
	printf("Timeout reached on connection: %d\r\n", Connection->Number);
}

/* Pinging started */
void TM_ESP8266_Callback_PingStarted(TM_ESP8266_t* ESP8266, char* address) {
	printf("Pinging started to address: %s\r\n", address);
}

/* Ping was successful */
void TM_ESP8266_Callback_PingFinished(TM_ESP8266_t* ESP8266, TM_ESP8266_Ping_t* PING) {
	if (PING->Success) {	
		printf("Successfully pinged %s in %d milliseconds\r\n", PING->Address, PING->Time);
	} else {
		printf("Error on pinging %s\r\n", PING->Address);
	}
}

/* Called on status messages for network firmware update */
void TM_ESP8266_Callback_FirmwareUpdateStatus(TM_ESP8266_t* ESP8266, TM_ESP8266_FirmwareUpdate_t status) {
	printf("CIP update status: %d\r\n", status);
}

/* Called when firmware network update was successful */
void TM_ESP8266_Callback_FirmwareUpdateSuccess(TM_ESP8266_t* ESP8266) {
	printf("CIP Update OK!\r\n");
}

/* Called when firmware network error occurred */
void TM_ESP8266_Callback_FirmwareUpdateError(TM_ESP8266_t* ESP8266) {
	printf("CIP Update error!\r\n");
}
