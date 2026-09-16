#include "../include/controller.h"
#include "../include/stab_output.h"
#include "../include/motion_sensor.h"

#if SIMULATOR_ON
	#include "../simulator/simulator.h"
#endif

typedef struct
{
	Motion_Sensor raw_data;
	s32_t roll_angle;
	s32_t roll_error;
	s32_t roll_correction;
	s32_t roll_derivative;

	s32_t pitch_angle;
	s32_t pitch_error;
	s32_t pitch_correction;
	s32_t pitch_derivative;
} Controller;


#define TARGET_ROLL 0
#define TARGET_PITCH 0

#define KP_ROLL 4
#define KP_PITCH 5

#define KD_ROLL 4
#define KD_PITCH 5	


static Controller ctrl;


none_t
controller_init(none_t)
{
	motion_sensor_init();
	ctrl = (Controller){0};

#if AQL_DEBUG
	printf("<init Controller> -> COMPLETE\n");
#endif
	return;
}


none_t
controller_input(none_t)
{

#if SIMULATOR_ON
	simulator_update(&ctrl.raw_data);
	ctrl.roll_angle = ctrl.raw_data.accel_x;
	ctrl.pitch_angle = ctrl.raw_data.accel_y;
#else
	motion_sensor_update();
	ctrl.raw_data = motion_sensor_get();
	// ^^^^^^^
	// For know this, but will create a file
	// to hold a func that will give a realistic input.
#endif
	
#if AQL_DEBUG
	printf("==========\naccel(x) = %d\ngyro(x) = %d\n", ctrl.raw_data.accel_x, ctrl.raw_data.gyro_x);
	printf("==========\naccel(y) = %d\ngyro(y) = %d\n", ctrl.raw_data.accel_y, ctrl.raw_data.gyro_y);
#endif
	return;
}

none_t
controller_update(none_t)
{
	// P calucation

	ctrl.roll_error 		= (TARGET_ROLL - ctrl.roll_angle);
	ctrl.roll_derivative 	= -ctrl.raw_data.gyro_x;

	ctrl.roll_correction 	= (KP_ROLL*ctrl.roll_error) + (KD_ROLL*ctrl.roll_derivative);

	ctrl.pitch_error 		= (TARGET_PITCH - ctrl.pitch_angle);
	ctrl.pitch_derivative 	= -ctrl.raw_data.gyro_y;
	
	ctrl.pitch_correction 	= (KP_PITCH*ctrl.pitch_error) + (KD_PITCH*ctrl.pitch_derivative);

	// D calulcation

	

#if AQL_DEBUG
	printf("==========\nroll(error) = %d\nroll(correction) = %d\n", ctrl.roll_error, ctrl.roll_correction);
	printf("==========\npitch(error) = %d\npitch(correction) = %d\n", ctrl.pitch_error, ctrl.pitch_correction);
#endif
	return;
}

none_t
controller_output(none_t)
{
	stab_output_update(ctrl.pitch_correction, ctrl.roll_correction, 0);
	stab_output_write();
	return;
}
