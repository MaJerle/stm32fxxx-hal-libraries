/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_lcd.h"

/* In case of STM32F429-Discovery, we need SPI also */
#if defined(LCD_USE_STM32F429_DISCOVERY)
#include "tm_stm32_spi.h"

/* Private defines */
/* SPI settings */
#define ILI9341_SPI 				SPI5
#define ILI9341_SPI_PINS			TM_SPI_PinsPack_1

/* Control pins */
#define ILI9341_CS_PORT				GPIOC
#define ILI9341_CS_PIN				GPIO_PIN_2
#define ILI9341_WRX_PORT			GPIOD
#define ILI9341_WRX_PIN				GPIO_PIN_13

/* Pin functions */
#define ILI9341_CS_SET				TM_GPIO_SetPinHigh(ILI9341_CS_PORT, ILI9341_CS_PIN)
#define ILI9341_CS_RESET			TM_GPIO_SetPinLow(ILI9341_CS_PORT, ILI9341_CS_PIN)
#define ILI9341_WRX_SET				TM_GPIO_SetPinHigh(ILI9341_WRX_PORT, ILI9341_WRX_PIN)
#define ILI9341_WRX_RESET			TM_GPIO_SetPinLow(ILI9341_WRX_PORT, ILI9341_WRX_PIN)

/* Commands */
#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC					0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB					0x51
#define ILI9341_WCD					0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC					0xB1
#define ILI9341_BPC					0xB5
#define ILI9341_DFC					0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC					0xF7

/* Function declarations */
static void TM_ILI9341_SendCommand(uint8_t data);
static void TM_ILI9341_SendData(uint8_t data);
static void TM_ILI9341_Delay(__IO uint32_t delay);
#endif

/* Private structures, variables and functions */
static void TM_LCD_INT_InitLTDC(void);
static void TM_LCD_INT_InitLayers(void);
static void TM_LCD_INT_InitLCD(void);
static void TM_LCD_INT_InitPins(void);

/* Private variables */
static LTDC_HandleTypeDef LTDCHandle;

/* Private structure */
typedef struct _TM_LCD_INT_t {
	uint16_t Width;
	uint16_t Height;
	uint16_t CurrentWidth;
	uint16_t CurrentHeight;
	uint8_t Orientation;
	uint32_t CurrentFrameBuffer;
	uint32_t FrameStart;
	uint32_t FrameOffset;
	uint8_t CurrentLayer;
	
	/* Strings */
	uint32_t ForegroundColor;
	uint32_t BackgroundColor;
	TM_FONT_t* CurrentFont;
	uint16_t CurrentX;
	uint16_t StartX;
	uint16_t CurrentY;
} TM_LCD_INT_t;
static TM_LCD_INT_t LCD;

