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
#if !defined(STM32F0xx)
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
#else
	/* Return OK */
	return 1;
#endif
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
	tmp->Flags.F.AREN = AutoReloadCmd;
	tmp->Flags.F.CNTEN = StartTimer;
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
	Timer->Flags.F.CNTEN = 0;
	
	/* Return pointer */
	return Timer;
}

TM_DELAY_Timer_t* TM_DELAY_TimerStart(TM_DELAY_Timer_t* Timer) {
	/* Enable timer */
	Timer->Flags.F.CNTEN = 1;
	
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
	Timer->Flags.F.AREN = AutoReloadCommand ? 1 : 0;

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
			CustomTimers.Timers[i] &&             /*!< Pointer exists */
			CustomTimers.Timers[i]->Flags.F.CNTEN /*!< Timer is enabled */
		) {
			/* Decrease counter if needed */
			if (CustomTimers.Timers[i]->CNT) {
				CustomTimers.Timers[i]->CNT--;
			}

			/* Check if count is zero */
			if (CustomTimers.Timers[i]->CNT == 0) {
				/* Call user callback function */
				CustomTimers.Timers[i]->Callback(CustomTimers.Timers[i], CustomTimers.Timers[i]->UserParameters);

				/* Set new counter value */
				CustomTimers.Timers[i]->CNT = CustomTimers.Timers[i]->ARR;

				/* Disable timer if auto reload feature is not used */
				if (!CustomTimers.Timers[i]->Flags.F.AREN) {
					/* Disable counter */
					CustomTimers.Timers[i]->Flags.F.CNTEN = 0;
				}
			}
		}
	}
	
	/* Call 1ms interrupt handler function */
	TM_DELAY_1msHandler();
}

void HAL_Delay(uint32_t Delay) {
	/* Delay for amount of milliseconds */
	/* Check if we are called from ISR */
	if (__get_IPSR() == 0) {
		/* Called from thread mode */
		uint32_t tickstart = HAL_GetTick();
		
		/* Count interrupts */
		while ((HAL_GetTick() - tickstart) < Delay) {
#ifdef DELAY_SLEEP
			/* Go sleep, wait systick interrupt */
			__WFI();
#endif
		}
	} else {
		/* Called from interrupt mode */
		while (Delay) {
			/* Check if timer reached zero after we last checked COUNTFLAG bit */
			if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
				Delay--;
			}
		}
	}
}

uint32_t HAL_GetTick(void) {
	/* Return current time in milliseconds */
	return TM_Time;
}
