/**
 * @author  Tilen MAJERLE
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/09/hal-library-25-nrf24l01-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Library template 
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen MAJERLE

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_NRF24L01_H
#define TM_NRF24L01_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_NRF24L01P
 * @brief    nRF24L01+ library for STM32xxx devices - http://stm32f4-discovery.net/2015/09/hal-library-25-nrf24l01-for-stm32fxxx/
 * @{
 *
 * This library allows you to work with nRF24L01+ modules.
 * 
 * You can send and receive data from nRF24L01+ modules.
 * 
 * \par Default pinout
 * 	
\verbatim
NRF24L01+	STM32Fxxx	DESCRIPTION

GND			GND			Ground
VCC			3.3V		3.3V
CE			PD8			RF activated pin
CSN			PD7			Chip select pin for SPI
SCK			PC10		SCK pin for SPI
MOSI		PC12		MOSI pin for SPI
MISO		PC11		MISO pin for SPI
IRQ			Not used	Interrupt pin. Goes low when active. Pin functionality is active, but not used in library
\endverbatim 	
 *
 * IRQ pin is not used in this library, but its functionality is enabled by this software.
 *
 * You can still set any pin on Fxxx to be an external interrupt and handle interrupts from nRF24L01+ module.
 *
 * The easiest way to that is to use @ref TM_EXTI library and attach interrupt functionality to this pin
 * 
 * \par Custom pinout
 *
 * Add lines below in your defines.h file if you want to change default pinout:
 *
\code
//Change SPI used. Refer to TM SPI library to check which pins are for SPI
#define NRF24L01_SPI				SPI3
#define NRF24L01_SPI_PINS			TM_SPI_PinsPack_2

//Change CSN pin. This is for SPI communication
#define NRF24L01_CSN_PORT			GPIOD
#define NRF24L01_CSN_PIN			GPIO_Pin_7

//Change CE pin. This pin is used to enable/disable transmitter/receiver functionality
#define NRF24L01_CE_PORT			GPIOD
#define NRF24L01_CE_PIN				GPIO_Pin_8
\endcode
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
 - TM SPI
 - TM GPIO
\endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_spi.h"
#include "tm_stm32_gpio.h"

/**
 * @defgroup TM_NRF24L01P_Macros
 * @brief    Library defines
 * @{
 */

/* Default SPI used */
#ifndef NRF24L01_SPI
#define NRF24L01_SPI				SPI3
#define NRF24L01_SPI_PINS			TM_SPI_PinsPack_2
#endif

/* SPI chip enable pin */
#ifndef NRF24L01_CSN_PIN
#define NRF24L01_CSN_PORT			GPIOD
#define NRF24L01_CSN_PIN			GPIO_PIN_7
#endif

/* Chip enable for transmitting */
#ifndef NRF24L01_CE_PIN
#define NRF24L01_CE_PORT			GPIOD
#define NRF24L01_CE_PIN				GPIO_PIN_8
#endif

/* Pins configuration */
#define NRF24L01_CE_LOW				TM_GPIO_SetPinLow(NRF24L01_CE_PORT, NRF24L01_CE_PIN)
#define NRF24L01_CE_HIGH			TM_GPIO_SetPinHigh(NRF24L01_CE_PORT, NRF24L01_CE_PIN)
#define NRF24L01_CSN_LOW			TM_GPIO_SetPinLow(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN)
#define NRF24L01_CSN_HIGH			TM_GPIO_SetPinHigh(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN)

/* Interrupt masks */
#define NRF24L01_IRQ_DATA_READY     0x40 /*!< Data ready for receive */
#define NRF24L01_IRQ_TRAN_OK        0x20 /*!< Transmission went OK */
#define NRF24L01_IRQ_MAX_RT         0x10 /*!< Max retransmissions reached, last transmission failed */

/**
 * @}
 */
 
/**
 * @defgroup TM_NRF24L01P_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Interrupt structure 
 */
typedef union _TM_NRF24L01_IRQ_t {
	struct {
		uint8_t reserved0:4;
		uint8_t MaxRT:1;     /*!< Set to 1 if MAX retransmissions flag is set */
		uint8_t DataSent:1;  /*!< Set to 1 if last transmission is OK */
		uint8_t DataReady:1; /*!< Set to 1 if data are ready to be read */
		uint8_t reserved1:1;
	} F;
	uint8_t Status;          /*!< NRF status register value */
} TM_NRF24L01_IRQ_t;

/**
 * @brief  Transmission status enumeration
 */
typedef enum _TM_NRF24L01_Transmit_Status_t {
	TM_NRF24L01_Transmit_Status_Lost = 0x00,   /*!< Message is lost, reached maximum number of retransmissions */
	TM_NRF24L01_Transmit_Status_Ok = 0x01,     /*!< Message sent successfully */
	TM_NRF24L01_Transmit_Status_Sending = 0xFF /*!< Message is still sending */
} TM_NRF24L01_Transmit_Status_t;

/**
 * @brief  Data rate enumeration
 */
typedef enum _TM_NRF24L01_DataRate_t {
	TM_NRF24L01_DataRate_2M = 0x00, /*!< Data rate set to 2Mbps */
	TM_NRF24L01_DataRate_1M,        /*!< Data rate set to 1Mbps */
	TM_NRF24L01_DataRate_250k       /*!< Data rate set to 250kbps */
} TM_NRF24L01_DataRate_t;

