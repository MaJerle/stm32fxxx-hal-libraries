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

/* Function declarations */
uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_DeInit(void);
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr);
HAL_SD_TransferStateTypedef BSP_SD_GetStatus(void);
void    BSP_SD_GetCardInfo(HAL_SD_CardInfoTypedef *CardInfo);
uint8_t BSP_SD_IsDetected(void);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void    BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params);
void    BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params);


#define SD_CardInfo HAL_SD_CardInfoTypedef
    
#define MSD_OK                        ((uint8_t)0x00)
#define MSD_ERROR                     ((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT      ((uint8_t)0x02)

#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)

#define SD_DATATIMEOUT           ((uint32_t)100000000)
    
/* DMA definitions for SD DMA transfer */
#define __DMAx_TxRx_CLK_ENABLE            __HAL_RCC_DMA2_CLK_ENABLE
#define SD_DMAx_Tx_CHANNEL                DMA_CHANNEL_4
#define SD_DMAx_Rx_CHANNEL                DMA_CHANNEL_4
#define SD_DMAx_Tx_STREAM                 DMA2_Stream6  
#define SD_DMAx_Rx_STREAM                 DMA2_Stream3  
#define SD_DMAx_Tx_IRQn                   DMA2_Stream6_IRQn
#define SD_DMAx_Rx_IRQn                   DMA2_Stream3_IRQn
#define SD_DMAx_Tx_IRQHandler             DMA2_Stream6_IRQHandler   
#define SD_DMAx_Rx_IRQHandler             DMA2_Stream3_IRQHandler

/* Block size on SDCARD */
#define BLOCK_SIZE 512

/* Status of SDCARD */
static volatile DSTATUS Stat = STA_NOINIT;

/**************************************************************/
/*                  SDCARD WP AND DETECT                      */
/**************************************************************/
/* SDCARD detect function */
static uint8_t SDCARD_IsDetected(void) {
#if FATFS_USE_DETECT_PIN
	/* Check if detected, pin LOW if detected */
	return !TM_GPIO_GetInputPinValue(FATFS_DETECT_PORT, FATFS_DETECT_PIN);
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
	SD_CardInfo CardInfo;
  
	/* Check if init OK */
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}
  
	switch (cmd) {
		/* Make sure that no pending write process */
		case CTRL_SYNC :
			res = RES_OK;
			break;

		/* Get number of sectors on the disk (DWORD) */
		case GET_SECTOR_COUNT :
			BSP_SD_GetCardInfo(&CardInfo);
			*(DWORD *)buff = CardInfo.CardCapacity / BLOCK_SIZE;
			res = RES_OK;
			break;

		/* Get R/W sector size (WORD) */
		case GET_SECTOR_SIZE :
			*(WORD *)buff = BLOCK_SIZE;
			res = RES_OK;
			break;

		/* Get erase block size in unit of sector (DWORD) */
		case GET_BLOCK_SIZE :
			*(DWORD*)buff = BLOCK_SIZE;
			break;

		default:
			res = RES_PARERR;
	}
  
	return res;
}

DRESULT TM_FATFS_SD_SDIO_disk_read(BYTE *buff, DWORD sector, UINT count) {
	if (BSP_SD_ReadBlocks((uint32_t *)buff, (uint64_t) (sector * BLOCK_SIZE), BLOCK_SIZE, count) != MSD_OK) {
		return RES_ERROR;
	}
	
	return RES_OK;
}

DRESULT TM_FATFS_SD_SDIO_disk_write(const BYTE *buff, DWORD sector, UINT count) {
	if (BSP_SD_WriteBlocks((uint32_t *)buff, (uint64_t) (sector * BLOCK_SIZE), BLOCK_SIZE, count) != MSD_OK) {
		return RES_ERROR;
	}
	
	return RES_OK;
}

/*********************************************************/
/*      Low level functions for SDIO communciation       */
/*********************************************************/

