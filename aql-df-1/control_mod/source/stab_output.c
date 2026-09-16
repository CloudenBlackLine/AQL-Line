#include "../include/stab_output.h"
#include "../include/pwm_output.h"


static Pwm_Output mixer = {0};


#define SERVO_CENTER_US 1500
#define SERVO_SAFE_MIN_US 1100
#define SERVO_SAFE_MAX_US 1900



static s32_t
clamp_s32(s32_t value, s32_t max, s32_t min)
{
	if(value > max) return max;
	if(value < min) return min;
	return value;
}

none_t
stab_output_update(s32_t pitch_new, s32_t roll_new, s32_t thrust_new)
{
	(void)thrust_new;

	// For know just roll testing, then will add rest
	// for simulation

	mixer.roll_vector = (u16_t)(clamp_s32((roll_new + SERVO_CENTER_US), SERVO_SAFE_MAX_US, SERVO_SAFE_MIN_US));
	mixer.pitch_vector = (u16_t)(clamp_s32((pitch_new + SERVO_CENTER_US), SERVO_SAFE_MAX_US, SERVO_SAFE_MIN_US));
	


	// Do the converting from PID controller
	// Values to raw servo and thrust (ESC) values (PWM) 
	return;	
}

none_t
stab_output_write()
{
	pwm_output_update(mixer);
	pwm_output_write();
	return;
}
