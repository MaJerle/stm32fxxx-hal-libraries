/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Library template 
 *	
@verbatim
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
@endverbatim
 */
#ifndef TM_LCD_H
#define TM_LCD_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_LCD
 * @brief    LCD library for STM32F7-Discovery, STM32F429-Discovery and STM32F439-Eval boards
 * @{
 *
 * This single library which supports multiple LCD devices depends on settings you have in your project.
 *
 * It uses LTDC and DMA2D for data transfer, so any LCD with these specifications can be used.
 *
@verbatim
//USE STM32F7-Discovery board, make these defines in defines.h file:
#define LCD_USE_STM32F7_DISCOVERY
#define SDRAM_USE_STM32F7_DISCOVERY

//USE STM32F439-Eval board, make these defines in defines.h file:
#define LCD_USE_STM32F439_EVAL
#define SDRAM_USE_STM32F439_EVAL

//USE STM32F429-Discovery board, make these defines in defines.h file:
#define LCD_USE_STM32F429_DISCOVERY
#define SDRAM_USE_STM32F429_DISCOVERY
@endverbatim
 * 
 * In addition, to use LCD on STM32F429-Discovery, you will also need my TM SPI library.
 *
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
 - TM GPIO
 - TM SDRAM
 - TM DMA2D GRAPHIC
 - TM FONTS
 - TM SPI in case STM32F429-Discovery is used
@endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "tm_stm32_sdram.h"
#include "tm_stm32_gpio.h"
#include "tm_stm32_fonts.h"
#include "tm_stm32_dma2d_graphic.h"

/**
 * @defgroup TM_LCD_Macros
 * @brief    Library defines
 * @{
 */

#if defined(LCD_USE_STM32F7_DISCOVERY) || defined(STM32F7_DISCOVERY)
	/* Check define */
	#ifndef LCD_USE_STM32F7_DISCOVERY
	#define LCD_USE_STM32F7_DISCOVERY
	#endif

	/* Set pixel settings */
	#define LCD_PIXEL_WIDTH        480
	#define LCD_PIXEL_HEIGHT       272
	#define LCD_PIXEL_SIZE         2
	
	/* LCD configuration */
	#define LCD_HSYNC              41
	#define LCD_HBP                13
	#define LCD_HFP                32
	#define LCD_VSYNC              10
	#define LCD_VBP                2
	#define LCD_VFP                2
	
	/* Frequency division for LTDC */
	#define LCD_FREQUENCY_DIV      5
#elif defined(LCD_USE_STM32F439_EVAL) || defined(STM32F439_EVAL)
	/* Check define */
	#ifndef LCD_USE_STM32F439_EVAL
	#define LCD_USE_STM32F439_EVAL
	#endif

	/* Set pixel settings */
	#define LCD_PIXEL_WIDTH        640
	#define LCD_PIXEL_HEIGHT       480
	#define LCD_PIXEL_SIZE         2
	
	/* LCD configuration */
	#define LCD_HSYNC              30
	#define LCD_HBP                114
	#define LCD_HFP                16
	#define LCD_VSYNC              3
	#define LCD_VBP                32
	#define LCD_VFP                10
	
	/* Frequency division for LTDC */
	#define LCD_FREQUENCY_DIV      1
#else	
	/* Check define */
	#ifndef LCD_USE_STM32F429_DISCOVERY
	#define LCD_USE_STM32F429_DISCOVERY
	#endif
	
	/* STM32F429-Discovery */
	#define LCD_PIXEL_WIDTH        240
	#define LCD_PIXEL_HEIGHT       320
	#define LCD_PIXEL_SIZE         2

	/* LCD configuration */
	#define LCD_HSYNC              9
	#define LCD_HBP                29
	#define LCD_HFP                2
	#define LCD_VSYNC              1
	#define LCD_VBP                3
	#define LCD_VFP                2
	
	/* Frequency division for LTDC */
	#define LCD_FREQUENCY_DIV      5
#endif

/* Frame settings */
#define LCD_FRAME_BUFFER           ((uint32_t)SDRAM_START_ADR)
#define LCD_BUFFER_OFFSET          ((uint32_t)(LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT * LCD_PIXEL_SIZE))

/**
 * @defgroup TM_LCD_Color
 * @brief    LCD Colors
 * @{
 */

#define LCD_COLOR_WHITE            0xFFFF
#define LCD_COLOR_BLACK            0x0000

/**
 * @}
 */

/**
 * @}
 */
 
/**
 * @defgroup TM_LCD_Typedefs
 * @brief    Library Typedefs
 * @{
 */

typedef enum _TM_LCD_Result_t {
	TM_LCD_Result_Ok = 0x00,
	TM_LCD_Result_Error,
	TM_LCD_Result_SDRAM_Error
} TM_LCD_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_LCD_Functions
 * @brief    Library Functions
 * @{
 */

TM_LCD_Result_t TM_LCD_Init(void);
TM_LCD_Result_t TM_LCD_Fill(uint32_t color);
TM_LCD_Result_t TM_LCD_DisplayOn(void);
TM_LCD_Result_t TM_LCD_DisplayOff(void);
uint16_t TM_LCD_GetWidth(void);
uint16_t TM_LCD_GetHeight(void);
uint32_t TM_LCD_GetFrameBuffer(void);

TM_LCD_Result_t TM_LCD_SetXY(uint16_t X, uint16_t Y);
TM_LCD_Result_t TM_LCD_SetFont(TM_FONT_t* Font);
TM_LCD_Result_t TM_LCD_SetColors(uint32_t Foreground, uint32_t Background);
TM_LCD_Result_t TM_LCD_Putc(char c);
TM_LCD_Result_t TM_LCD_Puts(char* str);

/**
 * @defgroup TM_LCD_GRAPHIC_Functions
 * @brief    Library Functions
 * @{
 */

TM_LCD_Result_t TM_LCD_DrawPixel(uint16_t X, uint16_t Y, uint32_t color);
uint32_t TM_LCD_GetPixel(uint16_t X, uint16_t Y);
TM_LCD_Result_t TM_LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
TM_LCD_Result_t TM_LCD_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color);
TM_LCD_Result_t TM_LCD_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color);
TM_LCD_Result_t TM_LCD_DrawRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color);
TM_LCD_Result_t TM_LCD_DrawFilledRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color);
TM_LCD_Result_t TM_LCD_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
TM_LCD_Result_t TM_LCD_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
 * @}
 */

/**
 * @defgroup TM_LCD_LAYER_Functions
 * @brief    Library Functions
 * @{
 */
 
TM_LCD_Result_t TM_LCD_SetLayer1(void);
TM_LCD_Result_t TM_LCD_SetLayer2(void);
TM_LCD_Result_t TM_LCD_SetLayer1Opacity(uint8_t opacity);
TM_LCD_Result_t TM_LCD_SetLayer2Opacity(uint8_t opacity);
TM_LCD_Result_t TM_LCD_ChangeLayers(void);
TM_LCD_Result_t TM_LCD_Layer2To1(void);
TM_LCD_Result_t TM_LCD_Layer1To2(void);

/**
 * @}
 */
 
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
