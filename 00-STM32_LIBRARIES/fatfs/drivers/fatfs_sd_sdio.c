/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "fatfs_sd_sdio.h"

static uint8_t SDCARD_IsDetected(void);

/* Status of SDCARD */
static volatile DSTATUS Stat = STA_NOINIT;
static SD_HandleTypeDef uSdHandle;

/**
  * @}
  */ 
  
/** @defgroup STM32746G_DISCOVERY_SD_Private_FunctionPrototypes STM32746G_DISCOVERY_SD Private Function Prototypes
  * @{
  */
/**
  * @}
  */ 
  
/** @defgroup STM32746G_DISCOVERY_SD_Exported_Functions STM32746G_DISCOVERY_SD Exported Functions
  * @{
  */

/**
  * @brief  Initializes the SD card device.
  * @retval SD status
  */
uint8_t BSP_SD_Init(void)
{ 
	uint8_t SD_state = MSD_OK;

	/* uSD device interface configuration */
#if defined(SDIO)
	uSdHandle.Instance = SDIO;

	uSdHandle.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
	uSdHandle.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
	uSdHandle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
	uSdHandle.Init.BusWide             = SDIO_BUS_WIDE_1B;
	uSdHandle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	uSdHandle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;
#elif defined(SDMMC1)
	uSdHandle.Instance = SDMMC1;

	uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
	uSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
	uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
	uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
	uSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;	
#else
#error "NOT SUPPORTED!"
#endif

	/* Init GPIO, DMA and NVIC */
	BSP_SD_MspInit(&uSdHandle, NULL);

	/* Check if the SD card is plugged in the slot */
	if (SDCARD_IsDetected() != SD_PRESENT) {
		return MSD_ERROR;
	}

	/* HAL SD initialization */
	if (HAL_SD_Init(&uSdHandle) != HAL_OK) {
		SD_state = MSD_ERROR;
	}

	/* Configure SD Bus width */
	if (SD_state == MSD_OK) {
		/* Enable wide operation */
#if defined(SDIO_BUS_WIDE_4B)
#if FATFS_SDIO_4BIT == 1
		if (HAL_SD_ConfigWideBusOperation(&uSdHandle, SDIO_BUS_WIDE_4B) != HAL_OK) {
#else
		if (HAL_SD_ConfigWideBusOperation(&uSdHandle, SDIO_BUS_WIDE_1B) != HAL_OK) {	
#endif
#else
#if FATFS_SDIO_4BIT == 1
		if (HAL_SD_ConfigWideBusOperation(&uSdHandle, SDMMC_BUS_WIDE_4B) != HAL_OK) {
#else
		if (HAL_SD_ConfigWideBusOperation(&uSdHandle, SDMMC_BUS_WIDE_1B) != HAL_OK) {	
#endif
#endif
			SD_state = MSD_ERROR;
		} else {
			SD_state = MSD_OK;
		}
	}

	return  SD_state;
}

/**
  * @brief  DeInitializes the SD card device.
  * @retval SD status
  */
uint8_t BSP_SD_DeInit(void) { 
    uint8_t sd_state = MSD_OK;
#if defined(SDMMC1)
    uSdHandle.Instance = SDMMC1;
#else
    uSdHandle.Instance = SDIO;
#endif

    /* HAL SD deinitialization */
    if(HAL_SD_DeInit(&uSdHandle) != HAL_OK) {
        sd_state = MSD_ERROR;
    }

    /* Msp SD deinitialization */
#if defined(SDMMC1)
    uSdHandle.Instance = SDMMC1;
#else
    uSdHandle.Instance = SDIO;
#endif
    BSP_SD_MspDeInit(&uSdHandle, NULL);

    return  sd_state;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t sector, uint32_t NumOfBlocks, uint32_t Timeout) {
    if (HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, sector, NumOfBlocks, Timeout) != HAL_OK) {
        return MSD_ERROR;
    } else {
        return MSD_OK;
    }
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t sector, uint32_t NumOfBlocks, uint32_t Timeout) {
    if (HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, sector, NumOfBlocks, Timeout) != HAL_OK) {
        return MSD_ERROR;
    } else {
        return MSD_OK;
    }
}

/**
  * @brief  Erases the specified memory area of the given SD card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr) {
    if (HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != HAL_OK) {
        return MSD_ERROR;
    } else {
        return MSD_OK;
    }
}

/**
  * @brief  Initializes the SD MSP.
  * @param  hsd: SD handle
  * @param  Params
  * @retval None
  */
__weak void BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params) {
    static DMA_HandleTypeDef dma_rx_handle;
    static DMA_HandleTypeDef dma_tx_handle;
	uint16_t gpio_af;
	
	/* Get GPIO alternate function */
#if defined(GPIO_AF12_SDIO)
	gpio_af = GPIO_AF12_SDIO;
	__HAL_RCC_SDIO_CLK_ENABLE();
#endif
#if defined(GPIO_AF12_SDMMC1)
	gpio_af = GPIO_AF12_SDMMC1;
	__HAL_RCC_SDMMC1_CLK_ENABLE();
#endif

	/* Enable DMA2 clocks */
	__DMAx_TxRx_CLK_ENABLE();

	/* Detect pin, write protect pin */
#if FATFS_USE_DETECT_PIN > 0
	TM_GPIO_Init(FATFS_DETECT_PORT, FATFS_DETECT_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
#endif
#if FATFS_USE_WRITEPROTECT_PIN > 0
	TM_GPIO_Init(FATFS_WRITEPROTECT_PORT, FATFS_WRITEPROTECT_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
#endif
	
	/* SDIO/SDMMC pins */
#if FATFS_SDIO_4BIT == 1
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, gpio_af);
#else
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_8 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, gpio_af);
#endif
	TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_2, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, gpio_af);
    
    /* NVIC configuration for SDIO interrupts */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

    /* Configure DMA Rx parameters */
    dma_rx_handle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
    dma_rx_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    dma_rx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
    dma_rx_handle.Init.MemInc              = DMA_MINC_ENABLE;
    dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_rx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dma_rx_handle.Init.Mode                = DMA_PFCTRL;
    dma_rx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    dma_rx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    dma_rx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    dma_rx_handle.Init.MemBurst            = DMA_MBURST_INC4;
    dma_rx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;

    dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmarx, dma_rx_handle);

    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&dma_rx_handle);

    /* Configure the DMA stream */
    HAL_DMA_Init(&dma_rx_handle);

    /* Configure DMA Tx parameters */
    dma_tx_handle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
    dma_tx_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    dma_tx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
    dma_tx_handle.Init.MemInc              = DMA_MINC_ENABLE;
    dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dma_tx_handle.Init.Mode                = DMA_PFCTRL;
    dma_tx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
    dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;

    dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmatx, dma_tx_handle);

    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&dma_tx_handle);

    /* Configure the DMA stream */
    HAL_DMA_Init(&dma_tx_handle); 

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
}

