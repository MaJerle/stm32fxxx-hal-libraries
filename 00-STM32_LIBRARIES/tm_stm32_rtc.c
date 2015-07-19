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
#include "tm_stm32_rtc.h"

/* Private macros */
/* Internal status registers for RTC */
#if defined(STM32F7xx)
#define RTC_STATUS_REG      			RTC_BKP_DR31 /* Status Register */
#else
#define RTC_STATUS_REG      			RTC_BKP_DR19 /* Status Register */
#endif
#define RTC_STATUS_TIME_OK  			0x4321       /* RTC time OK */
#define	RTC_STATUS_ZERO					0x0000

/* Internal RTC defines */
#define TM_RTC_LEAP_YEAR(year) 			((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))
#define TM_RTC_DAYS_IN_YEAR(x)			TM_RTC_LEAP_YEAR(x) ? 366 : 365
#define TM_RTC_OFFSET_YEAR				1970
#define TM_RTC_SECONDS_PER_DAY			86400
#define TM_RTC_SECONDS_PER_HOUR			3600
#define TM_RTC_SECONDS_PER_MINUTE		60
#define TM_RTC_BCD2BIN(x)				((((x) >> 4) & 0x0F) * 10 + ((x) & 0x0F))
#define TM_RTC_CHAR2NUM(x)				((x) - '0')
#define TM_RTC_CHARISNUM(x)				((x) >= '0' && (x) <= '9')

/* Internal functions */
static void TM_RTC_Config(TM_RTC_ClockSource_t source);
/* Default RTC status */
uint32_t TM_RTC_Status = RTC_STATUS_ZERO;

/* RTC Handle */
static RTC_HandleTypeDef hRTC;
static RTC_DateTypeDef RTC_DateStruct;
static RTC_TimeTypeDef RTC_TimeStruct;

/* Days in a month */
static uint8_t TM_RTC_Months[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	/* Not leap year */
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}	/* Leap year */
};

uint32_t TM_RTC_Init(TM_RTC_ClockSource_t source) {
	uint32_t status;
	TM_RTC_t datatime;
	
	/* Set instance */
	hRTC.Instance = RTC;
	hRTC.Init.AsynchPrediv = RTC_ASYNC_PREDIV;
	hRTC.Init.SynchPrediv = RTC_SYNC_PREDIV;
	hRTC.Init.HourFormat = RTC_HOURFORMAT_24;
	hRTC.Init.OutPut = RTC_OUTPUT_DISABLE;
	hRTC.Init.OutPutType = RTC_OUTPUT_TYPE_PUSHPULL;
	hRTC.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	
	/* Enable PWR peripheral clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Allow access to BKP Domain */
	HAL_PWR_EnableBkUpAccess();
	
	/* Get RTC status */
	status = HAL_RTCEx_BKUPRead(&hRTC, RTC_STATUS_REG);
	
	/* Check if RTC already initialized */
	if (status == RTC_STATUS_TIME_OK) {
		TM_RTC_Status = RTC_STATUS_TIME_OK;
		
		/* Start internal clock if we choose internal clock */
		if (source == TM_RTC_ClockSource_Internal) {
			TM_RTC_Config(TM_RTC_ClockSource_Internal);
		}
		
		/* Wait for RTC APB registers synchronisation (needed after start-up from Reset) */
		HAL_RTC_WaitForSynchro(&hRTC);
		
		/* Get date and time */
		TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
		
		/* Clear reset flags */
		__HAL_RCC_CLEAR_RESET_FLAGS();
		
		/* Return OK */
		return 1;
	} else {
		/* Start RTC clock */
		TM_RTC_Config(source);
		
		/* Set date */
		RTC_DateStruct.Year = 0;
		RTC_DateStruct.Month = 1;
		RTC_DateStruct.Date = 1;
		RTC_DateStruct.WeekDay = RTC_WEEKDAY_TUESDAY;

		/* Set date */
		HAL_RTC_SetDate(&hRTC, &RTC_DateStruct, RTC_FORMAT_BIN);

		/* Set time */
		RTC_TimeStruct.Hours = 0x00;
		RTC_TimeStruct.Minutes = 0x00;
		RTC_TimeStruct.Seconds = 0x00;
		RTC_TimeStruct.TimeFormat = RTC_HOURFORMAT_24;
		RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;

		/* Set time */
		HAL_RTC_SetTime(&hRTC, &RTC_TimeStruct, RTC_FORMAT_BCD);
		
		/* Init RTC */
		HAL_RTC_Init(&hRTC);

		/* Save data to backup regiser */
		HAL_RTCEx_BKUPWrite(&hRTC, RTC_STATUS_REG, RTC_STATUS_TIME_OK); 
		
		/* RTC was initialized now */
		return 0;
	}
}

