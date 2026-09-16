#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H


#include "../../global/aqldf1.h"


typedef struct
{
	u16_t pitch_vector;
	u16_t roll_vector;
	u16_t thrust_vector;
} Pwm_Output;


none_t	pwm_output_update	(Pwm_Output);
none_t	pwm_output_write	(none_t);


#endif