/**
  * @brief  Initializes the SD Detect pin MSP.
  * @param  hsd: SD handle
  * @param  Params
  * @retval None
  */
__weak void BSP_SD_Detect_MspInit(SD_HandleTypeDef *hsd, void *Params) {

}

/**
  * @brief  DeInitializes the SD MSP.
  * @param  hsd: SD handle
  * @param  Params
  * @retval None
  */
__weak void BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params) {
    static DMA_HandleTypeDef dma_rx_handle;
    static DMA_HandleTypeDef dma_tx_handle;

    /* Disable NVIC for DMA transfer complete interrupts */
    HAL_NVIC_DisableIRQ(SD_DMAx_Rx_IRQn);
    HAL_NVIC_DisableIRQ(SD_DMAx_Tx_IRQn);

    /* Deinitialize the stream for new transfer */
    dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;
    HAL_DMA_DeInit(&dma_rx_handle);

    /* Deinitialize the stream for new transfer */
    dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;
    HAL_DMA_DeInit(&dma_tx_handle);

    /* Disable NVIC for SDIO interrupts */
    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);

    /* DeInit GPIO pins can be done in the application
    (by surcharging this __weak function) */

    /* Disable SDMMC1 clock */
    __HAL_RCC_SDMMC1_CLK_DISABLE();

    /* GPIO pins clock and DMA clocks can be shut down in the application
        by surcharging this __weak function */
}

/**
  * @brief  Handles SD card interrupt request.
  * @retval None
  */
#if defined(STM32F4xx)
void SDIO_IRQHandler(void)
#elif defined(STM32F7xx)
void SDMMC1_IRQHandler(void)
#endif
{
  HAL_SD_IRQHandler(&uSdHandle);
}

/**
  * @brief  Handles SD DMA Tx transfer interrupt request.
  * @retval None
  */
void SD_DMAx_Tx_IRQHandler(void) {
    HAL_DMA_IRQHandler(uSdHandle.hdmatx); 
}

/**
  * @brief  Handles SD DMA Rx transfer interrupt request.
  * @retval None
  */
