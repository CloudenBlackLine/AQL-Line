#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include "../../global/aqldf1.h"

typedef struct
{
	s16_t accel_x, accel_y, accel_z;
	s16_t gyro_x, gyro_y, gyro_z;
} Motion_Sensor;

none_t			motion_sensor_init		(none_t);
none_t			motion_sensor_update	(none_t);
Motion_Sensor	motion_sensor_get		(none_t);

#endif
