/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-12-lcd-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   Library template 
 *	
\verbatim
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
\endverbatim
 */
#ifndef TM_LCD_H
#define TM_LCD_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_LCD
 * @brief    LCD library for STM32F7-Discovery, STM32F429-Discovery and STM32F439-Eval boards - http://stm32f4-discovery.net/2015/07/hal-library-12-lcd-for-stm32fxxx/
 * @{
 *
 * This single library which supports multiple LCD devices depends on settings you have in your project.
 *
 * It uses LTDC and DMA2D for data transfer, so any LCD with these specifications can be used.
 *
 * \par Set your target
 *
 * @note  If you follow my description in @ref TM_DISCO library on how to select your used board, then this library will automatically select your "target".
 *
 * You will need to set bottom defines in case you don't use my @ref TM_DISCO library configuration for board used.
 *
\code
//USE STM32F7-Discovery board, make these defines in defines.h file:
#define LCD_USE_STM32F7_DISCOVERY
#define SDRAM_USE_STM32F7_DISCOVERY

//USE STM32F439-Eval board, make these defines in defines.h file:
#define LCD_USE_STM32F439_EVAL
#define SDRAM_USE_STM32F439_EVAL

//USE STM32F429-Discovery board, make these defines in defines.h file:
#define LCD_USE_STM32F429_DISCOVERY
#define SDRAM_USE_STM32F429_DISCOVERY
\endcode
 *
 * @note  To use LCD on STM32F429-Discovery, you will also need @ref TM_SPI library for LCD configurations.
 *
 * \par Supported LCD orientations
 * 
 * Library supports changeable orientation for LCD using @ref TM_LCD_SetOrientation where you can use these values:
 *
\verbatim
- 0: 180 degrees, default mode inverted
- 1: Normal mode, default selected
- 2: 90 degrees
- 3: 270 degrees
\endverbatim
 *
 * \par Supported fonts for strings
 *
 * For list of all supported fonts, check @ref TM_FONTS library with description.
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
 - TM GPIO
 - TM SDRAM
 - TM DMA2D GRAPHIC
 - TM FONTS
 - TM SPI in case STM32F429-Discovery is used
\endverbatim
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
#define LCD_COLOR_RED              0xF800
#define LCD_COLOR_GREEN            0x07E0
#define LCD_COLOR_GREEN2           0xB723
#define LCD_COLOR_BLUE             0x001F
#define LCD_COLOR_BLUE2            0x051D
#define LCD_COLOR_YELLOW           0xFFE0
#define LCD_COLOR_ORANGE           0xFBE4
#define LCD_COLOR_CYAN             0x07FF
#define LCD_COLOR_MAGENTA          0xA254
#define LCD_COLOR_GRAY             0x7BEF
#define LCD_COLOR_BROWN            0xBBCA

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

/**
 * @brief  LCD result enumeration
 */
typedef enum _TM_LCD_Result_t {
	TM_LCD_Result_Ok = 0x00,  /*!< Everything OK */
	TM_LCD_Result_Error,      /*!< An error occurred */
	TM_LCD_Result_SDRAM_Error /*!< SDRAM initialization has failed */
} TM_LCD_Result_t;

/**
 * @}
 */

/**
 * @defgroup TM_LCD_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes LCD
 * @note   This function must be called at the beginning of LCD operations
 * @param  None
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_Init(void);

/**
 * @brief  Fills currently active layer with desired color
 * @note   To select active layer, use @ref TM_LCD_SetLayer1() or @ref TM_LCD_SetLayer2() functions
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_Fill(uint32_t color);

/**
 * @brief  Sets display ON
 * @note   This function is already called inside @ref TM_LCD_Init() function
 * @param  None
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DisplayOn(void);

/**
 * @brief  Sets display OFF
 * @param  None
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DisplayOff(void);

/**
 * @brief  Gets LCD width in unit of pixels
 * @param  None
 * @retval LCD width in unit of pixels 
 */
uint16_t TM_LCD_GetWidth(void);

/**
 * @brief  Gets LCD height in unit of pixels
 * @param  None
 * @retval LCD width in unit of pixels 
 */
uint16_t TM_LCD_GetHeight(void);

/**
 * @brief  Gets location where LCD buffer is located
 * @param  None
 * @retval Location in memory where LCD buffer is located
 */
uint32_t TM_LCD_GetFrameBuffer(void);

/**
 * @brief  Sets LCD orientation
 * @param  orientation: LCD orientation you wanna use, values 0 to 3 are available
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_SetOrientation(uint8_t orientation);

/**
 * @brief  Gets LCD orientation
 * @retval LCD orientation
 */
uint8_t TM_LCD_GetOrientation(void);

/**
 * @defgroup TM_LCD_String_Functions
 * @brief    String functions
 * @{
 */

/**
 * @brief  Sets X and Y location for character drawings
 * @param  X: X coordinate for character drawings
 * @param  Y: Y coordinate for character drawings
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_SetXY(uint16_t X, uint16_t Y);

/**
 * @brief  Gets current X location for LCD strings
 * @param  None
 * @retval Current X location for string operations
 */
uint16_t TM_LCD_GetCurrentX(void);

/**
 * @brief  Gets current Y location for LCD strings
 * @param  None
 * @retval Current Y location for string operations
 */
uint16_t TM_LCD_GetCurrentY(void);

/**
 * @brief  Sets active font for character drawings
 * @param  *Font: Pointer to @ref TM_FONT_t structure with font
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_SetFont(TM_FONT_t* Font);

/**
 * @brief  Gets active font for character drawings
 * @param  None
 * @retval Pointer to @ref TM_FONT_t structure with font
 */
TM_FONT_t* TM_LCD_GetFont(void);

/**
 * @brief  Sets foreground and background colors for character drawings
 * @param  Foreground: Foreground color for characters
 * @param  Background: Background color for characters
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_SetColors(uint32_t Foreground, uint32_t Background);

/**
 * @brief  Puts single character to LCD
 * @note   To set X and Y coordinate, use @ref TM_LCD_SetXY function
 * @param  c: Character to be written on LCD
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_Putc(char c);

/**
 * @brief  Puts string to LCD
 * @note   To set X and Y coordinate, use @ref TM_LCD_SetXY function
 * @param  *str: String to be written on LCD
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_Puts(char* str);

/**
 * @}
 */

/**
 * @defgroup TM_LCD_GRAPHIC_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Draws single pixel to LCD
 * @param  X: X coordinate for pixel
 * @param  Y: Y coordinate for pixel
 * @param  color: Color index in RGB565 mode
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawPixel(uint16_t X, uint16_t Y, uint32_t color);

/**
 * @brief  Gets pixel index at specific coordinate
 * @param  X: X coordinate for pixel
 * @param  Y: Y coordinate for pixel
 * @retval Pixel index in RGB565 format 
 */
uint32_t TM_LCD_GetPixel(uint16_t X, uint16_t Y);

/**
 * @brief  Draws line between 2 coordinates
 * @param  x0: First point X location
 * @param  y0: First point Y location
 * @param  x1: Second point X location
 * @param  y1: Second point Y location
 * @param  color: Color index for line in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);

/**
 * @brief  Draws rectangle on LCD
 * @param  x0: Top left X location
 * @param  y0: Top left Y location
 * @param  Width: Rectangle width in unit of pixels
 * @param  Height: Rectangle height in unit of pixels
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color);

/**
 * @brief  Draws filled rectangle on LCD
 * @param  x0: Top left X location
 * @param  y0: Top left Y location
 * @param  Width: Rectangle width in unit of pixels
 * @param  Height: Rectangle height in unit of pixels
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color);

/**
 * @brief  Draws rounded rectangle on LCD
 * @param  x0: Top left X location
 * @param  y0: Top left Y location
 * @param  Width: Rectangle width in unit of pixels
 * @param  Height: Rectangle height in unit of pixels
 * @param  r: Radius in unit of pixels in each corner
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color);

/**
 * @brief  Draws filled rounded rectangle on LCD
 * @param  x0: Top left X location
 * @param  y0: Top left Y location
 * @param  Width: Rectangle width in unit of pixels
 * @param  Height: Rectangle height in unit of pixels
 * @param  r: Radius in unit of pixels in each corner
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawFilledRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color);

/**
 * @brief  Draws circle on LCD
 * @param  x0: X coordinate of circle's center location
 * @param  y0: Y coordinate of circle's center location
 * @param  r: Radius in unit of pixels
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
 * @brief  Draws filled circle on LCD
 * @param  x0: X coordinate of circle's center location
 * @param  y0: Y coordinate of circle's center location
 * @param  r: Radius in unit of pixels
 * @param  color: Color index in RGB565 format
 * @retval Member of @ref TM_LCD_Result_t enumeration
 */
TM_LCD_Result_t TM_LCD_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
 * @}
 */

