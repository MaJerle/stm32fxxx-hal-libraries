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
#include "tm_stm32_dma2d_graphic.h"

/* Absolute number */
#define ABS(X)	((X) > 0 ? (X) : -(X))  

/* Color used for transfer in ARGB8888 format */
uint32_t DMA2D_Color = 0x00FF0000;
uint32_t DMA2D_Width;
uint32_t DMA2D_Height;
uint32_t DMA2D_StartAddress;

/* Convert function */
static void DMA2D_Convert565ToARGB8888(uint16_t color) {
	/* Input color: RRRRR GGGGGG BBBBB */
	/* Output color: RRRRR000 GGGGGG00 BBBBB000 */
	DMA2D_Color = 0;
	
	DMA2D_Color |= (color & 0xF800) << 8;
	DMA2D_Color |= (color & 0x07E0) << 5;
	DMA2D_Color |= (color & 0x001F) << 3;
}

/* Internal structure */
typedef struct {
	uint16_t Width;
	uint16_t Height;
	uint16_t CurrentWidth;
	uint16_t CurrentHeight;
	uint32_t StartAddress;
	uint32_t LayerOffset;
	uint8_t LayerNumber;
	uint32_t Offset;
	uint32_t Pixels;
	uint8_t Initialized;
	uint8_t Orientation;
	uint8_t PixelSize;
} TM_INT_DMA2D_t;

/* Private structures */
static DMA2D_HandleTypeDef DMA2DHandle;
//static DMA2D_FG_InitTypeDef GRAPHIC_DMA2D_FG_InitStruct;
volatile TM_INT_DMA2D_t DIS;

__STATIC_INLINE void DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
	TM_DMA2DGRAPHIC_DrawHorizontalLine(x, y, 1, color);
}

/* Private functions */
void TM_INT_DMA2DGRAPHIC_InitAndTransfer(void);
void TM_INT_DMA2DGRAPHIC_SetMemory(uint32_t MemoryAddress, uint32_t Offset, uint32_t NumberOfLine, uint32_t PixelPerLine);
void TM_INT_DMA2DGRAPHIC_DrawCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color);
void TM_INT_DMA2DGRAPHIC_DrawFilledCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color);

void TM_DMA2DGRAPHIC_Init(void) {
	/* Internal settings */
	DIS.StartAddress = DMA2D_GRAPHIC_RAM_ADDR;
	DIS.Offset = 0;
	DIS.Width = DMA2D_GRAPHIC_LCD_WIDTH;
	DIS.Height = DMA2D_GRAPHIC_LCD_HEIGHT;
	DIS.Pixels = DMA2D_GRAPHIC_PIXELS;
	DIS.CurrentHeight = DMA2D_GRAPHIC_LCD_WIDTH;
	DIS.CurrentWidth = DMA2D_GRAPHIC_LCD_HEIGHT;
	DIS.Orientation = 0;
	DIS.PixelSize = 2;
	DIS.LayerOffset = DMA2D_GRAPHIC_LCD_WIDTH * DMA2D_GRAPHIC_LCD_HEIGHT * DIS.PixelSize;
	DIS.LayerNumber = 0;
	
	/* Set DMA2D instance value */
	DMA2DHandle.Instance = DMA2D;
	
	/* Enable DMA2D clock */
	__HAL_RCC_DMA2D_CLK_ENABLE();
	
	/* Initialized */
	DIS.Initialized = 1;
}

void TM_DMA2DGRAPHIC_SetLayer(uint8_t layer_number) {
	/* Set offset */
	DIS.Offset = (layer_number) * DIS.LayerOffset;
	DIS.LayerNumber = layer_number;
}

void TM_DMA2DGRAPHIC_DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
	if (DIS.Orientation == 1) { /* Normal */
		*(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * (y * DIS.Width + x)) = color;
	} else if (DIS.Orientation == 0) { /* 180 */
		*(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * ((DIS.Height - y - 1) * DIS.Width + (DIS.Width - x - 1))) = color;
	} else if (DIS.Orientation == 3) { /* 90 */ /* x + width * y */
		*(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * ((x) * DIS.Width + DIS.Width - y - 1)) = color;
	} else if (DIS.Orientation == 2) { /* 270 */
		*(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * ((DIS.Height - x - 1) * DIS.Width + y)) = color;
	}
}

