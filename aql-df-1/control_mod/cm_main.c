/*
 * Clouden Blackline Source Code
 * Author: Judah McCloud	
 * Project: AQL-LINE
 */


#include "./include/controller.h"
#include "../global/aqldf1.h"
#include "./include/timer.h"

#if SIMULATOR_ON
	#include "./simulator/simulator.h"
#endif



none_t
cm_main(none_t)
{
	controller_init();
	timer_init();
	

#if SIMULATOR_ON
	simulator_init();
#endif

	while(1)
	{
		if(timer_ready())
		{
			timer_clear();
#if SIMULATOR_ON
			timer_delay_ms(40);
#endif
			controller_input();
			controller_update();
			controller_output();
		}
	}		
	return;
}



s32_t
main(none_t)
{
	cm_main();
	return 0;
}