TM_LCD_Result_t TM_LCD_Init(void) {
	TM_DMA2DGRAPHIC_INT_Conf_t DMA2DConf;
	
	/* Init SDRAM */
	if (!TM_SDRAM_Init()) {
		/* Return error */
		return TM_LCD_Result_SDRAM_Error;
	}
	
	/* Fill default structure */
	LCD.Width = LCD_PIXEL_WIDTH;
	LCD.Height = LCD_PIXEL_HEIGHT;
	LCD.CurrentFrameBuffer = LCD_FRAME_BUFFER;
	LCD.FrameStart = LCD_FRAME_BUFFER;
	LCD.FrameOffset = LCD_BUFFER_OFFSET;
	LCD.CurrentFont = &TM_Font_11x18;
	LCD.ForegroundColor = 0x0000;
	LCD.BackgroundColor = 0xFFFF;
	LCD.Orientation = 1;
	
	/* Set orientation */
	TM_LCD_SetOrientation(LCD.Orientation);
	
	/* Set configrations for DMA2D */
	DMA2DConf.BufferStart = LCD_FRAME_BUFFER;
	DMA2DConf.BufferOffset = LCD_BUFFER_OFFSET;
	DMA2DConf.BytesPerPixel = LCD_PIXEL_SIZE;
	DMA2DConf.Width = LCD_PIXEL_WIDTH;
	DMA2DConf.Height = LCD_PIXEL_HEIGHT;
	DMA2DConf.Orientation = 1;
	
	/* Init LCD pins */
	TM_LCD_INT_InitPins();
	
	/* Init LCD dependant settings */
	TM_LCD_INT_InitLCD();
	
	/* Init LTDC peripheral */
	TM_LCD_INT_InitLTDC();
	
	/* Init LTDC layers */
	TM_LCD_INT_InitLayers();
	
	/* Init DMA2D GRAPHICS */
	TM_DMA2DGRAPHIC_Init();
	
	/* Set settings */
	TM_INT_DMA2DGRAPHIC_SetConf(&DMA2DConf);
	
	/* Enable LCD */
	TM_LCD_DisplayOn();
	
	/* Set layer 1 as active layer */
	TM_LCD_SetLayer1();
	TM_LCD_Fill(LCD_COLOR_WHITE);
	TM_LCD_SetLayer2();
	TM_LCD_Fill(LCD_COLOR_WHITE);
	TM_LCD_SetLayer1();
	
	/* Set layer 1 as active layer */
	TM_LCD_SetLayer1Opacity(255);
	TM_LCD_SetLayer2Opacity(0);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_Fill(uint32_t color) {
	/* Erase memory */
	TM_DMA2DGRAPHIC_Fill(color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_SetOrientation(uint8_t orientation) {
	/* Check input */
	if (orientation > 3) {
		return TM_LCD_Result_Error;
	}
	
	/* Save rotation */
	LCD.Orientation = orientation;
	
	/* Check X and Y values */
	if (
		orientation == 0 ||
		orientation == 1
	) {
		LCD.CurrentHeight = LCD.Height;
		LCD.CurrentWidth = LCD.Width;
	} else {
		LCD.CurrentHeight = LCD.Width;
		LCD.CurrentWidth = LCD.Height;
	}
	
	/* Rotate DMA2D graphic library */
	TM_DMA2DGRAPHIC_SetOrientation(orientation);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DisplayOn(void) {
	/* Enable LTDC */
	LTDC->GCR |= LTDC_GCR_LTDCEN;
	
#if defined(LCD_USE_STM32F7_DISCOVERY)
	/* Set PIN high */
	TM_GPIO_SetPinHigh(GPIOI, GPIO_PIN_12);	
    TM_GPIO_SetPinHigh(GPIOK, GPIO_PIN_3);
#endif
#if defined(LCD_USE_STM32F429_DISCOVERY)
	/* Send command to display on */
	TM_ILI9341_SendCommand(0x29);
#endif
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DisplayOff(void) {
	/* Disable LTDC */
	LTDC->GCR &= ~LTDC_GCR_LTDCEN;
	
#if defined(LCD_USE_STM32F7_DISCOVERY)
	/* Set PIN low */
	TM_GPIO_SetPinLow(GPIOI, GPIO_PIN_12);	
    TM_GPIO_SetPinLow(GPIOK, GPIO_PIN_3);
#endif
#if defined(LCD_USE_STM32F429_DISCOVERY)
	/* Send command to display off */
	TM_ILI9341_SendCommand(0x28);
#endif
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_SetXY(uint16_t X, uint16_t Y) {
	/* Check if we are inside LCD */
	if (X >= LCD.CurrentWidth || Y >= LCD.CurrentHeight) {
		return TM_LCD_Result_Error;
	}
	
	/* Set new values */
	LCD.CurrentX = X;
	LCD.CurrentY = Y;
	LCD.StartX = X;
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

uint16_t TM_LCD_GetCurrentX(void) {
	/* Return current X location for strings */
	return LCD.CurrentX;
}

uint16_t TM_LCD_GetCurrentY(void) {
	/* Return current Y location for strings */
	return LCD.CurrentY;
}

TM_LCD_Result_t TM_LCD_SetFont(TM_FONT_t* Font) {
	/* Set new font used for drawing */
	LCD.CurrentFont = Font;
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_FONT_t* TM_LCD_GetFont(void) {
	/* Return pointer to font */
	return LCD.CurrentFont;
}

TM_LCD_Result_t TM_LCD_SetColors(uint32_t Foreground, uint32_t Background) {
	/* Set new colors */
	LCD.ForegroundColor = Foreground;
	LCD.BackgroundColor = Background;
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_Putc(char c) {
	uint32_t i, b, j;
	
	/* Check current coordinates */
	if ((LCD.CurrentX + LCD.CurrentFont->FontWidth) >= LCD.CurrentWidth || c == '\n') {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		LCD.CurrentY += LCD.CurrentFont->FontHeight;
		LCD.CurrentX = LCD.StartX;
		
		/* Check X */
		if ((LCD.CurrentX + LCD.CurrentFont->FontWidth) >= LCD.CurrentWidth) {
			LCD.CurrentX = 0;
		}
		
		/* Check for Y position */
		if (LCD.CurrentY >= LCD.CurrentHeight) {
			/* Return error */
			return TM_LCD_Result_Error;
		}
	}
	
	/* Draw character */
	if (c != '\n') {
		/* Draw all pixels */
		for (i = 0; i < LCD.CurrentFont->FontHeight; i++) {
			b = LCD.CurrentFont->data[(c - 32) * LCD.CurrentFont->FontHeight + i];
			for (j = 0; j < LCD.CurrentFont->FontWidth; j++) {
				if ((b << j) & 0x8000) {
					TM_DMA2DGRAPHIC_DrawPixel(LCD.CurrentX + j, (LCD.CurrentY + i), LCD.ForegroundColor);
				} else {
					TM_DMA2DGRAPHIC_DrawPixel(LCD.CurrentX + j, (LCD.CurrentY + i), LCD.BackgroundColor);
				}
			}
		}
	
		/* Set new current X location */
		LCD.CurrentX += LCD.CurrentFont->FontWidth;
	}
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawPixel(uint16_t X, uint16_t Y, uint32_t color) {
	/* Draw pixel at desired location */
	TM_DMA2DGRAPHIC_DrawPixel(X, Y, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

uint32_t TM_LCD_GetPixel(uint16_t X, uint16_t Y) {
	/* Get pixel at desired location */
	return TM_DMA2DGRAPHIC_GetPixel(X, Y);
}

TM_LCD_Result_t TM_LCD_Puts(char* str) {
	/* Send till string ends or error returned */
	while (*str) {
		/* Check if string OK */
		if (TM_LCD_Putc(*str) != TM_LCD_Result_Ok) {
			/* Return error */
			return TM_LCD_Result_Error;
		}
		
		/* Increase pointer */
		str++;
	}
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_SetLayer1(void) {
	/* Fill settings */
	LCD.CurrentFrameBuffer = LCD.FrameStart;
	LCD.CurrentLayer = 0;
	
	/* Set layer for DMA2D */
	TM_DMA2DGRAPHIC_SetLayer(LCD.CurrentLayer);
	
	/* Return OK */
	return TM_LCD_Result_Ok;

}

TM_LCD_Result_t TM_LCD_SetLayer2(void) {
	/* Fill settings */
	LCD.CurrentFrameBuffer = LCD.FrameStart + LCD.FrameOffset;
	LCD.CurrentLayer = 1;
	
	/* Set layer for DMA2D also */
	TM_DMA2DGRAPHIC_SetLayer(LCD.CurrentLayer);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_SetLayer1Opacity(uint8_t opacity) {
	/* Set opacity */
	HAL_LTDC_SetAlpha(&LTDCHandle, opacity, 0);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_SetLayer2Opacity(uint8_t opacity) {
	/* Set opacity */
	HAL_LTDC_SetAlpha(&LTDCHandle, opacity, 1);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_ChangeLayers(void) {
	/* Change layers */
	if (LCD.CurrentLayer == 0) {
		TM_LCD_SetLayer2();
		TM_LCD_SetLayer1Opacity(0);
		TM_LCD_SetLayer2Opacity(255);
	} else {
		TM_LCD_SetLayer1();
		TM_LCD_SetLayer1Opacity(255);
		TM_LCD_SetLayer2Opacity(0);
	}
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_Layer2To1(void) {
	/* Copy buffer using DMA2D */
	TM_DMA2DGRAPHIC_CopyBuffer(
		(void *)(LCD.FrameStart),
		(void *)(LCD.FrameStart + LCD.FrameOffset),
		LCD.Height,
		LCD.Width,
		0,
		0
	);

	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_Layer1To2(void) {
	/* Copy buffer using DMA2D */
	TM_DMA2DGRAPHIC_CopyBuffer(
		(void *)(LCD.FrameStart),
		(void *)(LCD.FrameStart + LCD.FrameOffset),
		LCD.Width,
		LCD.Height,
		0,
		0
	);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	/* Draw line with DMA2D */
	TM_DMA2DGRAPHIC_DrawLine(x0, y0, x1, y1, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}


TM_LCD_Result_t TM_LCD_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color) {
	/* Use DMA2D for drawing */
	TM_DMA2DGRAPHIC_DrawRectangle(x0, y0, Width, Height, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color) {
	/* Check input parameters */
	if ((x0 + Width) > LCD.Width || (y0 + Height) > LCD.Height) {
		/* Return error */
		return TM_LCD_Result_Error;
	}
	
	/* Draw rectangle with DMA2D */
	TM_DMA2DGRAPHIC_DrawRoundedRectangle(x0, y0, Width, Height, r, color);

	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawFilledRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color) {
	/* Check input parameters */
	TM_DMA2DGRAPHIC_DrawFilledRoundedRectangle(x0, y0, Width, Height, r, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color) {
	/* Draw with DMA2D */
	TM_DMA2DGRAPHIC_DrawFilledRectangle(x0, y0, Width, Height, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	/* Use DMA2D */
	TM_DMA2DGRAPHIC_DrawCircle(x0, y0, r, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}

TM_LCD_Result_t TM_LCD_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	/* Use DMA2D */
	TM_DMA2DGRAPHIC_DrawFilledCircle(x0, y0, r, color);
	
	/* Return OK */
	return TM_LCD_Result_Ok;
}


uint16_t TM_LCD_GetWidth(void) {
	return LCD.CurrentWidth;
}

uint16_t TM_LCD_GetHeight(void) {
	return LCD.CurrentHeight;
}

uint32_t TM_LCD_GetFrameBuffer(void) {
	return LCD.FrameStart;
}

uint8_t TM_LCD_GetOrientation(void) {
	return LCD.Orientation;
}

/* Private functions */
static void TM_LCD_INT_InitLTDC(void) {
	RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;
	
	/* Enable LTDC and DMA2D clocks */
	__HAL_RCC_DMA2D_CLK_ENABLE();
	__HAL_RCC_LTDC_CLK_ENABLE();
	
	/* Disable LTDC */
	LTDC->GCR &= ~LTDC_GCR_LTDCEN;

    /* Timing configuration */     
    LTDCHandle.Init.HorizontalSync = LCD_HSYNC - 1;
    LTDCHandle.Init.VerticalSync = LCD_VSYNC - 1;
    LTDCHandle.Init.AccumulatedHBP = LCD_HSYNC + LCD_HBP - 1;
    LTDCHandle.Init.AccumulatedVBP = LCD_VSYNC + LCD_VBP - 1;  
    LTDCHandle.Init.AccumulatedActiveH = LCD_PIXEL_HEIGHT + LCD_VSYNC + LCD_VBP - 1;
    LTDCHandle.Init.AccumulatedActiveW = LCD_PIXEL_WIDTH + LCD_HSYNC + LCD_HBP - 1;
    LTDCHandle.Init.TotalWidth = LCD_PIXEL_WIDTH + LCD_HSYNC + LCD_HBP + LCD_HFP - 1; 
    LTDCHandle.Init.TotalHeigh = LCD_PIXEL_HEIGHT + LCD_VSYNC + LCD_VBP + LCD_VFP - 1;

    /* Configure PLLSAI prescalers for LCD */
    /* Enable Pixel Clock */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/2 = 96 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 96/4 = 24 Mhz */
	periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
	periph_clk_init_struct.PLLSAI.PLLSAIR = LCD_FREQUENCY_DIV;
	periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
	HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);

	/* Initialize the LCD pixel width and pixel height */
	LTDCHandle.LayerCfg->ImageWidth  = LCD_PIXEL_WIDTH;
	LTDCHandle.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;

	/* Background value */
	LTDCHandle.Init.Backcolor.Blue = 0;
	LTDCHandle.Init.Backcolor.Green = 0;
	LTDCHandle.Init.Backcolor.Red = 0;

	/* Polarity */
	LTDCHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	LTDCHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
	LTDCHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
	LTDCHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
	LTDCHandle.Instance = LTDC;

	/* Init LTDC */
	HAL_LTDC_Init(&LTDCHandle);
}

static void TM_LCD_INT_InitLayers(void) {
	LTDC_LayerCfgTypeDef layer_cfg;
	
	/* Layer Init */
	layer_cfg.WindowX0 = 0;
	layer_cfg.WindowX1 = LCD_PIXEL_WIDTH;
	layer_cfg.WindowY0 = 0;
	layer_cfg.WindowY1 = LCD_PIXEL_HEIGHT; 
	layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
	layer_cfg.FBStartAdress = SDRAM_START_ADR;
	layer_cfg.Alpha = 255;
	layer_cfg.Alpha0 = 0;
	layer_cfg.Backcolor.Blue = 0;
	layer_cfg.Backcolor.Green = 0;
	layer_cfg.Backcolor.Red = 0;
	layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	layer_cfg.ImageWidth = LCD_PIXEL_WIDTH;
	layer_cfg.ImageHeight = LCD_PIXEL_HEIGHT;

	/* Init layer 1 */
	HAL_LTDC_ConfigLayer(&LTDCHandle, &layer_cfg, 0);
	
	/* Config layer 2 */
	layer_cfg.Alpha = 255;
	layer_cfg.FBStartAdress = SDRAM_START_ADR + LCD_PIXEL_SIZE * LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH;

	/* Init layer 2 */
	HAL_LTDC_ConfigLayer(&LTDCHandle, &layer_cfg, 1);
}

/* ILI9341 related functions */
#if defined(LCD_USE_STM32F429_DISCOVERY)
static void TM_ILI9341_SendCommand(uint8_t data) {
	ILI9341_WRX_RESET;
	ILI9341_CS_RESET;
	TM_SPI_Send(ILI9341_SPI, data);
	ILI9341_CS_SET;
}

static void TM_ILI9341_SendData(uint8_t data) {
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;
	TM_SPI_Send(ILI9341_SPI, data);
	ILI9341_CS_SET;
}

static void TM_ILI9341_Delay(__IO uint32_t delay) {
	for (; delay != 0; delay--); 
}
#endif

static void TM_LCD_INT_InitLCD(void) {
#if defined(LCD_USE_STM32F429_DISCOVERY)
	/* CS high */
	ILI9341_CS_SET;
	
	/* Init SPI */
	TM_SPI_Init(ILI9341_SPI, ILI9341_SPI_PINS);
	
	/* Initialization sequence */
	TM_ILI9341_SendCommand(0xCA);
	TM_ILI9341_SendData(0xC3);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x50);
	TM_ILI9341_SendCommand(ILI9341_POWERB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x30);
	TM_ILI9341_SendCommand(ILI9341_POWER_SEQ);
	TM_ILI9341_SendData(0x64);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x12);
	TM_ILI9341_SendData(0x81);
	TM_ILI9341_SendCommand(ILI9341_DTCA);
	TM_ILI9341_SendData(0x85);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x78);
	TM_ILI9341_SendCommand(ILI9341_POWERA);
	TM_ILI9341_SendData(0x39);
	TM_ILI9341_SendData(0x2C);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x34);
	TM_ILI9341_SendData(0x02);
	TM_ILI9341_SendCommand(ILI9341_PRC);
	TM_ILI9341_SendData(0x20);
	TM_ILI9341_SendCommand(ILI9341_DTCB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_FRC);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x1B);
	TM_ILI9341_SendCommand(ILI9341_DFC);
	TM_ILI9341_SendData(0x0A);
	TM_ILI9341_SendData(0xA2);
	TM_ILI9341_SendCommand(ILI9341_POWER1);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendCommand(ILI9341_POWER2);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendCommand(ILI9341_VCOM1);
	TM_ILI9341_SendData(0x45);
	TM_ILI9341_SendData(0x15);
	TM_ILI9341_SendCommand(ILI9341_VCOM2);
	TM_ILI9341_SendData(0x90);
	TM_ILI9341_SendCommand(ILI9341_MAC);
	TM_ILI9341_SendData(0xC8);
	TM_ILI9341_SendCommand(ILI9341_3GAMMA_EN);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_RGB_INTERFACE);
	TM_ILI9341_SendData(0xC2);
	TM_ILI9341_SendCommand(ILI9341_DFC);
	TM_ILI9341_SendData(0x0A);
	TM_ILI9341_SendData(0xA7);
	TM_ILI9341_SendData(0x27);
	TM_ILI9341_SendData(0x04);

	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xEF);

	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendData(0x3F);
	TM_ILI9341_SendCommand(ILI9341_INTERFACE);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x06);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
	TM_ILI9341_Delay(1000000);

	TM_ILI9341_SendCommand(ILI9341_GAMMA);
	TM_ILI9341_SendData(0x01);

	TM_ILI9341_SendCommand(ILI9341_PGAMMA);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x29);
	TM_ILI9341_SendData(0x24);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x09);
	TM_ILI9341_SendData(0x4E);
	TM_ILI9341_SendData(0x78);
	TM_ILI9341_SendData(0x3C);
	TM_ILI9341_SendData(0x09);
	TM_ILI9341_SendData(0x13);
	TM_ILI9341_SendData(0x05);
	TM_ILI9341_SendData(0x17);
	TM_ILI9341_SendData(0x11);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_NGAMMA);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x16);
	TM_ILI9341_SendData(0x1B);
	TM_ILI9341_SendData(0x04);
	TM_ILI9341_SendData(0x11);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x33);
	TM_ILI9341_SendData(0x42);
	TM_ILI9341_SendData(0x05);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x0A);
	TM_ILI9341_SendData(0x28);
	TM_ILI9341_SendData(0x2F);
	TM_ILI9341_SendData(0x0F);

	TM_ILI9341_SendCommand(ILI9341_SLEEP_OUT);
	TM_ILI9341_Delay(1000000);
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
#endif
}

static void TM_LCD_INT_InitPins(void) {
#if defined(LCD_USE_STM32F7_DISCOVERY)
	/* Init GPIO pins for LTDC */
	TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_4, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOI, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOJ, GPIO_PIN_All & ~(GPIO_PIN_12), TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOK, 0x00FF & ~(GPIO_PIN_3), TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, GPIO_AF14_LTDC);
	
	/* Init pins for LCD control */
	/* Display enable */
	TM_GPIO_Init(GPIOI, GPIO_PIN_12, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Low);
	/* Backlight control */
	TM_GPIO_Init(GPIOK, GPIO_PIN_3, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Low);
#elif defined(LCD_USE_STM32F439_EVAL)
	/* LCD pins */
	TM_GPIO_InitAlternate(GPIOI, 0xF000, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, 0x0E);
	TM_GPIO_InitAlternate(GPIOJ, 0xFFFF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, 0x0E);
	TM_GPIO_InitAlternate(GPIOK, 0x00FF, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast, 0x0E);	
#elif defined(LCD_USE_STM32F429_DISCOVERY)
	TM_GPIO_Init(ILI9341_WRX_PORT, ILI9341_WRX_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(ILI9341_CS_PORT, ILI9341_CS_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Medium);
	TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_11 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF9_LTDC);
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_3 | GPIO_PIN_6, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_10, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF14_LTDC);
	TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_10 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF9_LTDC);
#endif
}
