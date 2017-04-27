/**
 * Keil project example for LCD 
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.net
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
#include "tm_stm32_lcd.h"
#include "tm_stm32_general.h"

/* Font size structure */
TM_FONT_SIZE_t FontSize;

/* String to draw on LCD */
#if defined(STM32F7_DISCOVERY)
char str[] = "This is LCD driver for F7-Discovery board";
#elif defined(STM32F439_EVAL)
char str[] = "This is LCD driver for F439-Eval board";
#elif defined(STM32F429_DISCOVERY)
char str[] = "F429-Discovery board";
#endif

uint16_t op_back = 0x00, op_front = 0x00;

uint8_t layer = 1;
void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    DMA2D_HandleTypeDef hDma2dHandler;
    uint32_t layer3 = (uint32_t)(LCD_FRAME_BUFFER_3 + LCD_PIXEL_SIZE * (LCD_PIXEL_WIDTH * y + x));
    uint32_t layer1 = (uint32_t)(LCD_FRAME_BUFFER + (layer ? 0 : 1) * LCD_BUFFER_OFFSET + LCD_PIXEL_SIZE * (LCD_PIXEL_WIDTH * y + x));
    
    uint8_t o1;
   
    o1 = (color >> 24) & 0xFF;
    
    hDma2dHandler.Instance = DMA2D;
    
    /* Register to memory mode with ARGB8888 as color Mode */ 
    hDma2dHandler.Init.Mode         = DMA2D_R2M;
    hDma2dHandler.Init.ColorMode    = DMA2D_ARGB8888;
    hDma2dHandler.Init.OutputOffset = LCD_PIXEL_WIDTH - w;

    /* Transfer data to memory */
    if (HAL_DMA2D_Init(&hDma2dHandler) == HAL_OK)  {
        if (HAL_DMA2D_Start(&hDma2dHandler, color, o1 < 0xFF ? layer3 : layer1, w, h) == HAL_OK) {
            HAL_DMA2D_PollForTransfer(&hDma2dHandler, 10);
        }
    }

    /* In case color is transparent, we need blending */
    if (o1 < 0xFF) {
        /* Foreground layer Configuration */
        hDma2dHandler.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
        hDma2dHandler.LayerCfg[1].InputAlpha = o1;
        hDma2dHandler.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
        hDma2dHandler.LayerCfg[1].InputOffset = LCD_PIXEL_WIDTH - w;
        
        /* Background layer Configuration */
        hDma2dHandler.LayerCfg[0].AlphaMode = DMA2D_REPLACE_ALPHA;
        hDma2dHandler.LayerCfg[0].InputAlpha = 0xFF;
        hDma2dHandler.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;
        hDma2dHandler.LayerCfg[0].InputOffset = LCD_PIXEL_WIDTH - w;

        /* DMA2D Initialization */
        HAL_DMA2D_Init(&hDma2dHandler);
        HAL_DMA2D_ConfigLayer(&hDma2dHandler, 1);
        HAL_DMA2D_ConfigLayer(&hDma2dHandler, 0);
        
        /* Register to memory mode with ARGB8888 as color Mode */ 
        hDma2dHandler.Init.Mode         = DMA2D_M2M_BLEND;
        hDma2dHandler.Init.ColorMode    = DMA2D_ARGB8888;
        hDma2dHandler.Init.OutputOffset = (LCD_PIXEL_WIDTH - w);

        if (HAL_DMA2D_Init(&hDma2dHandler) == HAL_OK)  {
            if (HAL_DMA2D_BlendingStart(&hDma2dHandler, 
                    layer3,
                    layer1,
                    layer1,
                    w, 
                    h
                ) == HAL_OK) {
                HAL_DMA2D_PollForTransfer(&hDma2dHandler, 100);
            }
        }
    }
}

uint32_t pixel;
volatile uint32_t change_layers = 0;
int main(void) {
    int16_t x = 0, y = 0;
    int16_t xdir = 1, ydir = 1;
    
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
    
    TM_DISCO_ButtonInit();
    
    /* Init delay functions */
    TM_DELAY_Init();
    
    TM_GENERAL_DWTCounterEnable();
	
	/* Init LCD */
	TM_LCD_Init();
	
	/* Fill LCD with color */
	TM_LCD_Fill(LCD_COLOR_BLUE);
	
	/* Put string on the middle of LCD */
	TM_LCD_SetFont(&TM_Font_11x18);
	
	/* Get width and height of string */
	TM_FONT_GetStringSize(str, &FontSize, &TM_Font_11x18);
	
	/* Calculate middle of LCD */
	TM_LCD_SetXY((TM_LCD_GetWidth() - FontSize.Width) / 2, (TM_LCD_GetHeight() - FontSize.Height) / 2);
	
	/* Put string to LCD */
	TM_LCD_Puts(str);
    
//    TM_LCD_Fill(LCD_COLOR_BLACK);
//    DrawRectangle(0,   0,   300, 180, 0xFFFF0000);
//    DrawRectangle(180, 10,  290, 190, 0x2F00FF00);
//    DrawRectangle(90,  150, 300, 120, 0x800000FF);
//    
//    pixel = *(uint32_t *)(LCD_FRAME_BUFFER + LCD_PIXEL_SIZE * (LCD_PIXEL_WIDTH * 155 + 95));
//	
//    TM_LCD_ChangeLayers();
//    
//    layer = !layer;
//    TM_LCD_Fill(LCD_COLOR_BLACK);
//    DrawRectangle(0,   0,   300, 180, 0xFFFFFF00);
//    DrawRectangle(180, 10,  290, 190, 0x2F0FFF00);
//    DrawRectangle(90,  150, 300, 120, 0x800060FF);
    
    layer = 0;
	while (1) {
        uint32_t start = DWT->CYCCNT;
        DrawRectangle(0, 0, 480, 272, 0xFF00FF00);
        DrawRectangle(x, y,  30, 30,  0xFF0000FF);
        
        x += xdir;
        y += ydir;
        
        if (xdir == 1) {
            if (x > (TM_LCD_GetWidth() - 30)) {
                x -= 2;
                xdir = -1;
            }
        } else {
            if (x < 0) {
                x += 2;
                xdir = 1;
            }
        }
        
        if (ydir == 1) {
            if (y > (TM_LCD_GetHeight() - 30)) {
                y -= 2;
                ydir = -1;
            }
        } else {
            if (y < 0) {
                y += 2;
                ydir = 1;
            }
        }
            
        change_layers = 1;
        while (change_layers);
	}
}

LTDC_HandleTypeDef hLTDC;
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef* hLtdc) {
    if (change_layers) {
        change_layers = 0;
        TM_LCD_ChangeLayers();
        layer = !layer;
    }
    hLTDC.Instance = LTDC;
    HAL_LTDC_ProgramLineEvent(&hLTDC, 0);
}
