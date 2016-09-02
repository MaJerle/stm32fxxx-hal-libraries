/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   DMA functionality for TM I2C library for STM32F4xx and STM32F7xx devices
 *	
@verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

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
@endverbatim
 */
#ifndef TM_I2C_DMA_H
#define TM_I2C_DMA_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_I2C_DMA
 * @brief    DMA functionality for @ref TM_I2C library for STM32F4xx and STM32F7xx devices - http://stm32f4-discovery.net/2016/04/hal-library-33-dma-extension-for-I2C-on-stm32fxxx
 * @{
 *
 * This library allows you to use DMA with I2C peripheral.
 *
 * It can send (TX only), receive (RX only) or transmit (TX and RX) data over I2C with DMA feature.
 *
 * \par 
 *
 * @note All possible DMA Streams and Channels for I2C DMA can be found in STM32Fxxx Reference manual.
 *
 * Default DMA streams and channels:
 *
@verbatim
I2Cx     | DMA  | DMA TX Stream | DMA TX Channel | DMA RX Stream | DMA RX Channel
                                                   
I2C1     | DMA1 | DMA Stream 6  | DMA Channel 1  | DMA Stream 0  | DMA Channel 1 
I2C2     | DMA1 | DMA Stream 7  | DMA Channel 7  | DMA Stream 3  | DMA Channel 7
I2C3     | DMA1 | DMA Stream 4  | DMA Channel 3  | DMA Stream 2  | DMA Channel 3
@endverbatim
 *
 * \par Changelog
 *
@verbatim
 Version 1.0
  - First release
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM DMA
 - TM I2C
 - stdlib.h
@endverbatim
 */
#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_dma.h"
#include "tm_stm32_i2c.h"
#include "stdlib.h"

/**
 * @defgroup TM_I2C_DMA_Macros
 * @brief    Library defines
 * @{
 */

/* I2C1 TX and RX default settings */
#ifndef I2C1_DMA_TX_STREAM
#define I2C1_DMA_TX_STREAM    DMA1_Stream6
#define I2C1_DMA_TX_CHANNEL   DMA_CHANNEL_1
#endif
#ifndef I2C1_DMA_RX_STREAM
#define I2C1_DMA_RX_STREAM    DMA1_Stream0
#define I2C1_DMA_RX_CHANNEL   DMA_CHANNEL_1
#endif

/* I2C2 TX and RX default settings */
#ifndef I2C2_DMA_TX_STREAM
#define I2C2_DMA_TX_STREAM    DMA1_Stream7
#define I2C2_DMA_TX_CHANNEL   DMA_CHANNEL_7
#endif
#ifndef I2C2_DMA_RX_STREAM
#define I2C2_DMA_RX_STREAM    DMA1_Stream3
#define I2C2_DMA_RX_CHANNEL   DMA_CHANNEL_7
#endif

/* I2C3 TX and RX default settings */
#ifndef I2C3_DMA_TX_STREAM
#define I2C3_DMA_TX_STREAM    DMA1_Stream4
#define I2C3_DMA_TX_CHANNEL   DMA_CHANNEL_3
#endif
#ifndef I2C3_DMA_RX_STREAM
#define I2C3_DMA_RX_STREAM    DMA1_Stream2
#define I2C3_DMA_RX_CHANNEL   DMA_CHANNEL_3
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_I2C_DMA_Typedefs
 * @brief    Library Typedefs
 * @{
 */
/* Typedefs here */
/**
 * @}
 */

/**
 * @defgroup TM_I2C_DMA_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes I2C peripheral DMA
 * @note   This function initializes TX and RX DMA streams for I2C
 *
 * @note   I2C HAVE TO be previously initialized using @ref TM_I2C library
 * @param  *I2Cx: Pointer to I2C peripheral where you want to enable DMA
 * @retlva None 
 */
void TM_I2C_DMA_Init(I2C_TypeDef* I2Cx);

/**
 * @brief  Initializes I2C DMA functionality with custom DMA stream and channel options
 * @note   I2C HAVE TO be previously initialized using @ref TM_USART library
 *
 * @note   Use this function only in case default Stream and Channel settings are not good for you
 * @param  *I2Cx: Pointer to I2Cx where you want to set custom DMA streams and channels
 * @param  *TX_Stream: Pointer to DMAy_Streamx, where y is DMA (1 or 2) and x is Stream (0 to 7)
 * @param  TX_Channel: Select DMA TX channel for your I2C in specific DMA Stream
 * @param  *RX_Stream: Pointer to DMAy_Streamx, where y is DMA (1 or 2) and x is Stream (0 to 7)
 * @param  RX_Channel: Select DMA RX channel for your I2C in specific DMA Stream
 * @retval None
 */
void TM_I2C_DMA_InitWithStreamAndChannel(I2C_TypeDef* I2Cx, DMA_Stream_TypeDef* TX_Stream, uint32_t TX_Channel, DMA_Stream_TypeDef* RX_Stream, uint32_t RX_Channel);

/**
 * @brief  Deinitializes I2C DMA functionality
 * @param  *I2Cx: Pointer to I2Cx where you want to disable DMA mode
 * @retval None
 */
void TM_I2C_DMA_Deinit(I2C_TypeDef* I2Cx);

/**
 * @brief  Transmits (exchanges) data over I2C with DMA
 * @note   Try not to use local variables pointers for DMA memory as TX and RX Buffers
 * @param  *I2Cx: Pointer to I2Cx where DMA transmission will happen
 * @param  *TX_Buffer: Pointer to TX_Buffer where DMA will take data to sent over I2C.
 *            Set this parameter to NULL, if you want to sent "0x00" and only receive data into *RX_Buffer pointer
 * @param  *RX_Buffer: Pointer to RX_Buffer where DMA will save data from I2C.
 *            Set this parameter to NULL, if you don't want to receive any data, only sent from TX_Buffer
 * @param  count: Number of bytes to be send/received over I2C with DMA
 * @retval Transmission started status:
 *            - 0: DMA has not started with sending data
 *            - > 0: DMA has started with sending data
 */
uint8_t TM_I2C_DMA_Transmit(I2C_TypeDef* I2Cx, uint8_t* TX_Buffer, uint8_t* RX_Buffer, uint16_t count);

/**
 * @brief  Sends data over I2C without receiving data back using DMA
 * @note   Try not to use local variables pointers for DMA memory as TX and RX Buffers
 * @param  *I2Cx: Pointer to I2Cx where DMA transmission will happen
 * @param  *TX_Buffer: Pointer to TX_Buffer where DMA will take data to sent over I2C
 * @param  count: Number of bytes to be send/received over I2C with DMA
 * @retval Sending started status:
 *            - 0: DMA has not started with sending data
 *            - > 0: DMA has started with sending data
 * @note   Defined as macro for faster execution
 */
#define TM_I2C_DMA_Send(I2Cx, TX_Buffer, count)   (TM_I2C_DMA_Transmit(I2Cx, TX_Buffer, NULL, count))

/**
 * @brief  Sends dummy byte (0x00) over I2C to receive data back from slave over DMA
 * @note   Try not to use local variables pointers for DMA memory as TX and RX Buffers
 * @param  I2Cx: Pointer to I2Cx where DMA transmission will happen
 * @param  RX_Buffer: Pointer to RX_Buffer where DMA will save data from I2C
 * @param  count: Number of bytes to be received over I2C with DMA
 * @retval Receiving started status:
 *            - 0: DMA has not started with sending data
 *            - > 0: DMA has started with sending data
 * @note   Defined as macro for faster execution
 */
#define TM_I2C_DMA_Receive(I2Cx, RX_Buffer, count)   (TM_I2C_DMA_Transmit(I2Cx, NULL, RX_Buffer, count))

/**
 * @brief  Sends one byte value multiple times over I2C with DMA
 * @param  I2Cx: Pointer to I2Cx where DMA transmission will happen
 * @param  value: Byte value to be sent
 * @param  count: Number of bytes with value of @arg value will be sent
 * @retval Transmission started status:
 *            - 0: DMA has not started with sending data
 *            - > 0: DMA has started with sending data
 */
uint8_t TM_I2C_DMA_SendByte(I2C_TypeDef* I2Cx, uint8_t value, uint16_t count);

/**
 * @brief  Sends one half word value multiple times over I2C with DMA
 * @param  I2Cx: Pointer to I2Cx where DMA transmission will happen
 * @param  value: Byte value to be sent
 * @param  count: Number of half words with value of @arg value will be sent
 * @retval Transmission started status:
 *            - 0: DMA has not started with sending data
 *            - > 0: DMA has started with sending data
 */
uint8_t TM_I2C_DMA_SendHalfWord(I2C_TypeDef* I2Cx, uint16_t value, uint16_t count);

/**
 * @brief  Checks if I2C DMA is still sending/receiving data
 * @param  *I2Cx: Pointer to I2Cx where you want to enable DMA TX mode
 * @retval Sending status:
 *            - 0: I2C DMA does not sending any more
 *            - > 0: I2C DMA is still sending data 
 */
uint8_t TM_I2C_DMA_Transmitting(I2C_TypeDef* I2Cx);

/**
 * @brief  Gets TX DMA stream for specific I2C 
 * @param  *I2Cx: Pointer to I2Cx peripheral where you want to get TX stream
 * @retval Pointer to I2C DMA TX stream
 */
DMA_Stream_TypeDef* TM_I2C_DMA_GetStreamTX(I2C_TypeDef* I2Cx);

/**
 * @brief  Gets RX DMA stream for specific I2C 
 * @param  *I2Cx: Pointer to I2Cx peripheral where you want to get RX stream
 * @retval Pointer to I2C DMA RX stream
 */
DMA_Stream_TypeDef* TM_I2C_DMA_GetStreamRX(I2C_TypeDef* I2Cx);

/**
 * @brief  Enables DMA stream interrupts for specific I2C
 * @param  *I2Cx: Pointer to I2Cx peripheral where you want to enable DMA stream interrupts
 * @retval None
 */
void TM_I2C_DMA_EnableInterrupts(I2C_TypeDef* I2Cx);

/**
 * @brief  Disables DMA stream interrupts for specific I2C
 * @param  *I2Cx: Pointer to I2Cx peripheral where you want to disable DMA stream interrupts
 * @retval None
 */
void TM_I2C_DMA_DisableInterrupts(I2C_TypeDef* I2Cx);

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
