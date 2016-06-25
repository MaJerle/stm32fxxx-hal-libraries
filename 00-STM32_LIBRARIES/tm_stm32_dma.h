/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2016/04/hal-library-31-dma-stm32fxxx-devices
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   DMA library for STM32F4xx and STM32F7xx devices for several purposes
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
#ifndef TM_DMA_H
#define TM_DMA_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_DMA
 * @brief    DMA library for STM32F4xx and STM32F7xx devices for several purposes - http://stm32f4-discovery.com/2016/04/hal-library-31-dma-stm32fxxx-devices
 * @{
 *
 * This is a generic DMA library for STM32F4xx devices.
 * 
 * It allows you to easily enable interrupts for DMA streams, checking flags and more.
 * 
 * It is also designed in a ways that all interrupts that happens are handled by this library. No worries for correct stream handler anymore!
 *
 * \par Customization
 * 
 * Library implements ALL stream handlers (DMA1,2, streams 0 to 7, together is this 14 stream handlers) but some of my libraries uses default stream handler, like FATFS.
 * FATFS uses DMA2_Stream3 for interrupts for SDIO and in my FATFS library is written function name DMA2_Stream3_IRQHandler(). 
 *
 * DMA library also features this function and now if you use DMA library and FATFS library, you will get error for "Multiple declarations...".
 *
 * To prevent this link errors, you can open defines.h configuration file and add defines like below:
 *
@code
//Disable DMA2 Stream3 IRQ handler for TM DMA library
#define DMA2_STREAM3_DISABLE_IRQHANDLER

//For all other DMAs and STREAMS, syntax is similar:
#define DMAx_STREAMy_DISABLE_IRQHANDLER
//Where X is 1 or 2 for DMA1 or DMA2 and y is 0 to 7 for STREAM0 to STREAM7 on specific DMA
@endcode
 *
 * \par Handling interrupts
 * 
 * Every stream on DMA can make 5 interrupts. My library is designed in a way that specific callback is called for each interrupt type.
 * Check functions section for more informations.
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
 - STM32F4xx
 - STM32F4xx DMA
 - MISC
 - defines.h
 - attributes.h
@endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "attributes.h"

/**
 * @defgroup TM_DMA_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  DMA macros for interrupt flags
 */
#define DMA_FLAG_TCIF    ((uint32_t)0x00000020) /*!< DMA stream transfer complete */
#define DMA_FLAG_HTIF    ((uint32_t)0x00000010) /*!< DMA stream half transfer complete */
#define DMA_FLAG_TEIF    ((uint32_t)0x00000008) /*!< DMA stream transfer error */
#define DMA_FLAG_DMEIF   ((uint32_t)0x00000004) /*!< DMA stream direct mode error */
#define DMA_FLAG_FEIF    ((uint32_t)0x00000001) /*!< DMA stream FIFO error */
#define DMA_FLAG_ALL     ((uint32_t)0x0000003D) /*!< DMA stream all flags */

/* DMA1 preemption priority */
#ifndef DMA1_NVIC_PREEMPTION_PRIORITY
#define DMA1_NVIC_PREEMPTION_PRIORITY   0x01
#endif

/* DMA2 preemption priority */
#ifndef DMA2_NVIC_PREEMPTION_PRIORITY
#define DMA2_NVIC_PREEMPTION_PRIORITY   0x01
#endif

/**
 * @}
 */
 
/**
 * @defgroup TM_DMA_Typedefs
 * @brief    Library Typedefs
 * @{
 */
/* Typedefs here */
/**
 * @}
 */

/**
 * @defgroup TM_DMA_Functions
 * @brief    Library Functions
 * @{
 */

/** 
 * @brief  Initializes DMA stream
 * @param  *DMA_Stream: Pointer to @ref DMA_Stream_TypeDef DMA stream to initialize
 * @param  *HDMA: Pointer to @ref DMA_HandleTypeDef structure with settings. When this parameter
 *            is set to null, only click will be enabled for desired stream for DMA
 * @retval None
 */
void TM_DMA_Init(DMA_Stream_TypeDef* Stream, DMA_HandleTypeDef* HDMA);

/** 
 * @brief  Deinitializes DMA stream
 * @param  *DMA_Stream: Pointer to @ref DMA_Stream_TypeDef DMA stream to deinitialize
 * @retval None
 */
void TM_DMA_DeInit(DMA_Stream_TypeDef* Stream);

/** 
 * @brief  Starts DMA transmission for selected stream
 * @param  *HDMA: Pointer to @ref DMA_HandleTypeDef structure with settings where transmission will be started
 * @param  Source: Source address from where data will be sent
 * @param  Destination: Destination address where data will be sent
 * @param  Length: Number of bytes to sent
 * @retval None
 */
void TM_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t Source, uint32_t Destination, uint16_t Length);