uint32_t TM_DMA2DGRAPHIC_GetPixel(uint16_t x, uint16_t y) {
	if (DIS.Orientation == 1) { /* Normal */
		return *(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * (y * DIS.Width + x));
	} else if (DIS.Orientation == 0) { /* 180 */
		return *(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * ((DIS.Height - y - 1) * DIS.Width + (DIS.Width - x - 1)));
	} else if (DIS.Orientation == 3) { /* 90 */ /* x + width * y */
		return *(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * ((x) * DIS.Width + DIS.Width - y - 1));
	} else if (DIS.Orientation == 2) { /* 270 */
		return *(__IO uint16_t *) (DIS.StartAddress + DIS.Offset + DIS.PixelSize * ((DIS.Height - x - 1) * DIS.Width + y));
	}
	return 0;
}

void TM_DMA2DGRAPHIC_SetOrientation(uint8_t orientation) {
	/* Filter */
	if (orientation > 3) {
		return;
	}
	
	/* Save new orientation */
	DIS.Orientation = orientation;
	
	if (
		orientation == 0 ||
		orientation == 1
	) {
		DIS.CurrentHeight = DIS.Height;
		DIS.CurrentWidth = DIS.Width;
	} else {
		DIS.CurrentHeight = DIS.Width;
		DIS.CurrentWidth = DIS.Height;
	}
}

void TM_DMA2DGRAPHIC_Fill(uint32_t color) {
	/* Set parameters */
	DMA2DHandle.Init.Mode = DMA2D_R2M;
	DMA2DHandle.Init.ColorMode = DMA2D_RGB565;
	DMA2DHandle.Init.OutputOffset = 0;

	/* Convert color */
	DMA2D_Convert565ToARGB8888(color);

	/* Init DMA2D and start transfer */
	if (HAL_DMA2D_Init(&DMA2DHandle) == HAL_OK) {
		if (HAL_DMA2D_ConfigLayer(&DMA2DHandle, DIS.LayerNumber) == HAL_OK) {
			if (HAL_DMA2D_Start(&DMA2DHandle, DMA2D_Color, (uint32_t)(DIS.StartAddress + DIS.Offset), DIS.Width, DIS.Height) == HAL_OK) {        
				HAL_DMA2D_PollForTransfer(&DMA2DHandle, 20);
			}
		}
	}
}

void TM_DMA2DGRAPHIC_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	/* Check if initialized */
	if (DIS.Initialized != 1) {
		return;
	}
	
	/* Filter */
	if (
		x >= DIS.CurrentWidth ||
		y >= DIS.CurrentHeight
	) {
		return;
	}
	
	/* Filter height */
	if ((y + height) >= DIS.CurrentHeight) {
		height = DIS.CurrentHeight - y;
	}
	/* Filter width */
	if ((x + width) >= DIS.CurrentWidth) {
		width = DIS.CurrentWidth - x;
	}
	
	/* Set colors */
	/* Convert color */
	DMA2D_Convert565ToARGB8888(color);
	
	/* Set memory settings */
	if (DIS.Orientation == 1) { /* Normal */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (y * DIS.Width + x), DIS.Width - width, height, width);
	} 
	if (DIS.Orientation == 0) { /* 180 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * ((DIS.Height - height - y) * DIS.Width + DIS.Width - x - width), DIS.Width - width, height, width);
	} 
	if (DIS.Orientation == 3) { /* 90 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (DIS.Width - y - height + DIS.Width * x), DIS.Width - height, width, height);
	} 
	if (DIS.Orientation == 2) { /* 270 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (x + DIS.Width * (DIS.Height - width - x)), DIS.Width - height, width, height);
	}
	
	/* Start transfer and wait till done */
	TM_INT_DMA2DGRAPHIC_InitAndTransfer();
}

