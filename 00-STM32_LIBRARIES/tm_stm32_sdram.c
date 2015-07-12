/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
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
#include "tm_stm32_sdram.h"

/* Internal functions */
static void TM_SDRAM_InitPins(void);

uint8_t TM_SDRAM_Init(void) {
	SDRAM_HandleTypeDef SDRAMHandle;
	FMC_SDRAM_TimingTypeDef Timing;
	FMC_SDRAM_CommandTypeDef Command;

	volatile uint32_t timeout = SDRAM_TIMEOUT;
	static uint8_t initialized = 0;
	
	/* Already initialized */
	if (initialized) {
		return 1;
	}
	
	/* Set instance */
	SDRAMHandle.Instance = FMC_SDRAM_DEVICE;
	
	/* Initialize FMC pins */
	TM_SDRAM_InitPins();
	
	/* Enable FMC clock */
	__HAL_RCC_FMC_CLK_ENABLE();
	
	/* FMC SDRAM device initialization sequence --------------------------------*/ 
	/* Step 1 ----------------------------------------------------*/ 
	/* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
	/* TMRD: 2 Clock cycles */
	/* 1 clock cycle = 1 / 90MHz = 11.1ns */
	Timing.LoadToActiveDelay    = 2;
	Timing.ExitSelfRefreshDelay = 7;
	Timing.SelfRefreshTime      = 4;
	Timing.RowCycleDelay        = 7;
	Timing.WriteRecoveryTime    = 2;
	Timing.RPDelay              = 2;
	Timing.RCDDelay             = 2;
	
	
	/* FMC SDRAM control configuration */
	SDRAMHandle.Init.SDBank             = SDRAM_BANK;
	SDRAMHandle.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
	SDRAMHandle.Init.RowBitsNumber      = SDRAM_ROWBITS_NUMBER;
	SDRAMHandle.Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
	SDRAMHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	SDRAMHandle.Init.CASLatency         = SDRAM_CAS_LATENCY;
	SDRAMHandle.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	SDRAMHandle.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;
	SDRAMHandle.Init.ReadBurst          = SDRAM_READ_BURST_STATE;
	SDRAMHandle.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;
	
	/* FMC SDRAM bank initialization */
	HAL_SDRAM_Init(&SDRAMHandle, &Timing);
	
	/* SDRAM Init sequence */
	
	/* Configure a clock configuration enable command */
	Command.CommandMode				= FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget 			= SDRAM_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= 0;
	
	/* Send command */
	HAL_SDRAM_SendCommand(&SDRAMHandle, &Command, SDRAM_TIMEOUT);
	
	/* Little delay */
	timeout = SDRAM_TIMEOUT * 0xF;
	while (timeout--);
	
	/* Configure a PALL (precharge all) command */ 
	Command.CommandMode          	= FMC_SDRAM_CMD_PALL;
	Command.CommandTarget          	= SDRAM_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber      	= 1;
	Command.ModeRegisterDefinition 	= 0;
	
	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAMHandle, &Command, SDRAM_TIMEOUT);

	/* Configure a Auto-Refresh command */ 
	Command.CommandMode            	= FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget          	= SDRAM_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber      	= 8;
	Command.ModeRegisterDefinition 	= 0;
	
	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAMHandle, &Command, SDRAM_TIMEOUT);
	
	/* Configure a load Mode register command */
	Command.CommandMode            	= FMC_SDRAM_CMD_LOAD_MODE;
	Command.CommandTarget          	= SDRAM_COMMAND_TARGET_BANK;
	Command.AutoRefreshNumber      	= 1;
	Command.ModeRegisterDefinition 	= (uint32_t)SDRAM_REG_VALUE;
	
	/* Wait until the SDRAM controller is ready */
	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAMHandle, &Command, SDRAM_TIMEOUT);
  
	/* Step 6: Set the refresh rate counter */
	/* Set the device refresh rate */
	HAL_SDRAM_ProgramRefreshRate(&SDRAMHandle, SDRAM_REFRESH_COUNT); 
	
	/* Little delay */
	timeout = SDRAM_TIMEOUT * 0xF;
	while (timeout--);
	
	/* Check if everything goes right */
	/* Write 0x45 at location 0x50 and check if result is the same on read operation */
	TM_SDRAM_Write8(0x50, 0x45);
	
	/* Read and check */
	if (TM_SDRAM_Read8(0x50) == 0x45) {
		/* Initialized OK */
		initialized = 1;
		/* Initialized OK */
		return 1;
	}
	
	/* Not initialized OK */
	initialized = 0;
	
	/* Not ok */
	return 0;
}

static void TM_SDRAM_InitPins(void) {
	/* Try to initialize from user */
	if (TM_SDRAM_InitCustomPinsCallback()) {
		/* User has initialized pins by itself */
		return;
	}
	
#if defined(SDRAM_USE_STM32439_EVAL)
	TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOH, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOI, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
#elif defined(SDRAM_USE_STM32F7_DISCOVERY)
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_3, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7| GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4| GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOH, GPIO_PIN_3 | GPIO_PIN_5, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
#else
	TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_5 | GPIO_PIN_6, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_0, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOF, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
	TM_GPIO_InitAlternate(GPIOG, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_FMC);
#endif
}

__weak uint8_t TM_SDRAM_InitCustomPinsCallback(void) {
	/* If you need custom implementation, then create this function externally. */
	/* This function here should not be modified */
	
	/* Return 0, lib will use default pins */
	return 0;
}
