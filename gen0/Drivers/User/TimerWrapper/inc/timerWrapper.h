/*
 * timerWrapper.h
 *
 *  Created on: May 18, 2025
 *      Author: aad
 */

#ifndef USER_TIMERWRAPPER_INC_TIMERWRAPPER_H_
#define USER_TIMERWRAPPER_INC_TIMERWRAPPER_H_


#include "stm32f407xx.h" // where TIM_TYpeDef is defined
#include "stm32f4xx_ll_tim.h"

extern uint32_t SystemCoreClock;

#define xu32TimerWrapper_getFreq(__POINTER__, __RET__)                              \
	do {                                                                            \
		uint32_t const timer_clk = SystemCoreClock /2;                                \
		uint32_t const prescaler = LL_TIM_GetPrescaler(__POINTER__);               \
		uint32_t const arr = LL_TIM_GetAutoReload(__POINTER__);                    \
		(*(uint32_t *)(__RET__)) = timer_clk / ((prescaler + 1) * (arr + 1));      \
	} while (0)


#define xvTimerWrapper_SetFreq(__POINTER__, __TARGET_FREQ__, __IS_16BIT_TIMER__)    \
	do {                                                                            \
		__disable_irq();                                                            \
		LL_TIM_DisableCounter(__POINTER__);                                         \
                                                                                    \
		uint32_t const timer_clk = SystemCoreClock / 2; /* STM32F4 APB Timers run at PCLKx*2 if APBx_Prescaler != 1, else PCLKx. User stated 84MHz, SystemCoreClock usually 168MHz */ \
		uint32_t prescaler_val = 0;                                                 \
		uint32_t arr_val = 0;                                                       \
		uint64_t temp_arr_calc; /* Use 64-bit for intermediate calculation */       \
                                                                                    \
		uint32_t const max_arr = (__IS_16BIT_TIMER__) ? 0xFFFF : 0xFFFFFFFF;        \
                                                                                    \
		if ((__TARGET_FREQ__) == 0) { /* Avoid division by zero, set to lowest possible freq */ \
			prescaler_val = 0xFFFF;                                                 \
			arr_val = max_arr;                                                      \
		} else {                                                                    \
			/* Calculate initial ARR with prescaler 0 */                            \
			temp_arr_calc = (uint64_t)timer_clk / (__TARGET_FREQ__);                \
                                                                                    \
			/* If initial ARR with PSC=0 is 0, it means target freq is too high. */ \
            /* Smallest ARR is 0 (counter counts 0 to 0). temp_arr_calc must be at least 1. */ \
			if (temp_arr_calc == 0) temp_arr_calc = 1;                              \
                                                                                    \
			prescaler_val = (uint32_t)((temp_arr_calc -1) / (max_arr + 1));         \
            if (prescaler_val > 0xFFFF) prescaler_val = 0xFFFF; /* Cap prescaler */ \
                                                                                    \
			arr_val = (uint32_t)(((temp_arr_calc) / (prescaler_val + 1)) - 1);       \
                                                                                    \
			/* Final checks and adjustments */                                      \
			if (arr_val > max_arr) arr_val = max_arr;                               \
            /* If target frequency is too high, resulting arr_val might be -1 (underflow) */ \
            /* Set to 0, which is the smallest possible reload value. */ \
            if (arr_val == (uint32_t)-1 && (__TARGET_FREQ__) > (timer_clk / (prescaler_val + 1))) { \
                arr_val = 0; \
            } \
		}                                                                           \
                                                                                    \
		LL_TIM_SetPrescaler(__POINTER__, prescaler_val);                            \
		LL_TIM_SetAutoReload(__POINTER__, arr_val);                                 \
		/* Ensure the counter is reset to apply new ARR value if the timer is running */ \
		/* And prescaler values are latched at update event */ \
		LL_TIM_SetCounter(__POINTER__, 0);                                          \
		LL_TIM_GenerateEvent_UPDATE(__POINTER__); /* Load prescaler & ARR */         \
		LL_TIM_ClearFlag_UPDATE(__POINTER__);     /* Clear the update flag */        \
		LL_TIM_EnableCounter(__POINTER__);                                          \
		__enable_irq();                                                             \
	} while (0)


/*
#define xu32TimerWrapper_getFreq(__POINTER__, __RET__)							\
	do {																		\
		uint32_t const  timer_clk = SystemCoreClock;							\
		uint32_t const prescaler = LL_TIM_GetPrescaler(__POINTER__);			\
        uint32_t const arr = LL_TIM_GetAutoReload(__POINTER__);                 \
		(*(uint32_t *)(__RET__)) = timer_clk / ((prescaler + 1) * (arr + 1));   \
	} while (0)


#define xvTimerWrapper_SetFreq(__POINTER__, __FREQ)							\
	do {																	\
		__disable_irq();													\
		LL_TIM_DisableCounter(__POINTER__);									\
		uint32_t const  timer_clk = SystemCoreClock;						\
		uint32_t const prescaler = LL_TIM_GetPrescaler(__POINTER__);		\
		const uint32_t arr = (timer_clk / ((prescaler + 1) * __FREQ)) - 1;	\
		LL_TIM_SetPrescaler(__POINTER__, prescaler);						\
		LL_TIM_SetAutoReload(__POINTER__, arr);								\
		LL_TIM_SetCounter(__POINTER__, 0);									\
		LL_TIM_ClearFlag_UPDATE(__POINTER__);								\
		LL_TIM_EnableCounter(__POINTER__);									\
		__enable_irq();														\
	} while (0)

*/


#endif /* USER_TIMERWRAPPER_INC_TIMERWRAPPER_H_ */
