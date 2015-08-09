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
#include "tm_stm32_usb.h"

/* If HOST mode is used */
#ifdef USB_USE_HOST
/* Include TM USB HOST library */
#include "tm_stm32_usb_host.h"

/* Format external variables */
#ifdef USB_USE_FS
extern HCD_HandleTypeDef hhcd_FS;
#endif
#ifdef USB_USE_HS
extern HCD_HandleTypeDef hhcd_HS;
#endif
#endif

/* If DEVICE mode is used */
#ifdef USB_USE_DEVICE
/* Include TM USB DEVICE library */
#include "tm_stm32_usb_device.h"

/* Format external variables */
#ifdef USB_USE_FS
extern PCD_HandleTypeDef hpcd_FS;
#endif
#ifdef USB_USE_HS
extern PCD_HandleTypeDef hpcd_HS;
#endif
#endif

TM_USB_Result_t TM_USB_Init(void) {
#if defined(STM32F7xx)
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
	
	/* Select PLLSAI output as USB clock source */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIQ = 4; 
	PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
	
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		/* Return Error */
		return TM_USB_Result_Error;
	}
#endif
	
	/* Return OK */
	return TM_USB_Result_Ok;
}

TM_USB_Result_t TM_USB_InitFS(void) {
	/* Init DP and DM pins for USB */
	TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF10_OTG_FS);
	
	/* Init ID pin */
	TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_10, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High, GPIO_AF10_OTG_FS);
	
#if defined(USB_FS_USE_ENABLE_PIN)
	/* Init VBUS ENABLE pin */
	TM_GPIO_Init(USB_FS_ENABLE_PORT, USB_FS_ENABLE_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High);
	
	/* Disable USB output */
	TM_GPIO_SetPinValue(USB_FS_ENABLE_PORT, USB_FS_ENABLE_PIN, !USB_FS_ENABLE_STATE);
#endif

	/* Enable USB FS Clocks */ 
	__HAL_RCC_USB_OTG_FS_CLK_ENABLE();

	/* Set USBFS Interrupt priority */
	HAL_NVIC_SetPriority(OTG_FS_IRQn, USB_NVIC_PRIORITY, 0);

	/* Enable USBFS Interrupt */
	HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
	
	/* Return OK */
	return TM_USB_Result_Ok;
}

TM_USB_Result_t TM_USB_InitHS(void) {
#if defined(USB_USE_HS)
#if defined(USB_USE_ULPI_PHY)
	/* Use external ULPI PHY */
	
	/* D0 and CLK */
	TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_3 | GPIO_PIN_5, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF10_OTG_HS);
	/* D1 D2 D3 D4 D5 D6 D7 */
	TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF10_OTG_HS);
	/* STP */
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_0, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF10_OTG_HS);
	/* NXT */
	TM_GPIO_InitAlternate(GPIOH, GPIO_PIN_4, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF10_OTG_HS);
	/* DIR */
	TM_GPIO_InitAlternate(GPIOC, GPIO_PIN_2, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF10_OTG_HS);
	
	/* Enable ULPI clock */
	__HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
#else
	/* Use embedded PHY */
	/* Init ID, VBUS, DP and DM pins for USB */
	TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF12_OTG_HS_FS);
	
#if defined(USB_HS_USE_ENABLE_PIN)
	/* Init VBUS ENABLE pin */
	TM_GPIO_Init(USB_HS_ENABLE_PORT, USB_HS_ENABLE_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High);
	
	/* Disable USB output */
	TM_GPIO_SetPinValue(USB_HS_ENABLE_PORT, USB_HS_ENABLE_PIN, !USB_HS_ENABLE_STATE);
#endif /* USB_HS_USE_ENABLE_PIN */
#endif /* USB_USE_ULPI_PHY */

	/* Enable USB HS Clocks */ 
	__HAL_RCC_USB_OTG_HS_CLK_ENABLE();

	/* Set USBHS Interrupt priority */
	HAL_NVIC_SetPriority(OTG_HS_IRQn, USB_NVIC_PRIORITY, 0);

	/* Enable USBHS Interrupt */
	HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
	
	/* Return OK */
	return TM_USB_Result_Ok;
#else
	/* Return ERROR */
	return TM_USB_Result_Error;
#endif
}

/**************************************************/
/*          USB ENABLE/DISABLE CALLBACKS          */
/**************************************************/
__weak void TM_USB_DriveVBUSCallback(TM_USB_t USB_Mode, uint8_t state) {
#if defined(USB_USE_FS) && defined(USB_FS_ENABLE_PIN)
	if (USB_Mode == TM_USB_FS) {
		if (state == 0) {
			/* Disable output */
			TM_GPIO_SetPinValue(USB_FS_ENABLE_PORT, USB_FS_ENABLE_PIN, !USB_FS_ENABLE_STATE);
		} else {
			/* Enable output */
			TM_GPIO_SetPinValue(USB_FS_ENABLE_PORT, USB_FS_ENABLE_PIN, USB_FS_ENABLE_STATE);
		}
	}
#endif
	
#if defined(USB_USE_HS) && defined(USB_HS_ENABLE_PIN)
	if (USB_Mode == TM_USB_HS) {
		if (state == 0) {
			/* Disable output */
			TM_GPIO_SetPinValue(USB_HS_ENABLE_PORT, USB_HS_ENABLE_PIN, !USB_HS_ENABLE_STATE);
		} else {
			/* Enable output */
			TM_GPIO_SetPinValue(USB_HS_ENABLE_PORT, USB_HS_ENABLE_PIN, USB_HS_ENABLE_STATE);
		}
	}
#endif
}

/**************************************************/
/*               USB FS IRQ HANDLER               */
/**************************************************/
#ifdef USB_USE_FS
void OTG_FS_IRQHandler(void) {
#ifdef USB_USE_HOST
	/* Process HCD IRQ */
	HAL_HCD_IRQHandler(&hhcd_FS);
#endif
	
#ifdef USB_USE_DEVICE
	/* Process PCD IRQ */
	HAL_PCD_IRQHandler(&hpcd_FS);
#endif
}
#endif

/**************************************************/
/*               USB HS IRQ HANDLER               */
/**************************************************/
#ifdef USB_USE_HS
void OTG_HS_IRQHandler(void) {
#ifdef USB_USE_HOST
	/* Process HCD IRQ */
	HAL_HCD_IRQHandler(&hhcd_HS);
#endif
	
#ifdef USB_USE_DEVICE
	/* Process PCD IRQ */
	HAL_PCD_IRQHandler(&hpcd_HS);
#endif
}
#endif
