#include "../include/motion_sensor.h"
#include "../include/i2c_bus.h"


#define MOTION_SENSOR_ADDR		0x68

#define MPU9250_EXPECTED_WHOAMI 0x71

#define MPU9250_PWR_MGMT_1		0x6b
#define MPU9250_WHO_AM_I		0x75


#define MPU9250_ACCEL_XOUT_H	0x3b
#define MPU9250_ACCEL_YOUT_H	0x3d
#define MPU9250_ACCEL_ZOUT_H	0x3f

#define MPU9250_GYRO_XOUT_H		0x43
#define MPU9250_GYRO_YOUT_H		0x45
#define MPU9250_GYRO_ZOUT_H		0x47


volatile u8_t imu_whoami = 0;


static Motion_Sensor raw_data;

static u8_t
motion_sensor_read_u8(u8_t reg_addr)
{
	return i2c_bus_read_u8(MOTION_SENSOR_ADDR, reg_addr);
}

static none_t
motion_sensor_write_u8(u8_t reg_addr, u8_t value)
{
	i2c_bus_write_u8(MOTION_SENSOR_ADDR, reg_addr, value);
	return;
}

static s16_t
motion_sensor_read_s16(u8_t high_reg_addr)
{
	u8_t high = motion_sensor_read_u8(high_reg_addr);
	u8_t low = motion_sensor_read_u8(high_reg_addr+1);

	return (s16_t)((high << 8) | low);
}



none_t
motion_sensor_init(none_t)
{
	i2c_bus_init();
	imu_whoami = motion_sensor_read_u8(MPU9250_WHO_AM_I);
	motion_sensor_write_u8(MPU9250_PWR_MGMT_1, 0x00);

	raw_data = (Motion_Sensor){0};
#if AQL_DEBUG
	printf("MPU9250 WHO_AM_I -> 0x%02x\n", imu_whoami);

	if(imu_whoami != MPU9250_EXPECTED_WHOAMI)
	printf("MPU9250 WHO_AM_I -> mismtch\n");

#endif
	return;
}

none_t
motion_sensor_update(none_t)
{
	raw_data.accel_x = motion_sensor_read_s16(MPU9250_ACCEL_XOUT_H);
	raw_data.accel_y = motion_sensor_read_s16(MPU9250_ACCEL_YOUT_H);
	raw_data.accel_z = motion_sensor_read_s16(MPU9250_ACCEL_ZOUT_H);
	
	raw_data.gyro_x = motion_sensor_read_s16(MPU9250_GYRO_XOUT_H);
	raw_data.gyro_y = motion_sensor_read_s16(MPU9250_GYRO_YOUT_H);
	raw_data.gyro_z = motion_sensor_read_s16(MPU9250_GYRO_ZOUT_H);
	return;
}

Motion_Sensor
motion_sensor_get(none_t)
{
	return raw_data;
}