void TM_DMA2DGRAPHIC_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	/* Draw 2 vertical lines */
	TM_DMA2DGRAPHIC_DrawVerticalLine(x, y, height, color);
	TM_DMA2DGRAPHIC_DrawVerticalLine(x + width - 1, y, height, color);
	
	/* Draw 2 horizontal lines */
	TM_DMA2DGRAPHIC_DrawHorizontalLine(x, y, width, color);
	TM_DMA2DGRAPHIC_DrawHorizontalLine(x, y + height, width, color);
}

void TM_DMA2DGRAPHIC_DrawRoundedRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r, uint32_t color) {
	/* Check input parameters */
	if (width == 0 || height == 0) {
		return;
	}
	
	/* Check max radius */
	if (r > (width / 2)) {
		r = width / 2 - 1;
	}
	if (r > (height / 2)) {
		r = height / 2 - 1;
	}
	if (
		r > (width / 2) || 
		r > (height / 2)
	) {
		r = 0;
	}
	
	/* No radius */
	if (r == 0) {
		/* Draw normal rectangle */
		TM_DMA2DGRAPHIC_DrawRectangle(x, y, width, height, color);
		
		/* Return from function */
		return;
	}
	
	/* Draw lines */
	TM_DMA2DGRAPHIC_DrawHorizontalLine(x + r, y, width - 2 * r, color); /* Top */
	TM_DMA2DGRAPHIC_DrawHorizontalLine(x + r, y + height - 1, width - 2 * r, color); /* Bottom */
	
	TM_DMA2DGRAPHIC_DrawVerticalLine(x, y + r, height - 2 * r, color); /* Right */
	TM_DMA2DGRAPHIC_DrawVerticalLine(x + width - 1, y + r, height - 2 * r, color); /* Left */
	
	/* Draw corners */
	TM_INT_DMA2DGRAPHIC_DrawCircleCorner(x + r, y + r, r, 0x01, color); /* Top left */
	TM_INT_DMA2DGRAPHIC_DrawCircleCorner(x + width - r - 1, y + r, r, 0x02, color); /* Top right */
	TM_INT_DMA2DGRAPHIC_DrawCircleCorner(x + width - r - 1, y + height - r - 1, r, 0x04, color); /* Bottom right */
	TM_INT_DMA2DGRAPHIC_DrawCircleCorner(x + r, y + height - r - 1, r, 0x08, color); /* Bottom left */
}

void TM_DMA2DGRAPHIC_DrawFilledRoundedRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r, uint32_t color) {
	/* Check input parameters */
	if (width == 0 || width == 0) {
		return;
	}
	
	/* Check max radius */
	if (r > (width / 2)) {
		r = width / 2 - 1;
	}
	if (r > (height / 2)) {
		r = height / 2 - 1;
	}
	if (
		r > (width / 2) || 
		r > (height / 2)
	) {
		r = 0;
	}
	
	/* No radius */
	if (r == 0) {
		TM_DMA2DGRAPHIC_DrawFilledRectangle(x, y, width, height, color);
		return;
	}
	
	/* Draw rectangles */
	TM_DMA2DGRAPHIC_DrawFilledRectangle(x + r, y, width - 2 * r, height, color);
	TM_DMA2DGRAPHIC_DrawFilledRectangle(x, y + r, r, height - 2 * r, color);
	TM_DMA2DGRAPHIC_DrawFilledRectangle(x + width - r, y + r, r, height - 2 * r, color);
	
	/* Draw corners */
	TM_INT_DMA2DGRAPHIC_DrawFilledCircleCorner(x + r, y + r, r, 0x01, color); /* Top left */
	TM_INT_DMA2DGRAPHIC_DrawFilledCircleCorner(x + width - r - 1, y + r, r, 0x02, color); /* Top right */
	TM_INT_DMA2DGRAPHIC_DrawFilledCircleCorner(x + width - r - 1, y + height - r - 1, r, 0x04, color); /* Bottom right */
	TM_INT_DMA2DGRAPHIC_DrawFilledCircleCorner(x + r, y + height - r - 1, r, 0x08, color); /* Bottom left */
}