/** @defgroup STM324x9I_EVAL_SD_Private_Variables
  * @{
  */
static SD_HandleTypeDef uSdHandle;
static SD_CardInfo uSdCardInfo;
/**
  * @}
  */ 
  
/** @defgroup STM324x9I_EVAL_SD_Private_FunctionPrototypes
  * @{
  */
static void SD_MspInit(void);
/**
  * @}
  */ 
  
/** @defgroup STM324x9I_EVAL_SD_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the SD card device.
  * @param  None
  * @retval SD status
  */
uint8_t BSP_SD_Init(void) { 
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
	SD_MspInit();

	/* Check if the SD card is plugged in the slot */
	if (BSP_SD_IsDetected() != SD_PRESENT) {
		return MSD_ERROR;
	}

	/* HAL SD initialization */
	if (HAL_SD_Init(&uSdHandle, &uSdCardInfo) != SD_OK) {
		SD_state = MSD_ERROR;
	}

	/* Configure SD Bus width */
	if (SD_state == MSD_OK) {
		/* Enable wide operation */
#if defined(SDIO_BUS_WIDE_4B)
#if FATFS_SDIO_4BIT == 1
		if (HAL_SD_WideBusOperation_Config(&uSdHandle, SDIO_BUS_WIDE_4B) != SD_OK) {
#else
		if (HAL_SD_WideBusOperation_Config(&uSdHandle, SDIO_BUS_WIDE_1B) != SD_OK) {	
#endif
#else
#if FATFS_SDIO_4BIT == 1
		if (HAL_SD_WideBusOperation_Config(&uSdHandle, SDMMC_BUS_WIDE_4B) != SD_OK) {
#else
		if (HAL_SD_WideBusOperation_Config(&uSdHandle, SDMMC_BUS_WIDE_1B) != SD_OK) {	
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
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param  None
 * @retval Returns if SD is detected or not
 */
uint8_t BSP_SD_IsDetected(void) {
	return SDCARD_IsDetected();
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks) {
	if (HAL_SD_ReadBlocks(&uSdHandle, pData, ReadAddr, BlockSize, NumOfBlocks) != SD_OK) {
		return MSD_ERROR;
	}
	
	return MSD_OK;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks) {
	if (HAL_SD_WriteBlocks(&uSdHandle, pData, WriteAddr, BlockSize, NumOfBlocks) != SD_OK) {
		return MSD_ERROR;
	}
	
	return MSD_OK;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks) {
	uint8_t SD_state = MSD_OK;

	/* Read block(s) in DMA transfer mode */
	if (HAL_SD_ReadBlocks_DMA(&uSdHandle, pData, ReadAddr, BlockSize, NumOfBlocks) != SD_OK) {
		SD_state = MSD_ERROR;
	}

	/* Wait until transfer is complete */
	if (SD_state == MSD_OK) {
		if (HAL_SD_CheckReadOperation(&uSdHandle, (uint32_t)SD_DATATIMEOUT) != SD_OK) {
			SD_state = MSD_ERROR;
		} else {
			SD_state = MSD_OK;
		}
	}

	return SD_state; 
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write 
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks) {
	uint8_t SD_state = MSD_OK;

	/* Write block(s) in DMA transfer mode */
	if (HAL_SD_WriteBlocks_DMA(&uSdHandle, pData, WriteAddr, BlockSize, NumOfBlocks) != SD_OK) {
		SD_state = MSD_ERROR;
	}

	/* Wait until transfer is complete */
	if (SD_state == MSD_OK) {
		if(HAL_SD_CheckWriteOperation(&uSdHandle, (uint32_t)SD_DATATIMEOUT) != SD_OK) {
			SD_state = MSD_ERROR;
		} else {
			SD_state = MSD_OK;
		}
	}

	return SD_state;  
}

/**
  * @brief  Erases the specified memory area of the given SD card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr) {
	if (HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != SD_OK) {
		return MSD_ERROR;
	}
	
	return MSD_OK;
}

/**
  * @brief  Initializes the SD MSP.
  * @param  None
  * @retval None
  */
static void SD_MspInit(void) {
	static DMA_HandleTypeDef dmaRxHandle;
	static DMA_HandleTypeDef dmaTxHandle;
	SD_HandleTypeDef *hsd = &uSdHandle;
	uint16_t gpio_af;
	
	/* Get GPIO alternate function */
#if defined(GPIO_AF12_SDIO)
	gpio_af = GPIO_AF12_SDIO;
#endif
#if defined(GPIO_AF12_SDMMC1)
	gpio_af = GPIO_AF12_SDMMC1;
#endif
	

	/* Enable SDIO clock */
	__HAL_RCC_SDIO_CLK_ENABLE();

	/* Enable DMA2 clocks */
	__DMAx_TxRx_CLK_ENABLE();

	/* Detect pin, write protect pin */
#if FATFS_USE_DETECT_PIN > 0
	TM_GPIO_Init(FATFS_DETECT_PIN, FATFS_DETECT_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
#endif
#if FATFS_USE_WRITEPROTECT_PIN > 0
	TM_GPIO_Init(FATFS_WRITEPROTECT_PORT, FATFS_WRITEPROTECT_PIN, TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);
#endif
	
	/* SDIO/SDMMC pins */
#if FATFS_SDIO_4BIT == 1
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Fast, gpio_af);
#else
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_8 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Fast, gpio_af);
#endif
	TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_2, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Fast, gpio_af);

	/* NVIC configuration for SDIO interrupts */
	HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(SDIO_IRQn);

	/* Configure DMA Rx parameters */
	dmaRxHandle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
	dmaRxHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	dmaRxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaRxHandle.Init.MemInc              = DMA_MINC_ENABLE;
	dmaRxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaRxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	dmaRxHandle.Init.Mode                = DMA_PFCTRL;
	dmaRxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	dmaRxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	dmaRxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	dmaRxHandle.Init.MemBurst            = DMA_MBURST_INC4;
	dmaRxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;

	dmaRxHandle.Instance = SD_DMAx_Rx_STREAM;

	/* Associate the DMA handle */
	__HAL_LINKDMA(hsd, hdmarx, dmaRxHandle);

	/* Deinitialize the stream for new transfer */
	HAL_DMA_DeInit(&dmaRxHandle);

	/* Configure the DMA stream */
	HAL_DMA_Init(&dmaRxHandle);

	/* Configure DMA Tx parameters */
	dmaTxHandle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
	dmaTxHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	dmaTxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dmaTxHandle.Init.MemInc              = DMA_MINC_ENABLE;
	dmaTxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaTxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	dmaTxHandle.Init.Mode                = DMA_PFCTRL;
	dmaTxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	dmaTxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	dmaTxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	dmaTxHandle.Init.MemBurst            = DMA_MBURST_INC4;
	dmaTxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;

	dmaTxHandle.Instance = SD_DMAx_Tx_STREAM;

	/* Associate the DMA handle */
	__HAL_LINKDMA(hsd, hdmatx, dmaTxHandle);

	/* Deinitialize the stream for new transfer */
	HAL_DMA_DeInit(&dmaTxHandle);

	/* Configure the DMA stream */
	HAL_DMA_Init(&dmaTxHandle); 

	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
}


/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None 
  */
void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypedef *CardInfo) {
	/* Get SD card Information */
	HAL_SD_Get_CardInfo(&uSdHandle, CardInfo);
}

HAL_SD_TransferStateTypedef BSP_SD_GetStatus(void) {
	return HAL_SD_GetStatus(&uSdHandle);
}

/**
  * @brief  This function handles DMA2 Stream 3 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream3_IRQHandler(void) {
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream6_IRQHandler(void) {
	HAL_DMA_IRQHandler(uSdHandle.hdmatx); 
}

/**
  * @brief  This function handles SDIO interrupt request.
  * @param  None
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