/**
 * @brief  Clears all flags for selected DMA stream
 * @param  *DMA_Stream: Pointer to @ref DMA_Stream_TypeDef DMA stream where you want to clear flags 
 * @retval None
 */
void TM_DMA_ClearFlags(DMA_Stream_TypeDef* DMA_Stream);

/** 
 * @brief  Clears selected DMA interrupt flag
 * @param  *DMA_Stream: Pointer to @ref DMA_Stream_TypeDef DMA stream where you want to clear flags
 * @param  flags: Flag(s) which you wanna clear:
 *           - DMA_FLAG_TCIF: Transfer complete interrupt flag
 *           - DMA_FLAG HTIF: Half transfer complete interrupt flag
 *           - DMA_FLAG_TEIF: Transfer error interrupt flag
 *           - DMA_FLAG_DMEIF: Direct mode error interrupt flag
 *           - DMA_FLAG_FEIF: FIFO error interrupt flag
 *           - DMA_FLAG_ALL: All flags
 * @retval None
 */
void TM_DMA_ClearFlag(DMA_Stream_TypeDef* DMA_Stream, uint32_t flags);

/**
 * @brief  Enables interrupts for DMA stream
 * @note   It adds IRQ to NVIC and enables all possible DMA STREAM interrupts
 * @param  *DMA_Stream: Pointer to DMA stream where to enable interrupts
 * @retval None
 */
void TM_DMA_EnableInterrupts(DMA_Stream_TypeDef* DMA_Stream);

/**
 * @brief  Disables interrupts for DMA stream
 * @note   It adds IRQ to NVIC and enables all possible DMA STREAM interrupts
 * @param  *DMA_Stream: Pointer to DMA stream where to disable interrupts
 * @retval None
 */
void TM_DMA_DisableInterrupts(DMA_Stream_TypeDef* DMA_Stream);

/**
 * @brief  Transfer complete callback
 * @note   This function is called when interrupt for specific stream happens for transfer complete
 *
 * @note   This function is called immediately after DMA transfers last element from memory to your peripheral if DMA is configured as M2P (Memory 2 Peripheral). 
 *            When using for example USART, DMA will transfer last byte to USART peripheral register, but USART still has to do its job.
 *            It's important to know this fact, because you can get a lot of troubles, when for ex. you are working with RS485 and you have
 *            to control Enable pin for your transceiver.
 * @param  *DMA_Stream: Pointer to DMA stream where interrupt happens
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_DMA_TransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream);

/**
 * @brief  Half transfer complete callback
 * @note   This function is called when interrupt for specific stream happens for hal transfer complete
 * @param  *DMA_Stream: Pointer to DMA stream where interrupt happens
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_DMA_HalfTransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream);

/**
 * @brief  Transfer error callback
 * @note   This function is called when interrupt for specific stream happens for transfer error
 * @param  *DMA_Stream: Pointer to DMA stream where interrupt happens
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_DMA_TransferErrorHandler(DMA_Stream_TypeDef* DMA_Stream);

/**
 * @brief  Direct mode error callback
 * @note   This function is called when interrupt for specific stream happens for direct mode error
 * @param  *DMA_Stream: Pointer to DMA stream where interrupt happens
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_DMA_DirectModeErrorHandler(DMA_Stream_TypeDef* DMA_Stream);

/**
 * @brief  FIFO error error callback
 * @note   This function is called when interrupt for specific stream happens for FIFO error
 * @param  *DMA_Stream: Pointer to DMA stream where interrupt happens
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_DMA_FIFOErrorHandler(DMA_Stream_TypeDef* DMA_Stream);

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