void TM_DMA2DGRAPHIC_DrawVerticalLine(int16_t x, int16_t y, uint16_t length, uint32_t color) {
	/* Check if initialized */
	if (DIS.Initialized != 1) {
		return;
	}
	
	/* Filter */
	if (
		x >= DIS.CurrentWidth ||
		y >= DIS.CurrentHeight ||
		length == 0
	) {
		return;
	}
	
	if (x < 0) {
		x = 0;
	}
	
	if (y < 0) {
		y = 0;
	}
	
	/* Filter height */
	if ((y + length) >= DIS.CurrentHeight) {
		length = DIS.CurrentHeight - y;
	}

	/* Set color */
	DMA2D_Convert565ToARGB8888(color);
	
	/* Set memory settings */
	if (DIS.Orientation == 1) { /* Normal */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (y * DIS.Width + x), DIS.Width - 1, length, 1);
	} 
	if (DIS.Orientation == 0) { /* 180 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * ((DIS.Height - length - y) * DIS.Width + DIS.Width - x - 1), DIS.Width - 1, length, 1);
	} 
	if (DIS.Orientation == 3) { /* 90 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (DIS.Width - y - length + DIS.Width * x), DIS.Width - length, 1, length);
	} 
	if (DIS.Orientation == 2) { /* 270 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (y + DIS.Width * (DIS.Height - 1 - x)), DIS.Width - length, 1, length);
	}
	
	/* Start transfer and wait till done */
	TM_INT_DMA2DGRAPHIC_InitAndTransfer();
}

void TM_DMA2DGRAPHIC_DrawHorizontalLine(int16_t x, int16_t y, uint16_t length, uint32_t color) {
	/* Check if initialized */
	if (DIS.Initialized != 1) {
		return;
	}
	
	/* Filter */
	if (
		x >= DIS.CurrentWidth ||
		y >= DIS.CurrentHeight ||
		length == 0
	) {
		return;
	}
	
	if (x < 0) {
		length += x;
		x = 0;
	}
	
	if (y < 0) {
		y = 0;
	}
	
	/* Filter height */
	if ((x + length) >= DIS.CurrentWidth) {
		length = DIS.CurrentWidth - x;
	}

	/* Set color */
	DMA2D_Convert565ToARGB8888(color);
	
	/* Set memory settings */
	if (DIS.Orientation == 1) { /* Normal */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (y * DIS.Width + x), DIS.Width - length, 1, length);
	} 
	if (DIS.Orientation == 0) { /* 180 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * ((DIS.Height - 1 - y) * DIS.Width + DIS.Width - x - length), DIS.Width - length, 1, length);
	} 
	if (DIS.Orientation == 3) { /* 90 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (DIS.Width - y - 1 + DIS.Width * x), DIS.Width - 1, length, 1);
	} 
	if (DIS.Orientation == 2) { /* 270 */
		TM_INT_DMA2DGRAPHIC_SetMemory(DIS.PixelSize * (y + DIS.Width * (DIS.Height - length - x)), DIS.Width - 1, length, 1);
	}
	
	/* Start transfer and wait till done */
	TM_INT_DMA2DGRAPHIC_InitAndTransfer();
}

void TM_DMA2DGRAPHIC_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;
	
	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		DrawPixel(x, y, color);
		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void TM_DMA2DGRAPHIC_DrawPolyLine(TM_DMA2DRAPHIC_Poly_t* Coordinates, uint16_t count, uint32_t color) {
	uint16_t X, Y;
	
	while (--count) {
		X = Coordinates->X;
		Y = Coordinates->Y;
		Coordinates++;
		TM_DMA2DGRAPHIC_DrawLine(X, Y, Coordinates->X, Coordinates->Y, color);
	};
}

