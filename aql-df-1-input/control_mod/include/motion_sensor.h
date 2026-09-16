#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include "../../global/aqldf1.h"

#ifndef MOTION_SENSOR_ADDR
#define MOTION_SENSOR_ADDR	0x68
#endif

#define MOTION_SENSOR_STALE_MS	100U

typedef struct
{
	s16_t accel_x, accel_y, accel_z;
	s16_t gyro_x, gyro_y, gyro_z;
} Motion_Sensor;

typedef enum
{
	IMU_OK,
	IMU_NOT_READY,
	IMU_WRONG_DEVICE,
	IMU_NO_ACK,
	IMU_TIMEOUT,
	IMU_BUS_ERROR,
	IMU_CONFIG_ERROR,
	IMU_CLOCK_ERROR,
	IMU_BAD_ARG
} Imu_Status;

extern volatile u8_t imu_whoami;
extern volatile Imu_Status imu_status;

Imu_Status	motion_sensor_init	(none_t);
/* Only changes *out when a complete fresh sample was read successfully. */
Imu_Status	motion_sensor_update	(Motion_Sensor* out);
u8_t		motion_sensor_stale	(none_t);

#endif