void SD_DMAx_Rx_IRQHandler(void) {
    HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

/**
  * @brief  Gets the current SD card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  0: No data transfer is acting
  *            @arg  1: Data transfer is acting
  *            @arg  SD_TRANSFER_ERROR: Data transfer error 
  */
uint8_t BSP_SD_GetStatus(void) {
    return (HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER) ? 0 : 1;
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None 
  */
void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo) {
    /* Get SD card Information */
    HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}

/**************************************************************/
/*                  SDCARD WP AND DETECT                      */
/**************************************************************/
/* SDCARD detect function */
static uint8_t SDCARD_IsDetected(void) {
#if FATFS_USE_DETECT_PIN
/* FATFS_DETECT_STATE should be defined in defines.h for active high or low state*/
#ifndef FATFS_DETECT_STATE
#define FATFS_DETECT_STATE 0
#endif
	/* Check if detected, compare to expected state */
	return TM_GPIO_GetInputPinValue(FATFS_DETECT_PORT, FATFS_DETECT_PIN) == FATFS_DETECT_STATE;
#endif
	
	/* Card is detected */
	return 1;
}

/* SDCARD write protect function */
static uint8_t SDCARD_IsWriteEnabled(void) {
#if FATFS_USE_WRITEPROTECT_PIN
	/* Check if write enabled, pin LOW if write enabled */
	return !TM_GPIO_GetInputPinValue(FATFS_WRITEPROTECT_PORT, FATFS_WRITEPROTECT_PIN);
#endif
	
	/* Card is not write protected */
	return 1;
}

/**************************************************************/
/*                    LOW LEVEL FUNCTIONS                     */
/**************************************************************/
DSTATUS TM_FATFS_SD_SDIO_disk_initialize(void) {
	Stat = STA_NOINIT;

	/* Configure the SDCARD device */
	if (BSP_SD_Init() == MSD_OK) {
		Stat &= ~STA_NOINIT;
	} else {
		Stat |= STA_NOINIT;
	}

	return Stat;
}

DSTATUS TM_FATFS_SD_SDIO_disk_status(void) {
	Stat = STA_NOINIT;

	/* Check SDCARD status */
	if (BSP_SD_GetStatus() == MSD_OK) {
		Stat &= ~STA_NOINIT;
	} else {
		Stat |= STA_NOINIT;
	}

	/* Check if write enabled */
	if (SDCARD_IsWriteEnabled()) {
		Stat &= ~STA_PROTECT;
	} else {
		Stat |= STA_PROTECT;
	}

	return Stat;
}

DRESULT TM_FATFS_SD_SDIO_disk_ioctl(BYTE cmd, void *buff) {
	DRESULT res = RES_ERROR;
	HAL_SD_CardInfoTypeDef CardInfo;
  
	/* Check if init OK */
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}
  
	switch (cmd) {
		/* Make sure that no pending write process */
		case CTRL_SYNC :
			res = RES_OK;
			break;

		/* Size in bytes for single sector */
		case GET_SECTOR_SIZE:
			*(WORD *)buff = SD_BLOCK_SIZE;
			res = RES_OK;
			break;

		/* Get number of sectors on the disk (DWORD) */
		case GET_SECTOR_COUNT :
			BSP_SD_GetCardInfo(&CardInfo);
			*(DWORD *)buff = CardInfo.LogBlockNbr;
			res = RES_OK;
			break;

		/* Get erase block size in unit of sector (DWORD) */
		case GET_BLOCK_SIZE :
			*(DWORD*)buff = CardInfo.LogBlockSize;
			break;

		default:
			res = RES_PARERR;
	}
  
	return res;
}

DRESULT TM_FATFS_SD_SDIO_disk_read(BYTE *buff, DWORD sector, UINT count) {
    uint32_t timeout = 100000;
	if (BSP_SD_ReadBlocks((uint32_t *)buff, sector, count, 1000) != MSD_OK) {
		return RES_ERROR;
	}
	while (BSP_SD_GetStatus() != MSD_OK) {
        if (timeout-- == 0) {
            return RES_ERROR;
        }
    }
	return RES_OK;
}

DRESULT TM_FATFS_SD_SDIO_disk_write(const BYTE *buff, DWORD sector, UINT count) {
    uint32_t timeout = 100000;
	if (BSP_SD_WriteBlocks((uint32_t *)buff, sector, count, 1000) != MSD_OK) {
		return RES_ERROR;
	}
	while (BSP_SD_GetStatus() != MSD_OK) {
        if (timeout-- == 0) {
            return RES_ERROR;
        }
    }
	return RES_OK;
}