void TM_DMA2DGRAPHIC_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    TM_DMA2DGRAPHIC_DrawVerticalLine(x0, y0 + r, 1, color);
    TM_DMA2DGRAPHIC_DrawVerticalLine(x0, y0 - r, 1, color);
    TM_DMA2DGRAPHIC_DrawVerticalLine(x0 + r, y0, 1, color);
    TM_DMA2DGRAPHIC_DrawVerticalLine(x0 - r, y0, 1, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 + x, y0 + y, 1, color);
        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 - x, y0 + y, 1, color);
        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 + x, y0 - y, 1, color);
        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 - x, y0 - y, 1, color);

        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 + y, y0 + x, 1, color);
        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 - y, y0 + x, 1, color);
        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 + y, y0 - x, 1, color);
        TM_DMA2DGRAPHIC_DrawVerticalLine(x0 - y, y0 - x, 1, color);
    }
}

void TM_DMA2DGRAPHIC_DrawFilledCircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    TM_DMA2DGRAPHIC_DrawVerticalLine(x0, y0 + r, 1, color);
    TM_DMA2DGRAPHIC_DrawVerticalLine(x0, y0 - r, 1, color);
    TM_DMA2DGRAPHIC_DrawVerticalLine(x0 + r, y0, 1, color);
    TM_DMA2DGRAPHIC_DrawVerticalLine(x0 - r, y0, 1, color);
    TM_DMA2DGRAPHIC_DrawHorizontalLine(x0 - r, y0, 2 * r, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        TM_DMA2DGRAPHIC_DrawHorizontalLine(x0 - x, y0 + y, 2 * x, color);
		TM_DMA2DGRAPHIC_DrawHorizontalLine(x0 - x, y0 - y, 2 * x, color);

        TM_DMA2DGRAPHIC_DrawHorizontalLine(x0 - y, y0 + x, 2 * y, color);
        TM_DMA2DGRAPHIC_DrawHorizontalLine(x0 - y, y0 - x, 2 * y, color);
    }
}

void TM_DMA2DGRAPHIC_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint32_t color) {
	/* Draw lines */
	TM_DMA2DGRAPHIC_DrawLine(x1, y1, x2, y2, color);
	TM_DMA2DGRAPHIC_DrawLine(x2, y2, x3, y3, color);
	TM_DMA2DGRAPHIC_DrawLine(x3, y3, x1, y1, color);
}


void TM_DMA2DGRAPHIC_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint32_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		TM_DMA2DGRAPHIC_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void TM_DMA2DGRAPHIC_CopyBuffer(void* pSrc, void* pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLineSrc, uint32_t OffLineDst) {
	/* Copy buffer using interrupt */
	TM_DMA2DGRAPHIC_CopyBufferIT(pSrc, pDst, xSize, ySize, OffLineSrc, OffLineDst);

	/* Wait until transfer is done */
	DMA2D_WAIT;
}


void TM_DMA2DGRAPHIC_CopyBufferIT(void* pSrc, void* pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLineSrc, uint32_t OffLineDst) {
	/* Wait for previous operation to be done */
	DMA2D_WAIT;
	
	/* DeInit DMA2D */
	RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2DRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2DRST;
	
	DMA2D->CR = 0x00000000UL | (1 << 9);
	
	/* Set up pointers */
	DMA2D->FGMAR = (uint32_t)pSrc;                       
	DMA2D->OMAR = (uint32_t)pDst;                       
	DMA2D->FGOR = OffLineSrc;                      
	DMA2D->OOR = OffLineDst; 

	/* Set up pixel format */  
	DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;

	/* Set up size */
	DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; 

	/* Start DMA2D */
	DMA2D->CR |= DMA2D_CR_START; 
}

/* Private functions */
void TM_INT_DMA2DGRAPHIC_SetConf(TM_DMA2DGRAPHIC_INT_Conf_t* Conf) {
	/* Fill settings for DMA2D */
	DIS.Width = Conf->Width;
	DIS.Height = Conf->Height;
	DIS.StartAddress = Conf->BufferStart;
	DIS.LayerOffset = Conf->BufferOffset;
	DIS.PixelSize = Conf->BytesPerPixel;
	DIS.Pixels = DIS.Width * DIS.Height;
	DIS.Orientation = Conf->Orientation;
	DIS.CurrentWidth = DIS.Width;
	DIS.CurrentHeight = DIS.Height;
	
	/* Set DMA2D orientation */
	TM_DMA2DGRAPHIC_SetOrientation(DIS.Orientation);
}

