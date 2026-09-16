#include "../include/pwm_output.h"
#if SIMULATOR_ON
#include "../simulator/simulator.h"
#endif


// Put defines for output registers here
// This is the File for it to be located.


static Pwm_Output pwm = {0};


none_t
pwm_output_update(Pwm_Output pwm_f)
{
	pwm = pwm_f;
}



none_t
pwm_output_write()
{
#if SIMULATOR_ON
	simulator_output_result(pwm);
#else
	
#endif
	
	// write/simulate to servos, thrust (ESC)
	// Threw registers or whatever you call it.
}