TM_RTC_Result_t TM_RTC_SetDateTime(TM_RTC_t* data, TM_RTC_Format_t format) {
	TM_RTC_t tmp;
	
	/* Check date and time validation */
	if (format == TM_RTC_Format_BCD) {
		tmp.Day = TM_RTC_BCD2BIN(data->Day);
		tmp.Month = TM_RTC_BCD2BIN(data->Month);
		tmp.Year = TM_RTC_BCD2BIN(data->Year);
		tmp.Hours = TM_RTC_BCD2BIN(data->Hours);
		tmp.Minutes = TM_RTC_BCD2BIN(data->Minutes);
		tmp.Seconds = TM_RTC_BCD2BIN(data->Seconds);
		tmp.WeekDay = TM_RTC_BCD2BIN(data->WeekDay);
	} else {
		tmp.Day = data->Day;
		tmp.Month = data->Month;
		tmp.Year = data->Year;
		tmp.Hours = data->Hours;
		tmp.Minutes = data->Minutes;
		tmp.Seconds = data->Seconds;
		tmp.WeekDay = data->WeekDay;
	}
	
	/* Check year and month */
	if (
		tmp.Year > 99 || 
		tmp.Month == 0 || 
		tmp.Month > 12 ||
		tmp.Day == 0 ||
		tmp.Day > TM_RTC_Months[TM_RTC_LEAP_YEAR(2000 + tmp.Year) ? 1 : 0][tmp.Month - 1] ||
		tmp.Hours > 23 ||
		tmp.Minutes > 59 ||
		tmp.Seconds > 59 ||
		tmp.WeekDay == 0 ||
		tmp.WeekDay > 7
	) {
		/* Invalid date */
		return TM_RTC_Result_Error; 
	}
	
	/* Fill time */
	RTC_TimeStruct.Hours = data->Hours;
	RTC_TimeStruct.Minutes = data->Minutes;
	RTC_TimeStruct.Seconds = data->Seconds;
	
	/* Fill date */
	RTC_DateStruct.Date = data->Day;
	RTC_DateStruct.Month = data->Month;
	RTC_DateStruct.Year = data->Year;
	RTC_DateStruct.WeekDay = data->WeekDay;

	/* Set time */
	if (format == TM_RTC_Format_BCD) {
		HAL_RTC_SetTime(&hRTC, &RTC_TimeStruct, RTC_FORMAT_BCD);
	} else {
		HAL_RTC_SetTime(&hRTC, &RTC_TimeStruct, RTC_FORMAT_BIN);
	}
	
	/* Set date */
	if (format == TM_RTC_Format_BCD) {
		HAL_RTC_SetDate(&hRTC, &RTC_DateStruct, RTC_FORMAT_BCD);
	} else {
		HAL_RTC_SetDate(&hRTC, &RTC_DateStruct, RTC_FORMAT_BIN);
	}	
		
	/* Init RTC */
	HAL_RTC_Init(&hRTC);
	
	/* Return OK */
	return TM_RTC_Result_Ok;
}

TM_RTC_Result_t TM_RTC_SetDateTimeString(char* str) {
	TM_RTC_t tmp;
	uint8_t i = 0;
	
	/* Get date */
	tmp.Day = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.Day = tmp.Day * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get month */
	tmp.Month = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.Month = tmp.Month * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get year */
	tmp.Year = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.Year = tmp.Year * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get day in a week */
	tmp.WeekDay = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.WeekDay = tmp.WeekDay * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get hours */
	tmp.Hours = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.Hours = tmp.Hours * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get minutes */
	tmp.Minutes = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.Minutes = tmp.Minutes * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get seconds */
	tmp.Seconds = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.Seconds = tmp.Seconds * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Return status from set date time function */
	return TM_RTC_SetDateTime(&tmp, TM_RTC_Format_BIN);
}

