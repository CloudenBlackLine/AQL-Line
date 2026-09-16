#include "./simulator.h"
#include "../include/timer.h"

#include <stdlib.h>
#include <time.h>



static s32_t sim_roll_angle = -300;
static s32_t sim_roll_rate = 20;
static s32_t last_roll_pwm = 1500;

static s32_t sim_pitch_angle = 300;
static s32_t sim_pitch_rate = 20;
static s32_t last_pitch_pwm = 1500;


static s32_t
sim_clamp_s32(s32_t value, s32_t min, s32_t max)
{
	if(value > max) return max;
	if(value < min) return min;
	return value;
}


none_t
simulator_init(none_t)
{
	srand((u32_t)time(0));
	printf("<init Simluator> -> COMPLETE\n");
	return;
}

none_t
simulator_update(Motion_Sensor* raw_data)
{
	s32_t t_roll_servo_error 	= last_roll_pwm - 1500;
	s32_t wind_push_roll 		= (rand() % 7) - 3;

	s32_t t_pitch_servo_error 	= last_pitch_pwm - 1500;
	s32_t wind_push_pitch 		= (rand() % 7) - 3;


	sim_roll_rate += t_roll_servo_error / 50;
	sim_roll_rate += wind_push_roll;
	sim_roll_rate -= sim_roll_rate / 80;
	sim_roll_rate = (sim_roll_rate * 9) / 10;

	sim_roll_angle += sim_roll_rate;

	raw_data->accel_x = sim_roll_angle;
	raw_data->gyro_x = sim_roll_rate;



	sim_pitch_rate += t_pitch_servo_error / 50;
	sim_pitch_rate += wind_push_pitch;
	sim_pitch_rate -= sim_pitch_rate / 80;
	sim_pitch_rate = (sim_pitch_rate * 9) / 10;

	sim_pitch_angle += sim_pitch_rate;

	raw_data->accel_y = sim_pitch_angle;
	raw_data->gyro_y = sim_pitch_rate;
	return;
}


none_t
simulator_output_result(Pwm_Output pwm)
{
	const s32_t LINE = 51;
	const s32_t CENTER = LINE / 2;

	s32_t roll_offset;
	s32_t roll_pos;

	s32_t pitch_offset;
	s32_t pitch_pos;

	last_roll_pwm = pwm.roll_vector;

	roll_offset = (last_roll_pwm - 1500) / 20;
	roll_pos = CENTER + roll_offset;
	roll_pos = sim_clamp_s32(roll_pos, 0, LINE - 1);

	last_pitch_pwm = pwm.pitch_vector;

	pitch_offset = (last_pitch_pwm - 1500) / 20;
	pitch_pos = CENTER + pitch_offset;
	pitch_pos = sim_clamp_s32(pitch_pos, 0, LINE - 1);

	

	printf("\033[2J\033[H");

	printf("===== AQL-DF-1 ROLL SIM =====\n\n");
/*
	printf("roll angle:  %d\n", sim_roll_angle);
	printf("roll rate:   %d\n", sim_roll_rate);
	printf("roll pwm:    %u us\n", (u32_t)pwm.roll_vector);
	
	printf("pitch angle:  %d\n", sim_pitch_angle);
	printf("pitch rate:   %d\n", sim_pitch_rate);
	printf("pitch pwm:    %u us\n", (u32_t)pwm.pitch_vector);
*/
	printf("roll vector:\t");

	s8_t P;

	for(s32_t i = 0; i < LINE; i++)
	{
		P = '=';
		if(i == roll_pos) P = '0';
		else if(i == CENTER) P = '|';
		putchar(P);
	}
	printf("\n\n");
/*	if(sim_roll_angle > 0) printf("state(ROLL): tilted RIGHT\n");
	else if(sim_roll_angle < 0) printf("state(ROLL): tilted LEFT\n");
	else printf("state(ROLL): level\n");
*/	printf("pitch vector:\t");


	for(s32_t i = 0; i < LINE; i++)
	{
		P = '=';
		if(i == pitch_pos) P = '0';
		else if(i == CENTER) P = '|';
		putchar(P);
	}
	printf("\n\n");
/*	if(sim_pitch_angle > 0) printf("state(PITCH): tilted RIGHT\n");
	else if(sim_pitch_angle < 0) printf("state(PITCH): tilted LEFT\n");
	else printf("state(PITCH): level\n");
*/



	for(s32_t y=0; y<(LINE/2); y++)
	{
		for(s32_t x=0; x<LINE; x++)
		{
			P = '`';
			if((roll_pos == x) && (pitch_pos/2 == y)) P = 'O';
			if((x == (LINE/2)) && (y == (LINE/4))) P = '+';
			putchar(P);
		}
		putchar('\n');
	}





	fflush(stdout);

	return;
}




/*

		PROBLEM: --

			Figure out how to create a simulated version so that the value changes
			based of some what real phys.

		;

*/
