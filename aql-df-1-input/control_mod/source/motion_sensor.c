#include "../include/motion_sensor.h"
#include "../include/i2c_bus.h"
#include "../include/timer.h"

#define MPU9250_EXPECTED_WHOAMI	0x71
#define MPU9250_WHO_AM_I		0x75
#define MPU9250_PWR_MGMT_1	0x6b
#define MPU9250_PWR_MGMT_2	0x6c
#define MPU9250_SMPLRT_DIV	0x19
#define MPU9250_CONFIG		0x1a
#define MPU9250_GYRO_CONFIG	0x1b
#define MPU9250_ACCEL_CONFIG	0x1c
#define MPU9250_ACCEL_CONFIG_2	0x1d
#define MPU9250_INT_PIN_CFG	0x37
#define MPU9250_INT_ENABLE	0x38
#define MPU9250_INT_STATUS	0x3a
#define MPU9250_ACCEL_XOUT_H	0x3b

volatile u8_t imu_whoami = 0;
volatile Imu_Status imu_status = IMU_NOT_READY;

static u8_t initialized;
static u8_t has_sample;
static u32_t last_sample_ms;

static Imu_Status
motion_sensor_bus_status(I2c_Status status)
{
	switch(status)
	{
		case I2C_OK: return IMU_OK;
		case I2C_NO_ACK: return IMU_NO_ACK;
		case I2C_TIMEOUT: return IMU_TIMEOUT;
		case I2C_CLOCK_ERROR: return IMU_CLOCK_ERROR;
		case I2C_NOT_READY: return IMU_NOT_READY;
		case I2C_BAD_ARG: return IMU_BAD_ARG;
		default: return IMU_BUS_ERROR;
	}
}

static Imu_Status
motion_sensor_read_u8(u8_t reg_addr, u8_t* value)
{
	return motion_sensor_bus_status(i2c_bus_read_u8(MOTION_SENSOR_ADDR, reg_addr, value));
}

static Imu_Status
motion_sensor_write_u8(u8_t reg_addr, u8_t value)
{
	return motion_sensor_bus_status(i2c_bus_write_u8(MOTION_SENSOR_ADDR, reg_addr, value));
}

static Imu_Status
motion_sensor_config(u8_t reg_addr, u8_t value)
{
	u8_t actual;
	Imu_Status status = motion_sensor_write_u8(reg_addr, value);
	if(status != IMU_OK) return status;
	status = motion_sensor_read_u8(reg_addr, &actual);
	if(status != IMU_OK) return status;
	return actual == value ? IMU_OK : IMU_CONFIG_ERROR;
}

static s16_t
motion_sensor_read_s16(const u8_t* data)
{
	u16_t value = ((u16_t)data[0] << 8) | data[1];
	return (s16_t)((value & 0x8000U) ? (s32_t)value - 65536 : (s32_t)value);
}

Imu_Status
motion_sensor_init(none_t)
{
	static const u8_t config[][2] =
	{
		{MPU9250_PWR_MGMT_1, 0x01},
		{MPU9250_PWR_MGMT_2, 0x00},
		{MPU9250_CONFIG, 0x03},
		{MPU9250_SMPLRT_DIV, 0x09},
		{MPU9250_GYRO_CONFIG, 0x00},
		{MPU9250_ACCEL_CONFIG, 0x00},
		{MPU9250_ACCEL_CONFIG_2, 0x03},
		{MPU9250_INT_PIN_CFG, 0x00},
		{MPU9250_INT_ENABLE, 0x01}
	};
	u8_t value;
	initialized = 0;
	has_sample = 0;
	imu_whoami = 0;
	imu_status = motion_sensor_bus_status(i2c_bus_init());
	if(imu_status != IMU_OK) return imu_status;

	timer_delay_ms(100);
	imu_status = motion_sensor_read_u8(MPU9250_WHO_AM_I, &value);
	if(imu_status != IMU_OK) return imu_status;
	imu_whoami = value;
	if(value != MPU9250_EXPECTED_WHOAMI) return imu_status = IMU_WRONG_DEVICE;

	imu_status = motion_sensor_write_u8(MPU9250_PWR_MGMT_1, 0x80);
	if(imu_status != IMU_OK) return imu_status;
	timer_delay_ms(100);

	/* 100 Hz, filtered accel/gyro, +/-2 g and +/-250 degrees/second. */
	for(u32_t i = 0; i < sizeof(config) / sizeof(config[0]); i++)
	{
		imu_status = motion_sensor_config(config[i][0], config[i][1]);
		if(imu_status != IMU_OK) return imu_status;
	}
	timer_delay_ms(100);
	/* Clear startup data-ready; update() waits for a new sample. */
	imu_status = motion_sensor_read_u8(MPU9250_INT_STATUS, &value);
	if(imu_status == IMU_OK) initialized = 1;
	return imu_status;
}

Imu_Status
motion_sensor_update(Motion_Sensor* out)
{
	u8_t data[15];
	Motion_Sensor sample;
	if(!out) return imu_status = IMU_BAD_ARG;
	if(!initialized) return imu_status;

	/* Status and all measurements in one transaction. */
	imu_status = motion_sensor_bus_status(i2c_bus_read(MOTION_SENSOR_ADDR,
		MPU9250_INT_STATUS, data, sizeof(data)));
	if(imu_status != IMU_OK) return imu_status;
	if(!(data[0] & 1U)) return imu_status = IMU_NOT_READY;

	sample.accel_x = motion_sensor_read_s16(&data[1]);
	sample.accel_y = motion_sensor_read_s16(&data[3]);
	sample.accel_z = motion_sensor_read_s16(&data[5]);
	/* Bytes 7 and 8 are temperature. */
	sample.gyro_x = motion_sensor_read_s16(&data[9]);
	sample.gyro_y = motion_sensor_read_s16(&data[11]);
	sample.gyro_z = motion_sensor_read_s16(&data[13]);
	*out = sample;
	last_sample_ms = timer_now_ms();
	has_sample = 1;
	return imu_status;
}

u8_t
motion_sensor_stale(none_t)
{
	return !has_sample || (u32_t)(timer_now_ms() - last_sample_ms) >= MOTION_SENSOR_STALE_MS;
}