void TM_RTC_GetDateTime(TM_RTC_t* data, TM_RTC_Format_t format) {
	uint32_t unix;

	/* Get time */
	if (format == TM_RTC_Format_BIN) {
		HAL_RTC_GetTime(&hRTC, &RTC_TimeStruct, RTC_FORMAT_BIN);
	} else {
		HAL_RTC_GetTime(&hRTC, &RTC_TimeStruct, RTC_FORMAT_BCD);
	}
	
	/* Format hours */
	data->Hours = RTC_TimeStruct.Hours;
	data->Minutes = RTC_TimeStruct.Minutes;
	data->Seconds = RTC_TimeStruct.Seconds;
	
	/* Get subseconds */
	data->Subseconds = RTC->SSR;
	
	/* Get date */
	if (format == TM_RTC_Format_BIN) {
		HAL_RTC_GetDate(&hRTC, &RTC_DateStruct, RTC_FORMAT_BIN);
	} else {
		HAL_RTC_GetDate(&hRTC, &RTC_DateStruct, RTC_FORMAT_BCD);
	}
	
	/* Format date */
	data->Year = RTC_DateStruct.Year;
	data->Month = RTC_DateStruct.Month;
	data->Day = RTC_DateStruct.Date;
	data->WeekDay = RTC_DateStruct.WeekDay;
	
	/* Calculate unix offset */
	unix = TM_RTC_GetUnixTimeStamp(data);
	data->Unix = unix;
}

uint8_t TM_RTC_GetDaysInMonth(uint8_t month, uint8_t year) {
	/* Check input data */
	if (
		month == 0 ||
		month > 12
	) {
		/* Error */
		return 0;
	}
	
	/* Return days in month */
	return TM_RTC_Months[TM_RTC_LEAP_YEAR(2000 + year) ? 1 : 0][month - 1];
}

uint16_t TM_RTC_GetDaysInYear(uint8_t year) {
	/* Return days in year */
	return TM_RTC_DAYS_IN_YEAR(2000 + year);
}

void TM_RTC_Config(TM_RTC_ClockSource_t source) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	/* Init LSE and LSI oscillators */
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	
	/* Check for proper oscillator */
	if (source == TM_RTC_ClockSource_Internal) {
		RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
		RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	} else {
		RCC_OscInitStruct.LSEState = RCC_LSE_ON;
		RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	}
	
	/* Config oscillator */
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select peripheral clock */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	if (source == TM_RTC_ClockSource_Internal) {
		PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	} else {
		PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	}
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	
	/* Enable RTC Clock */ 
	__HAL_RCC_RTC_ENABLE(); 
}

void TM_RTC_Interrupts(TM_RTC_Int_t int_value) {
	uint32_t int_val;
	
	/* Disable wakeup interrupt */
	__HAL_RTC_WAKEUPTIMER_DISABLE(&hRTC);
	
	/* Disable RTC interrupt flag */
	__HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hRTC, RTC_IT_WUT);
	
	/* Clear pending bit */
	__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
	
	/* Clear flag */
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hRTC, RTC_FLAG_WUTF);
	
	/* If enable again */
	if (int_value != TM_RTC_Int_Disable) {
		if (int_value == TM_RTC_Int_60s) {
			int_val = 0x3BFFF; 		/* 60 seconds = 60 * 4096 / 1 = 245760 */
		} else if (int_value == TM_RTC_Int_30s) {
			int_val = 0x1DFFF;		/* 30 seconds */
		} else if (int_value == TM_RTC_Int_15s) {
			int_val = 0xEFFF;		/* 15 seconds */
		} else if (int_value == TM_RTC_Int_10s) {
			int_val = 0x9FFF;		/* 10 seconds */
		} else if (int_value == TM_RTC_Int_5s) {
			int_val = 0x4FFF;		/* 5 seconds */
		} else if (int_value == TM_RTC_Int_2s) {
			int_val = 0x1FFF;		/* 2 seconds */
		} else if (int_value == TM_RTC_Int_1s) {
			int_val = 0x0FFF;		/* 1 second */
		} else if (int_value == TM_RTC_Int_500ms) {
			int_val = 0x7FF;		/* 500 ms */
		} else if (int_value == TM_RTC_Int_250ms) {
			int_val = 0x3FF;		/* 250 ms */
		} else if (int_value == TM_RTC_Int_125ms) {
			int_val = 0x1FF;		/* 125 ms */
		}		

		/* Clock divided by 8, 32768 / 8 = 4096 */
		/* 4096 ticks for 1second interrupt */
		HAL_RTCEx_SetWakeUpTimer_IT(&hRTC, int_val, RTC_WAKEUPCLOCK_RTCCLK_DIV8);
		
		/* Set NVIC */
		HAL_NVIC_SetPriority(RTC_WKUP_IRQn, RTC_PRIORITY, RTC_WAKEUP_SUBPRIORITY);
		HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
	}
}

