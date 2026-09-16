#ifndef INCLUDE_H
#define INCLUDE_H

#include "../../global/aqldf1.h"
#include "../include/motion_sensor.h"
#include "../include/pwm_output.h"

none_t	simulator_init			(none_t);
none_t	simulator_update		(Motion_Sensor*);
none_t	simulator_output_result	(Pwm_Output);


#endif
