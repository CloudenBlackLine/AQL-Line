#include "../include/timer.h"


#if SIMULATOR_ON
#include <time.h>
none_t
timer_delay_ms(u32_t ms)
{
	struct timespec req;
	req.tv_sec = ms / 1000;
	req.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&req, 0);
}
#endif


none_t
timer_init(none_t)
{
#if !SIMULATOR_ON
	RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2_CR1 = 0;
	TIM2_PSC = TIMER_PSC_VALUE;
	TIM2_ARR = TIMER_ARR_VALUE;
	TIM2_CNT = 0;
	TIM2_EGR = TIM_EGR_UG;
	TIM2_SR &= ~TIM_SR_UIF;
	TIM2_CR1 |= TIM_CR1_CEN;
#endif
	return;	
}

u8_t
timer_ready(none_t)
{
#if !SIMULATOR_ON
	if(TIM2_SR & TIM_SR_UIF) return 1;
	return 0;
#else
	return 1;
#endif
}

none_t
timer_clear(none_t)
{
#if !SIMULATOR_ON
	TIM2_SR &= ~TIM_SR_UIF;
#endif
	return;
}