void TM_INT_DMA2DGRAPHIC_InitAndTransfer(void) {
	/* Wait until transfer is done first from other calls */
	DMA2D_WAIT;
	
	/* Format DMA2D settings */
	DMA2DHandle.Init.Mode = DMA2D_R2M;
	DMA2DHandle.Init.ColorMode = DMA2D_RGB565;
	DMA2DHandle.Init.OutputOffset = DIS.Width - DMA2D_Width;

	/* DMA2D Initialization */
	if (HAL_DMA2D_Init(&DMA2DHandle) == HAL_OK) {
		if (HAL_DMA2D_ConfigLayer(&DMA2DHandle, 0) == HAL_OK) {
			if (HAL_DMA2D_Start(&DMA2DHandle, DMA2D_Color, (uint32_t)DMA2D_StartAddress, DMA2D_Width, DMA2D_Height) == HAL_OK) {        
				HAL_DMA2D_PollForTransfer(&DMA2DHandle, 100);
			}
		}
	}
	
	/* Wait till transfer ends */
	DMA2D_WAIT;
}

void TM_INT_DMA2DGRAPHIC_SetMemory(uint32_t MemoryAddress, uint32_t Offset, uint32_t NumberOfLine, uint32_t PixelPerLine) {	
	/* Set memory settings */
	DMA2DHandle.Init.OutputOffset = 0;
	DMA2D_StartAddress = DIS.StartAddress + DIS.Offset + MemoryAddress;
	DMA2D_Width = PixelPerLine;
	DMA2D_Height = NumberOfLine;
}

void TM_INT_DMA2DGRAPHIC_DrawCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (corner & 0x01) {/* Top left */	
			TM_DMA2DGRAPHIC_DrawPixel(x0 - y, y0 - x, color);
			TM_DMA2DGRAPHIC_DrawPixel(x0 - x, y0 - y, color);
		}
		
        if (corner & 0x02) {/* Top right */
			TM_DMA2DGRAPHIC_DrawPixel(x0 + x, y0 - y, color);
			TM_DMA2DGRAPHIC_DrawPixel(x0 + y, y0 - x, color);
		}
		
		if (corner & 0x04) {/* Bottom right */
			TM_DMA2DGRAPHIC_DrawPixel(x0 + x, y0 + y, color);
			TM_DMA2DGRAPHIC_DrawPixel(x0 + y, y0 + x, color);
		}
		
        if (corner & 0x08) {/* Bottom left */	
			TM_DMA2DGRAPHIC_DrawPixel(x0 - x, y0 + y, color);
			TM_DMA2DGRAPHIC_DrawPixel(x0 - y, y0 + x, color);
		}
    }
}

void TM_INT_DMA2DGRAPHIC_DrawFilledCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (corner & 0x01) {/* Top left */
			TM_DMA2DGRAPHIC_DrawLine(x0, y0 - y, x0 - x, y0 - y, color);
			TM_DMA2DGRAPHIC_DrawLine(x0, y0 - x, x0 - y, y0 - x, color);
		}
		
        if (corner & 0x02) {/* Top right */
			TM_DMA2DGRAPHIC_DrawLine(x0 + x, y0 - y, x0, y0 - y, color);
			TM_DMA2DGRAPHIC_DrawLine(x0 + y, y0 - x, x0, y0 - x, color);
		}
		
		if (corner & 0x04) {/* Bottom right */
			TM_DMA2DGRAPHIC_DrawLine(x0, y0 + y, x0 + x, y0 + y, color);
			TM_DMA2DGRAPHIC_DrawLine(x0 + y, y0 + x, x0, y0 + x, color);
		}
		
        if (corner & 0x08) {/* Bottom left */
			TM_DMA2DGRAPHIC_DrawLine(x0 - x, y0 + y, x0, y0 + y, color);
			TM_DMA2DGRAPHIC_DrawLine(x0, y0 + x, x0 - y, y0 + x, color);
		}
    }
}
