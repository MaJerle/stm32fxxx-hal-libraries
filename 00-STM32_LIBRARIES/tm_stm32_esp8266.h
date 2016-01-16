/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Library for ESP8266 module using AT commands on STM32F4xx devices
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_ESP8266_H
#define TM_ESP8266_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_ESP8266
 * @brief    Library for ESP8266 module using AT commands on STM32F4xx devices
 * @{
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM USART
 - TM BUFFER
 - TM GPIO
 - TM DELAY
 - stdio.h
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_buffer.h"
#include "tm_stm32_gpio.h"
#include "tm_stm32_delay.h"
#include "stdio.h"

/**
 * @defgroup TM_ESP8266_Macros
 * @brief    Library defines
 * @{
 */

#define ESP8266_MAX_CONNECTIONS        5  /*!< Number of maximum active connections on ESP */
#define ESP8266_MAX_AP                 10 /*!< Number of AP stations saved to received data array */

/**
 * @}
 */
 
/**
 * @defgroup TM_ESP8266_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  ESP8266 library possible return statements on function calls
 */
typedef enum {
	TM_ESP8266_Result_Ok = 0x00,          /*!< Everything is OK */
	TM_ESP8266_Result_Error,              /*!< An error occurred */
	TM_ESP8266_Result_DeviceNotConnected, /*!< Device is not connected to UART */
	TM_ESP8266_Result_Timeout,            /*!< Timeout was detected when sending command to ESP module */
	TM_ESP8266_Result_LinkNotValid,       /*!< Link for connection is not valid */
	TM_ESP8266_Result_NoAvailableHeap,    /*!< Heap memory is not available */
	TM_ESP8266_Result_WifiNotConnected,   /*!< Wifi is not connected to network */
	TM_ESP8266_Result_Busy                /*!< Device is busy, new command is not possible */
} TM_ESP8266_Result_t;

/**
 * @brief  ESP modes of operation enumeration
 */
typedef enum {
	TM_ESP8266_Mode_STA = 0x01,   /*!< ESP in station mode */
	TM_ESP8266_Mode_AP = 0x02,    /*!< ESP as software Access Point mode */
	TM_ESP8266_Mode_STA_AP = 0x03 /*!< ESP in both modes */
} TM_ESP8266_Mode_t;

/**
 * @brief  Security settings for wifi network
 */
typedef enum {
	TM_ESP8266_Ecn_OPEN = 0x00,         /*!< Wifi is open */
	TM_ESP8266_Ecn_WEP = 0x01,          /*!< Wired Equivalent Privacy option for wifi security. @note This mode can't be used when setting up ESP8266 wifi */
	TM_ESP8266_Ecn_WPA_PSK = 0x02,      /*!< Wi-Fi Protected Access */
	TM_ESP8266_Ecn_WPA2_PSK = 0x03,     /*!< Wi-Fi Protected Access 2 */
	TM_ESP8266_Ecn_WPA_WPA2_PSK = 0x04, /*!< Wi-Fi Protected Access with both modes */
} TM_ESP8266_Ecn_t;

/**
 * @brief  IPD network data structure
 */
typedef struct {
	uint8_t InIPD;        /*!< Set to 1 when ESP is in IPD mode with data */
	uint16_t InPtr;       /*!< Input pointer to save data to buffer */
	uint8_t ConnNumber;   /*!< Connection number where IPD is active */
	uint8_t USART_Buffer; /*!< Set to 1 when data are read from USART buffer or 0 if from temporary buffer */
} TM_ESP8266_IPD_t;

/**
 * @brief  Connection structure
 */
typedef struct {
	uint8_t Active;              /*!< Status if connection is active */
	uint8_t Number;              /*!< Connection number */
	uint8_t Client;              /*!< Set to 1 if connection was made as client */
	uint16_t RemotePort;         /*!< Remote PORT number */
	uint8_t RemoteIP[4];         /*!< IP address of device */
	uint32_t BytesReceived;      /*!< Number of bytes received in current data packet */
	uint32_t TotalBytesReceived; /*!< Number of bytes received in entire connection lifecycle */
	uint8_t WaitForWrapper;      /*!< Status flag, to wait for ">" wrapper on data sent */
	uint8_t WaitingSentRespond;  /*!< Set to 1 when we have sent data and we are waiting respond */
	char Data[5842];             /*!< Data array */
	uint8_t CallDataReceived;    /*!< Set to 1 when we are waiting for commands to be inactive before we call callback function */
	uint32_t ContentLength;      /*!< Value of "Content-Length" header if it exists in +IPD data packet */
	char Name[64];               /*!< Connection name, useful when using as client */
	void* UserParameters;        /*!< User parameters pointer. Useful when user wants to pass custom data which can later be used in callbacks */
	uint8_t HeadersDone;         /*!< User option flag to set when headers has been found in response */
	uint8_t FirstPacket;         /*!< Set to 1 when if first packet in connection received */
} TM_ESP8266_Connection_t;

/**
 * @brief  Connected network structure
 */
typedef struct {
	char SSID[64];   /*!< SSID network name */
	uint8_t MAC[6];  /*!< MAC address of network */
	uint8_t Channel; /*!< Network channel */
	int16_t RSSI;    /*!< Signal strength */
} TM_ESP8266_ConnectedWifi_t;

/**
 * @brief  AP station structure to use when searching for network
 */
typedef struct {
	uint8_t Ecn;         /*!< Security of Wi-Fi spot. This parameter has a value of @ref TM_ESP8266_Ecn_t enumeration */
	char SSID[64];       /*!< Service Set Identifier value. Wi-Fi spot name */
	int16_t RSSI;        /*!< Signal strength of Wi-Fi spot */
	uint8_t MAC[6];      /*!< MAC address of spot */
	uint8_t Channel;     /*!< Wi-Fi channel */
	uint8_t Offset;      /*!< Frequency offset from base 2.4GHz in kHz */
	uint8_t Calibration; /*!< Frequency offset calibration */
} TM_ESP8266_AP_t;

/**
 * @brief  List of AP stations found on network search
 */
typedef struct {
	TM_ESP8266_AP_t AP[ESP8266_MAX_AP]; /*!< Each AP point data */
	uint8_t Count;                      /*!< Number of valid AP stations */
} TM_ESP8266_APs_t;

/**
 * @brief  Access point configuration
 */
typedef struct {
	char SSID[65];          /*!< Network public name for ESP AP mode */
	char Pass[65];          /*!< Network password for ESP AP mode */
	TM_ESP8266_Ecn_t Ecn;   /*!< Security of Wi-Fi spot. This parameter can be a value of @ref TM_ESP8266_Ecn_t enumeration */
	uint8_t Channel;        /*!< Channel Wi-Fi is operating at */
	uint8_t MaxConnections; /*!< Max number of stations that are allowed to connect to ESP AP, between 1 and 4 */
	uint8_t Hidden;         /*!< Set to 1 if network is hidden (not broadcast) or zero if noz */
} TM_ESP8266_APConfig_t;

/**
 * @brief  Ping structure
 */
typedef struct {
	char Address[64]; /*!< Domain or IP to ping */
	uint32_t Time;    /*!< Time in milliseconds needed for pinging */
	uint8_t Success;  /*!< Status indicates if ping was successful */
} TM_ESP8266_Ping_t;

/**
 * @brief  Wifi connection error enumeration
 */
typedef enum {
	TM_ESP8266_WifiConnectError_Timeout = 0x01,       /*!< Connection timeout */
	TM_ESP8266_WifiConnectError_WrongPassword = 0x02, /*!< Wrong password for AP */
	TM_ESP8266_WifiConnectError_APNotFound = 0x03,    /*!< AP was not found */
	TM_ESP8266_WifiConnectError_Fail = 0x04           /*!< Connection failed with unknown cause */
} TM_ESP8266_WifiConnectError_t;

/**
 * @brief  Firmware update statuses
 */
typedef enum {
	TM_ESP8266_FirmwareUpdate_ServerFound = 0x01, /*!< Server for update has been found */
	TM_ESP8266_FirmwareUpdate_Connected = 0x02,   /*!< We are connected to server for firmware */
	TM_ESP8266_FirmwareUpdate_GotEdition = 0x03,  /*!< We have firmware edition to download */
	TM_ESP8266_FirmwareUpdate_StartUpdate = 0x04, /*!< Update has started */
} TM_ESP8266_FirmwareUpdate_t;

/**
 * @brief  Sleep mode enumeration
 */
typedef enum {
	TM_ESP8266_SleepMode_Disable = 0x00, /*!< Sleep mode disabled */
	TM_ESP8266_SleepMode_Light = 0x01,   /*!< Light sleep mode */
	TM_ESP8266_SleepMode_Modem = 0x02    /*!< Model sleep mode */
} TM_ESP8266_SleepMode_t;

/**
 * @brief  Main ESP8266 working structure
 */
typedef struct {
	uint32_t Baudrate;                                           /*!< Currently used baudrate for ESP module */
	uint32_t ActiveCommand;                                      /*!< Currently active AT command for module */
	char ActiveCommandResponse[5][64];                           /*!< List of responses we expect with AT command */
	uint32_t StartTime;                                          /*!< Time when command was sent */
	uint32_t Timeout;                                            /*!< Timeout in milliseconds for command to return response */
	TM_ESP8266_Connection_t Connection[ESP8266_MAX_CONNECTIONS]; /*!< Array of connections */
	uint8_t STAIP[4];                                            /*!< Assigned IP address for station for ESP module */
	uint8_t STAGateway[4];                                       /*!< Gateway address for station ESP is using */
	uint8_t STANetmask[4];                                       /*!< Netmask address for station ESP is using */
	uint8_t STAMAC[6];                                           /*!< MAC address for station of ESP module */
	uint8_t APIP[4];                                             /*!< Assigned IP address for softAP for ESP module */
	uint8_t APGateway[4];                                        /*!< Gateway address ESP for softAP is using */
	uint8_t APNetmask[4];                                        /*!< Netmask address ESP for softAP is using */
	uint8_t APMAC[6];                                            /*!< MAC address for softAP of ESP module */
	TM_ESP8266_Mode_t SentMode;                                  /*!< AP/STA mode we sent to module. This parameter can be a value of @ref TM_ESP8266_Mode_t enumeration */
	TM_ESP8266_Mode_t Mode;                                      /*!< AT/STA mode which is currently active. This parameter can be a value of @ref TM_ESP8266_Mode_t enumeration */
	TM_ESP8266_APConfig_t AP;                                    /*!< Configuration settings for ESP when using as Access point mode */
	TM_ESP8266_IPD_t IPD;                                        /*!< IPD status strucutre. Used when new data are available from module */
	TM_ESP8266_Ping_t PING;                                      /*!< Pinging settings */
	TM_ESP8266_ConnectedWifi_t ConnectedWifi;                    /*!< Informations about currently connected wifi network */
	TM_ESP8266_WifiConnectError_t WifiConnectError;              /*!< Error code for connection to wifi network. This parameter can be a value of @ref TM_ESP8266_WifiConnectError_t enumeration */
	int8_t StartConnectionSent;                                  /*!< Connection number which has active CIPSTART command and waits response */
	union {
		struct {
			uint8_t STAIPIsSet:1;                                /*!< IP is set */
			uint8_t STANetmaskIsSet:1;                           /*!< Netmask address is set */
			uint8_t STAGatewayIsSet:1;                           /*!< Gateway address is set */
			uint8_t STAMACIsSet:1;                               /*!< MAC address is set */
			uint8_t APIPIsSet:1;                                 /*!< IP is set */
			uint8_t APNetmaskIsSet:1;                            /*!< Netmask address is set */
			uint8_t APGatewayIsSet:1;                            /*!< Gateway address is set */
			uint8_t APMACIsSet:1;                                /*!< MAC address is set */
			uint8_t WaitForWrapper:1;                            /*!< We are waiting for wrapper */
			uint8_t LastOperationStatus:1;                       /*!< Last operations status was OK */
			uint8_t WifiConnected:1;                             /*!< Wifi is connected to network */
			uint8_t WifiGotIP:1;                                 /*!< Wifi got IP address from network */
		} F;
		uint32_t Value;
	} Flags;
	TM_ESP8266_Result_t Result;                                  /*!< Result status as returned from last function call. This parameter can be a value of @ref TM_ESP8266_Result_t enumeration */
} TM_ESP8266_t;

/**
 * @brief  ESP8266 initialization structure
 */
typedef struct {
	USART_TypeDef* USART;               /*!< Pointer to USART peripheral to use with ESP8266 module */
	TM_USART_PinsPack_t USART_PinsPack; /*!< Pinspack for USART peripheral to use with ESP8266 module */
	uint32_t USART_Baudrate;            /*!< Baudrate for ESP8266 communication */
	
	GPIO_TypeDef* RESET_PORT;           /*!< GPIO port for RESET. Set to null to disable RESET pin functionality */
	uint16_t RESET_PIN;                 /*!< GPIO pin for RESET. Not used, if @arg RESET_PORT is set to null */
	
	uint8_t* Buffer;                    /*!< Pointer to temporary buffer for data retrieved from ESP module. Set to null if you want to use HEAP for allocation */
	uint16_t BufferSize;                /*!< Size of temporary buffer in units of bytes */
} TM_ESP8266_Init_t;

/**
 * @}
 */

/**
 * @defgroup TM_ESP8266_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief      Initializes ESP8266 module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_Init(TM_ESP8266_t* ESP8266);

/**
 * @brief      Deinitializes ESP8266 module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_DeInit(TM_ESP8266_t* ESP8266);

/**
 * @brief      Waits for ESP8266 to be ready to accept new command
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_WaitReady(TM_ESP8266_t* ESP8266);

/**
 * @brief      Checks if ESP module can accept new AT command
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_IsReady(TM_ESP8266_t* ESP8266);

/**
 * @brief      Update function which does entire work.
 * @note       This function must be called periodically inside main loop to process all events
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_Update(TM_ESP8266_t* ESP8266);

/**
 * @brief      Restores default values from ESP8266 module flash memory
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_RestoreDefault(TM_ESP8266_t* ESP8266);

/**
 * @brief      Starts firmware module update over the air (OTA)
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_FirmwareUpdate(TM_ESP8266_t* ESP8266);

/**
 * @brief      Sets baudrate for ESP8266 module
 * @note       Module has some issues on returning OK to this command so I don't recommend UART change
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      baudrate: Baudrate to use with module
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetUART(TM_ESP8266_t* ESP8266, uint32_t baudrate);

/**
 * @brief      Sets baudrate for ESP8266 module and stores it to ESP flash for future use
 * @note       Module has some issues on returning OK to this command so I don't recommend UART change
 *             if you really want to change it, use this function and later reconfigure your program to start with changed UART for ESP USART BAUDRATE
 *             
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      baudrate: Baudrate to use with module
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetUARTDefault(TM_ESP8266_t* ESP8266, uint32_t baudrate);

/**
 * @brief      Sets sleep mode for ESP8266 module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      SleepMode: Sleep mode type. This parameter can be a value of @ref TM_ESP8266_SleepMode_t enumeration
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetSleepMode(TM_ESP8266_t* ESP8266, TM_ESP8266_SleepMode_t SleepMode);

/**
 * @brief      Puts ESP8266 to sleep for specific amount of milliseconds
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      Milliseconds: Number of milliseconds ESP will be in sleep mode
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_Sleep(TM_ESP8266_t* ESP8266, uint32_t Milliseconds);

/**
 * @brief      Connects to wifi network
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *ssid: SSID name to connect to
 * @param      *pass: Password for SSID. Set to "" if there is no password required
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_WifiConnect(TM_ESP8266_t* ESP8266, char* ssid, char* pass);

/**
 * @brief      Connects to wifi network and saves setting to internal flash of ESP for auto connect to network
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *ssid: SSID name to connect to
 * @param      *pass: Password for SSID. Set to "" if there is no password required
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_WifiConnectDefault(TM_ESP8266_t* ESP8266, char* ssid, char* pass);

/**
 * @brief      Gets AP settings of connected network
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure where data about AP will be stored
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_WifiGetConnected(TM_ESP8266_t* ESP8266);

/**
 * @brief      Disconnects from connected AP if any
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_WifiDisconnect(TM_ESP8266_t* ESP8266);

/**
 * @brief      Sets mode for ESP8266, either STA, AP or both
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      Mode: ESP8266 working mode. This parameter can be a value of @ref TM_ESP8266_Mode_t enumeration
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetMode(TM_ESP8266_t* ESP8266, TM_ESP8266_Mode_t Mode);

/**
 * @brief      Sets multiple connections for ESP8266 device.
 * @note       This setting is enabled by default
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      mux: Set to 0 to disable feature or 1 to enable
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetMux(TM_ESP8266_t* ESP8266, uint8_t mux);

/**
 * @brief      Sets data info on network data receive from module
 * @note       This setting is enabled by default to get proper working state
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      dinfo: Set to 1 to enable it, or zero to disable
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_Setdinfo(TM_ESP8266_t* ESP8266, uint8_t dinfo);

/**
 * @brief      Enables server mode on ESP8266 module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      port: Port number ESP will be visible on
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_ServerEnable(TM_ESP8266_t* ESP8266, uint16_t port);

/**
 * @brief      Sets server timeout value for connections waiting ESP to respond. This applies for all clients which connects to ESP module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      timeout: Timeout value in unit of seconds
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetServerTimeout(TM_ESP8266_t* ESP8266, uint16_t timeout);

TM_ESP8266_Result_t TM_ESP8266_GetSTAIP(TM_ESP8266_t* ESP8266);
TM_ESP8266_Result_t TM_ESP8266_GetSTAMAC(TM_ESP8266_t* ESP8266);
TM_ESP8266_Result_t TM_ESP8266_SetSTAMAC(TM_ESP8266_t* ESP8266, uint8_t* addr);
TM_ESP8266_Result_t TM_ESP8266_SetSTAMACDefault(TM_ESP8266_t* ESP8266, uint8_t* addr);
TM_ESP8266_Result_t TM_ESP8266_GetSTAIPPooling(TM_ESP8266_t* ESP8266);

TM_ESP8266_Result_t TM_ESP8266_GetAPIP(TM_ESP8266_t* ESP8266);
TM_ESP8266_Result_t TM_ESP8266_GetAPMAC(TM_ESP8266_t* ESP8266);
TM_ESP8266_Result_t TM_ESP8266_SetAPMAC(TM_ESP8266_t* ESP8266, uint8_t* addr);
TM_ESP8266_Result_t TM_ESP8266_SetAPMACDefault(TM_ESP8266_t* ESP8266, uint8_t* addr);
TM_ESP8266_Result_t TM_ESP8266_GetAPIPPooling(TM_ESP8266_t* ESP8266);

/**
 * @brief      Lists for all available AP stations ESP can connect to
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_ListWifiStations(TM_ESP8266_t* ESP8266);

/**
 * @brief      Gets current AP settings of ESP module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_GetAP(TM_ESP8266_t* ESP8266);

/**
 * @brief      Sets AP config values for ESP module
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *ESP8266_Config: Pointer to @ref TM_ESP8266_APConfig_t structure with settings
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_SetAP(TM_ESP8266_t* ESP8266, TM_ESP8266_APConfig_t* ESP8266_Config);

/**
 * @brief      Starts ping operation to another server
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *addr: Address to ping. Can be either domain name or IP address as string
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_Ping(TM_ESP8266_t* ESP8266, char* addr);

/**
 * @brief      Starts new connection as ESP client and connects to given address and port
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *name: Identification connection name for callback functions to detect proper connection
 * @param      *location: Domain name or IP address to connect to as string
 * @param      port: Port to connect to
 * @param      *user_parameters: Pointer to custom user parameters (if needed) which will later be passed to callback functions for client connection
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_StartClientConnection(TM_ESP8266_t* ESP8266, char* name, char* location, uint16_t port, void* user_parameters);

/**
 * @brief      Closes all opened connections
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_CloseAllConnections(TM_ESP8266_t* ESP8266);
/**
 * @brief      Closes specific previously opened connection
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *Connection: Pointer to @ref TM_ESP8266_Connection_t structure to close it
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_CloseConnection(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
/**
 * @brief      Makes a request to send data to specific open connection
 * @param      ESP8266: Pointer to working @ref TM_ESP8266_t structure
 * @param      *Connection: Pointer to @ref TM_ESP8266_Connection_t structure to close it
 * @return     Member of @ref TM_ESP8266_Result_t enumeration
 */
TM_ESP8266_Result_t TM_ESP8266_RequestSendData(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);

/* Callbacks */
void TM_ESP8266_Callback_Init(TM_ESP8266_t* ESP8266, TM_ESP8266_Init_t* Init);

void TM_ESP8266_Callback_WifiDisconnected(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_WifiConnected(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_WifiConnectFailed(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_WifiGotIP(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_WifiIPSet(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_DHCPTimeout(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_WifiDetected(TM_ESP8266_t* ESP8266, TM_ESP8266_APs_t* ESP8266_AP);

void TM_ESP8266_Callback_ServerConnectionActive(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ServerConnectionClosed(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ServerConnectionDataReceived(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer);
uint16_t TM_ESP8266_Callback_ServerConnectionSendData(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size);
void TM_ESP8266_Callback_ServerConnectionDataSent(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ServerConnectionDataSentError(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);

void TM_ESP8266_Callback_ClientConnectionConnected(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ClientConnectionError(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ClientConnectionTimeout(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ClientConnectionClosed(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
uint16_t TM_ESP8266_Callback_ClientConnectionSendData(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size);
void TM_ESP8266_Callback_ClientConnectionDataSent(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ClientConnectionDataSentError(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection);
void TM_ESP8266_Callback_ClientConnectionDataReceived(TM_ESP8266_t* ESP8266, TM_ESP8266_Connection_t* Connection, char* Buffer);

void TM_ESP8266_Callback_PingStarted(TM_ESP8266_t* ESP8266, char* address);
void TM_ESP8266_Callback_PingFinished(TM_ESP8266_t* ESP8266, TM_ESP8266_Ping_t* PING);

void TM_ESP8266_Callback_FirmwareUpdateStatus(TM_ESP8266_t* ESP8266, TM_ESP8266_FirmwareUpdate_t status);
void TM_ESP8266_Callback_FirmwareUpdateSuccess(TM_ESP8266_t* ESP8266);
void TM_ESP8266_Callback_FirmwareUpdateError(TM_ESP8266_t* ESP8266);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
