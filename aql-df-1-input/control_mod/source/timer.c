#include "../include/timer.h"

#if SIMULATOR_ON
#include <time.h>
#include <errno.h>
#else
static u32_t last_tick;
#endif

u32_t
timer_now_ms(none_t)
{
#if SIMULATOR_ON
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (u32_t)((uint64_t)now.tv_sec * 1000U + now.tv_nsec / 1000000U);
#else
	return TIM2_CNT;
#endif
}

none_t
timer_delay_ms(u32_t ms)
{
#if SIMULATOR_ON
	struct timespec req;
	req.tv_sec = ms / 1000;
	req.tv_nsec = (ms % 1000) * 1000000;
	while(nanosleep(&req, &req) != 0 && errno == EINTR) {}
#else
	u32_t start = timer_now_ms();
	while((u32_t)(timer_now_ms() - start) < ms) {}
#endif
}

none_t
timer_init(none_t)
{
#if !SIMULATOR_ON
	/* Free-running milliseconds; call before motion_sensor_init(). */
	RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;
	(void)RCC_APB1ENR;
	TIM2_CR1 = 0;
	TIM2_PSC = TIMER_PSC_VALUE;
	TIM2_ARR = TIMER_ARR_VALUE;
	TIM2_CNT = 0;
	TIM2_EGR = TIM_EGR_UG;
	TIM2_SR = 0;
	TIM2_CR1 = TIM_CR1_CEN;
	last_tick = timer_now_ms();
#endif
}

u8_t
timer_ready(none_t)
{
#if !SIMULATOR_ON
	return (u32_t)(timer_now_ms() - last_tick) >= (1000U / CONTROL_HZ);
#else
	return 1;
#endif
}

none_t
timer_clear(none_t)
{
#if !SIMULATOR_ON
	last_tick = timer_now_ms();
#endif
}