/**
 * @brief  Output power enumeration
 */
typedef enum _TM_NRF24L01_OutputPower_t {
	TM_NRF24L01_OutputPower_M18dBm = 0x00,/*!< Output power set to -18dBm */
	TM_NRF24L01_OutputPower_M12dBm,       /*!< Output power set to -12dBm */
	TM_NRF24L01_OutputPower_M6dBm,        /*!< Output power set to -6dBm */
	TM_NRF24L01_OutputPower_0dBm          /*!< Output power set to 0dBm */
} TM_NRF24L01_OutputPower_t;

/**
 * @}
 */

/**
 * @defgroup TM_NRF24L01P_Functions
 * @brief    Library Functions
 *
 * Here are listed very basic functions to work with NRF modules
 *
 * @{
 */

/**
 * @brief  Initializes NRF24L01+ module
 * @param  channel: channel you will use for communication, from 0 to 125 eg. working frequency from 2.4 to 2.525 GHz
 * @param  payload_size: maximum data to be sent in one packet from one NRF to another.
 * @note   Maximal payload size is 32bytes
 * @retval 1
 */
uint8_t TM_NRF24L01_Init(uint8_t channel, uint8_t payload_size);

/**
 * @brief  Sets own address. This is used for settings own id when communication with other modules
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void TM_NRF24L01_SetMyAddress(uint8_t* adr);

/**
 * @brief  Sets address you will communicate with
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void TM_NRF24L01_SetTxAddress(uint8_t* adr);

/**
 * @brief  Gets number of retransmissions needed in last transmission
 * @param  None
 * @retval Number of retransmissions, between 0 and 15.
 */
uint8_t TM_NRF24L01_GetRetransmissionsCount(void);

/**
 * @brief  Sets NRF24L01+ to TX mode
 * @note   In this mode is NRF able to send data to another NRF module
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerUpTx(void);

/**
 * @brief  Sets NRF24L01+ to RX mode
 * @note   In this mode is NRF able to receive data from another NRF module.
 *         This is default mode and should be used all the time, except when sending data
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerUpRx(void);

/**
 * @brief  Sets NRF24L01+ to power down mode
 * @note   In power down mode, you are not able to transmit/receive data.
 *         You can wake up device using @ref TM_NRF24L01_PowerUpTx() or @ref TM_NRF24L01_PowerUpRx() functions
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerDown(void);

/**
 * @brief  Gets transmissions status
 * @param  None
 * @retval Transmission status. Return is based on @ref TM_NRF24L01_Transmit_Status_t enumeration
 */
TM_NRF24L01_Transmit_Status_t TM_NRF24L01_GetTransmissionStatus(void);

/**
 * @brief  Transmits data with NRF24L01+ to another NRF module
 * @param  *data: Pointer to 8-bit array with data.
 *         Maximum length of array can be the same as "payload_size" parameter on initialization
 * @retval None
 */
void TM_NRF24L01_Transmit(uint8_t *data);

/**
 * @brief  Checks if data is ready to be read from NRF24L01+
 * @param  None
 * @retval Data ready status:
 *            - 0: No data available for receive in bufferReturns
 *            - > 0: Data is ready to be collected
 */
uint8_t TM_NRF24L01_DataReady(void);

/**
 * @brief  Gets data from NRF24L01+
 * @param  *data: Pointer to 8-bits array where data from NRF will be saved
 * @retval None
 */
void TM_NRF24L01_GetData(uint8_t *data);

/**
 * @brief  Sets working channel
 * @note   Channel value is just an offset in units MHz from 2.4GHz
 *         For example, if you select channel 65, then operation frequency will be set to 2.465GHz.
 * @param  channel: RF channel where device will operate
 * @retval None 
 */
void TM_NRF24L01_SetChannel(uint8_t channel);

/**
 * @brief  Sets RF parameters for NRF24L01+
 * @param  DataRate: Data rate selection for NRF module. This parameter can be a value of @ref TM_NRF24L01_DataRate_t enumeration
 * @param  OutPwr: Output power selection for NRF module. This parameter can be a value of @ref TM_NRF24L01_OutputPower_t enumeration
 * @retval None
 */
void TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_t DataRate, TM_NRF24L01_OutputPower_t OutPwr);

/**
 * @brief  Gets NRLF+ status register value
 * @param  None
 * @retval Status register from NRF
 */
uint8_t TM_NRF24L01_GetStatus(void);

/**
 * @brief  Reads interrupts from NRF 
 * @param  *IRQ: Pointer to @ref TM_NRF24L01_IRQ_t where IRQ status will be saved
 * @retval IRQ status
 *            - 0: No interrupts are active
 *            - > 0: At least one interrupt is active
 */
uint8_t TM_NRF24L01_Read_Interrupts(TM_NRF24L01_IRQ_t* IRQ);

/**
 * @brief  Clears interrupt status
 * @param  None
 * @retval None
 */
void TM_NRF24L01_Clear_Interrupts(void);

/* Private */
void TM_NRF24L01_WriteRegister(uint8_t reg, uint8_t value);

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