uint32_t TM_RTC_GetUnixTimeStamp(TM_RTC_t* data) {
	uint32_t days = 0, seconds = 0;
	uint16_t i;
	uint16_t year = (uint16_t) (data->Year + 2000);
	/* Year is below offset year */
	if (year < TM_RTC_OFFSET_YEAR) {
		return 0;
	}
	/* Days in back years */
	for (i = TM_RTC_OFFSET_YEAR; i < year; i++) {
		days += TM_RTC_DAYS_IN_YEAR(i);
	}
	/* Days in current year */
	for (i = 1; i < data->Month; i++) {
		days += TM_RTC_Months[TM_RTC_LEAP_YEAR(year)][i - 1];
	}
	/* Day starts with 1 */
	days += data->Day - 1;
	seconds = days * TM_RTC_SECONDS_PER_DAY;
	seconds += data->Hours * TM_RTC_SECONDS_PER_HOUR;
	seconds += data->Minutes * TM_RTC_SECONDS_PER_MINUTE;
	seconds += data->Seconds;
	
	/* seconds = days * 86400; */
	return seconds;
}

void TM_RTC_GetDateTimeFromUnix(TM_RTC_t* data, uint32_t unix) {
	uint16_t year;
	
	/* Store unix time to unix in struct */
	data->Unix = unix;
	/* Get seconds from unix */
	data->Seconds = unix % 60;
	/* Go to minutes */
	unix /= 60;
	/* Get minutes */
	data->Minutes = unix % 60;
	/* Go to hours */
	unix /= 60;
	/* Get hours */
	data->Hours = unix % 24;
	/* Go to days */
	unix /= 24;
	
	/* Get week day */
	/* Monday is day one */
	data->WeekDay = (unix + 3) % 7 + 1;

	/* Get year */
	year = 1970;
	while (1) {
		if (TM_RTC_LEAP_YEAR(year)) {
			if (unix >= 366) {
				unix -= 366;
			} else {
				break;
			}
		} else if (unix >= 365) {
			unix -= 365;
		} else {
			break;
		}
		year++;
	}
	/* Get year in xx format */
	data->Year = (uint8_t) (year - 2000);
	/* Get month */
	for (data->Month = 0; data->Month < 12; data->Month++) {
		if (TM_RTC_LEAP_YEAR(year) && unix >= (uint32_t)TM_RTC_Months[1][data->Month]) {
			unix -= TM_RTC_Months[1][data->Month];
		} else if (unix >= (uint32_t)TM_RTC_Months[0][data->Month]) {
			unix -= TM_RTC_Months[0][data->Month];
		} else {
			break;
		}
	}
	/* Get month */
	/* Month starts with 1 */
	data->Month++;
	/* Get date */
	/* Date starts with 1 */
	data->Day = unix + 1;
}