/**
 * @defgroup TM_LCD_LAYER_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Sets layer 1 as active layer for drawing on LCD 
 * @param  None
 * @retval None 
 */
TM_LCD_Result_t TM_LCD_SetLayer1(void);

/**
 * @brief  Sets layer 2 as active layer for drawing on LCD 
 * @param  None
 * @retval None 
 */
TM_LCD_Result_t TM_LCD_SetLayer2(void);

/**
 * @brief  Sets layer 1 alpha value
 * @param  opacity: Alpha value as 8-bit parameter
 * @retval None
 */
TM_LCD_Result_t TM_LCD_SetLayer1Opacity(uint8_t opacity);

/**
 * @brief  Sets layer 2 alpha value
 * @param  opacity: Alpha value as 8-bit parameter
 * @retval None
 */
TM_LCD_Result_t TM_LCD_SetLayer2Opacity(uint8_t opacity);

/**
 * @brief  Changes currently visible layer on LCD
 * @param  None
 * @retval None
 */
TM_LCD_Result_t TM_LCD_ChangeLayers(void);

/**
 * @brief  Copies content from layer 2 to layer 1
 * @param  None
 * @retval None
 */
TM_LCD_Result_t TM_LCD_Layer2To1(void);

/**
 * @brief  Copies content from layer 1 to layer 2
 * @param  None
 * @retval None
 */
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
