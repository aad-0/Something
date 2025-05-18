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
		uint32_t const timer_clk = SystemCoreClock;                                \
		uint32_t const prescaler = LL_TIM_GetPrescaler(__POINTER__);               \
		uint32_t const arr = LL_TIM_GetAutoReload(__POINTER__);                    \
		(*(uint32_t *)(__RET__)) = timer_clk / ((prescaler + 1) * (arr + 1));      \
	} while (0)


#define xvTimerWrapper_SetFreq(__POINTER__, __FREQ)                                 \
	do {                                                                            \
		__disable_irq();                                                            \
		LL_TIM_DisableCounter(__POINTER__);                                         \
                                                                                    \
		uint32_t const timer_clk = SystemCoreClock;                                 \
		uint32_t const prescaler = 0; /* <-- You can change or parameterize this */ \
		uint32_t const arr = (timer_clk / ((__FREQ) * (prescaler + 1))) - 1;        \
                                                                                    \
		LL_TIM_SetPrescaler(__POINTER__, prescaler);                                \
		LL_TIM_SetAutoReload(__POINTER__, arr);                                     \
		LL_TIM_SetCounter(__POINTER__, 0);                                          \
		LL_TIM_GenerateEvent_UPDATE(__POINTER__); /* Ensures new ARR is applied */  \
		LL_TIM_ClearFlag_UPDATE(__POINTER__);                                       \
		LL_TIM_EnableCounter(__POINTER__);                                          \
		__enable_irq();                                                              \
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