void TM_RTC_EnableAlarm(TM_RTC_Alarm_t Alarm, TM_RTC_AlarmTime_t* DataTime, TM_RTC_Format_t format) {
	RTC_AlarmTypeDef salarmstructure;
	
	/* Alarm type is every week the same day in a week */
	if (DataTime->Type == TM_RTC_AlarmType_DayInWeek) {
		/* Alarm trigger every week the same day in a week */
		salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
		
		/* Week day can be between 1 and 7 */
		if (DataTime->Day == 0) {
			salarmstructure.AlarmDateWeekDay = 1;
		} else if (DataTime->Day > 7) {
			salarmstructure.AlarmDateWeekDay = 7;
		} else {
			salarmstructure.AlarmDateWeekDay = DataTime->Day;
		}
	} else { /* Alarm type is every month the same day */
		/* Alarm trigger every month the same day in a month */
		salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	
		/* Month day can be between 1 and 31 */
		if (DataTime->Day == 0) {
			salarmstructure.AlarmDateWeekDay = 1;
		} else if (DataTime->Day > 31) {
			salarmstructure.AlarmDateWeekDay = 31;
		} else {
			salarmstructure.AlarmDateWeekDay = DataTime->Day;
		}
	}
	
	/* Set alarm settings */
	if (Alarm == TM_RTC_Alarm_A) {
		salarmstructure.Alarm = RTC_ALARM_A;
	} else {
		salarmstructure.Alarm = RTC_ALARM_B;
	}
	salarmstructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
	salarmstructure.AlarmTime.Hours = DataTime->Hours;
	salarmstructure.AlarmTime.Minutes = DataTime->Minutes;
	salarmstructure.AlarmTime.Seconds = DataTime->Seconds;
	salarmstructure.AlarmTime.SubSeconds = 0;

	/* Enable alarm with interrupt */
	if (format == TM_RTC_Format_BIN) {
		HAL_RTC_SetAlarm_IT(&hRTC, &salarmstructure, RTC_FORMAT_BIN);
	} else {
		HAL_RTC_SetAlarm_IT(&hRTC, &salarmstructure, RTC_FORMAT_BCD);
	}
	
	/* Enable NVIC */
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, RTC_PRIORITY, RTC_ALARM_SUBPRIORITY);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void TM_RTC_DisableAlarm(TM_RTC_Alarm_t Alarm) {
	if (Alarm == TM_RTC_Alarm_A) {	
		HAL_RTC_DeactivateAlarm(&hRTC, RTC_ALARM_A);
	} else {
		HAL_RTC_DeactivateAlarm(&hRTC, RTC_ALARM_B);
	}
}

void TM_RTC_WriteBackupRegister(uint8_t location, uint32_t value) {
	/* Write data to backup register */
	*(uint32_t *)((&RTC->BKP0R) + 4 * location) = value;
}

uint32_t TM_RTC_ReadBackupRegister(uint8_t location) {
	/* Read data from backup register */
	return *(uint32_t *)((&RTC->BKP0R) + 4 * location);
}

/* Callbacks */
__weak void TM_RTC_WakeupHandler(void) {
	/* If user needs this function, then they should be defined separatelly in your project */
}

__weak void TM_RTC_AlarmAHandler(void) {
	/* If user needs this function, then they should be defined separatelly in your project */
}

__weak void TM_RTC_AlarmBHandler(void) {
	/* If user needs this function, then they should be defined separatelly in your project */
}

/* Private RTC IRQ handlers */
void RTC_WKUP_IRQHandler(void) {
	/* Check for RTC interrupt */
	if (__HAL_RTC_WAKEUPTIMER_GET_IT(&hRTC, RTC_IT_WUT) != RESET) {
		/* Call user function */
		TM_RTC_WakeupHandler();
		
		/* Clear interrupt flags */
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hRTC, RTC_FLAG_WUTF);
	}
	
	/* Clear EXTI line 22 bit */
	__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
}

void RTC_Alarm_IRQHandler(void) {
	/* RTC Alarm A check */
	if (__HAL_RTC_ALARM_GET_IT(&hRTC, RTC_IT_ALRA) != RESET) {
		/* Clear RTC Alarm A interrupt flag */
		__HAL_RTC_ALARM_CLEAR_FLAG(&hRTC, RTC_FLAG_ALRAF);
		
		/* Call user function for Alarm A */
		TM_RTC_AlarmAHandler();
	}
	
	/* RTC Alarm B check */
	if (__HAL_RTC_ALARM_GET_IT(&hRTC, RTC_IT_ALRB) != RESET) {
		/* Clear RTC Alarm A interrupt flag */
		__HAL_RTC_ALARM_CLEAR_FLAG(&hRTC, RTC_FLAG_ALRBF);
		
		/* Call user function for Alarm B */
		TM_RTC_AlarmBHandler();
	}
	
	/* Clear EXTI line 22 bit */
	__HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
}
