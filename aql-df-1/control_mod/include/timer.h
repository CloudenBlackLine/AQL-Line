#ifndef TIMER_H
#define TIMER_H


#include "../../global/aqldf1.h"


#define	RCC_BASE			0x40023800UL
#define	TIM2_BASE			0x40000000UL

#define	RCC_APB1ENR			REG32(RCC_BASE + 0x40)

#define	TIM2_CR1			REG32(TIM2_BASE + 0x00)
#define	TIM2_SR				REG32(TIM2_BASE + 0x10)
#define	TIM2_EGR			REG32(TIM2_BASE + 0x14)
#define	TIM2_CNT			REG32(TIM2_BASE + 0x24)
#define	TIM2_PSC			REG32(TIM2_BASE + 0x28)
#define	TIM2_ARR			REG32(TIM2_BASE + 0x2C)

#define	RCC_APB1ENR_TIM2EN	(1U << 0)

#define TIM_CR1_CEN			(1U << 0)
#define TIM_SR_UIF			(1U << 0)
#define TIM_EGR_UG			(1U << 0)

#define TIMER_CLOCK_HZ		16000000UL
#define TIMER_COUNT_HZ		10000UL
#define CONTROL_HZ			100UL

#define TIMER_PSC_VALUE		((TIMER_CLOCK_HZ / TIMER_COUNT_HZ) - 1UL)
#define TIMER_ARR_VALUE		((TIMER_COUNT_HZ / CONTROL_HZ) - 1UL)


#if SIMULATOR_ON
none_t	timer_delay_ms	(u32_t);
#endif
none_t	timer_init		(none_t);
u8_t	timer_ready		(none_t);
none_t	timer_clear		(none_t);


#endif
