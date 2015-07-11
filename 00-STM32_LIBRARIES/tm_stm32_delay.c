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
#include "tm_stm32_delay.h"

/* Functions for delay */
__IO uint32_t TM_Time2 = 0;
__IO uint32_t TM_Time = 0;

/* Private structure */
typedef struct {
	uint8_t Count;
	TM_DELAY_Timer_t* Timers[DELAY_MAX_CUSTOM_TIMERS];
} TM_DELAY_Timers_t;

/* Custom timers structure */
static TM_DELAY_Timers_t CustomTimers = {0};

uint32_t TM_DELAY_Init(void) {
	uint32_t c;
	
    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;
	
    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;
	
    /* Reset counter */
    DWT->CYCCNT = 0;
	
	/* Check if DWT has started */
	c = DWT->CYCCNT;
	
	/* 2 dummys */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	
	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - c);
}


TM_DELAY_Timer_t* TM_DELAY_TimerCreate(uint32_t ReloadValue, uint8_t AutoReloadCmd, uint8_t StartTimer, void (*TM_DELAY_CustomTimerCallback)(struct _TM_DELAY_Timer_t*, void *), void* UserParameters) {
	TM_DELAY_Timer_t* tmp;
	
	/* Check if available */
	if (CustomTimers.Count >= DELAY_MAX_CUSTOM_TIMERS) {
		return NULL;
	}
	
	/* Try to allocate memory for timer structure */
	tmp = (TM_DELAY_Timer_t *) LIB_ALLOC_FUNC(sizeof(TM_DELAY_Timer_t));
	
	/* Check if allocated */
	if (tmp == NULL) {
		return NULL;
	}
	
	/* Fill settings */
	tmp->ARR = ReloadValue;
	tmp->CNT = tmp->ARR;
	tmp->AR = AutoReloadCmd;
	tmp->CTRL = StartTimer;
	tmp->Callback = TM_DELAY_CustomTimerCallback;
	tmp->UserParameters = UserParameters;
	
	/* Increase number of timers in memory */
	CustomTimers.Timers[CustomTimers.Count++] = tmp;
	
	/* Return pointer to user */
	return tmp;
} 

void TM_DELAY_TimerDelete(TM_DELAY_Timer_t* Timer) {
	uint8_t i;
	uint32_t irq;
	TM_DELAY_Timer_t* tmp;
	
	/* Get location in array of pointers */
	for (i = 0; i < CustomTimers.Count; i++) {
		if (Timer == CustomTimers.Timers[i]) {
			break;
		}
	}
	
	/* Check for valid input */
	if (i == CustomTimers.Count) {
		return;
	}
	
	/* Save pointer to timer */
	tmp = CustomTimers.Timers[i];
	
	/* Get interrupt status */
	irq = __get_PRIMASK();

	/* Disable interrupts */
	__disable_irq();
	
	/* Shift array up */
	for (; i < (CustomTimers.Count - 1); i++) {
		/* Shift data to the left */
		CustomTimers.Timers[i] = CustomTimers.Timers[i + 1];
	}
	
	/* Decrease count */
	CustomTimers.Count--;
	
	/* Free timer */
	LIB_FREE_FUNC(tmp);
	
	/* Enable IRQ if necessary */
	if (!irq) {
		__enable_irq();
	}
}

TM_DELAY_Timer_t* TM_DELAY_TimerStop(TM_DELAY_Timer_t* Timer) {
	/* Disable timer */
	Timer->CTRL = 0;
	
	/* Return pointer */
	return Timer;
}

TM_DELAY_Timer_t* TM_DELAY_TimerStart(TM_DELAY_Timer_t* Timer) {
	/* Enable timer */
	Timer->CTRL = 1;
	
	/* Return pointer */
	return Timer;
}

TM_DELAY_Timer_t* TM_DELAY_TimerReset(TM_DELAY_Timer_t* Timer) {
	/* Reset timer */
	Timer->CNT = Timer->ARR;
	
	/* Return pointer */
	return Timer;
}

TM_DELAY_Timer_t* TM_DELAY_TimerAutoReloadCommand(TM_DELAY_Timer_t* Timer, uint8_t AutoReloadCommand) {
	/* Set new auto reload command */
	Timer->AR = AutoReloadCommand;
	
	/* Return pointer */
	return Timer;
}

TM_DELAY_Timer_t* TM_DELAY_TimerAutoReloadValue(TM_DELAY_Timer_t* Timer, uint32_t AutoReloadValue) {
	/* Reset timer */
	Timer->ARR = AutoReloadValue;
	
	/* Return pointer */
	return Timer;
}

/* 1ms function called when systick makes interrupt */
__weak void TM_DELAY_1msHandler(void) {
  /* NOTE: This function Should not be modified, when the callback is needed,
           the TM_DELAY_1msHandler could be implemented in the user file
   */
}

/***************************************************/
/*       Custom HAL function implementations       */
/***************************************************/

/* Called from Systick handler */
void HAL_IncTick(void) {
	uint8_t i;
	
	/* Increase system time */
	TM_Time++;
	
	/* Decrease other system time */
	if (TM_Time2) {
		TM_Time2--;
	}
	
	/* Check for timers */
	/* Check custom timers */
	for (i = 0; i < CustomTimers.Count; i++) {
		/* Check if timer is enabled */
		if (
			CustomTimers.Timers[i] &&          /*!< Pointer exists */
			CustomTimers.Timers[i]->CTRL &&    /*!< Timer is enabled */
			CustomTimers.Timers[i]->CNT > 0    /*!< Counter is not NULL */
		) {
			/* Decrease counter */
			CustomTimers.Timers[i]->CNT--;
			
			/* Check if count is zero */
			if (CustomTimers.Timers[i]->CNT == 0) {
				/* Call user callback function */
				CustomTimers.Timers[i]->Callback(CustomTimers.Timers[i], CustomTimers.Timers[i]->UserParameters);
				
				/* Set new counter value */
				CustomTimers.Timers[i]->CNT = CustomTimers.Timers[i]->ARR;
				
				/* Disable timer if auto reload feature is not used */
				if (!CustomTimers.Timers[i]->AR) {
					/* Disable counter */
					CustomTimers.Timers[i]->CTRL = 0;
				}
			}
		}
	}
	
	/* Call 1ms interrupt handler function */
	TM_DELAY_1msHandler();
}

uint32_t HAL_GetTick(void) {
	return TM_Time;
}
