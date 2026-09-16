#include "../include/motion_sensor.h"
#include "../include/i2c_bus.h"
#include "../include/timer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static u8_t registers[128];
static u32_t now;
static I2c_Status bus_result;
static u8_t reject_config;
static u32_t burst_count;

u32_t timer_now_ms(none_t) { return now; }
none_t timer_delay_ms(u32_t ms) { now += ms; }
I2c_Status i2c_bus_init(none_t) { return bus_result; }

I2c_Status
i2c_bus_read_u8(u8_t address, u8_t reg, u8_t* value)
{
	assert(address == MOTION_SENSOR_ADDR);
	if(bus_result != I2C_OK) return bus_result;
	*value = registers[reg];
	if(reg == 0x3a) registers[reg] = 0;
	return I2C_OK;
}

I2c_Status
i2c_bus_write_u8(u8_t address, u8_t reg, u8_t value)
{
	assert(address == MOTION_SENSOR_ADDR);
	if(bus_result != I2C_OK) return bus_result;
	if(reg == 0x6b && value == 0x80)
	{
		u8_t identity = registers[0x75];
		memset(registers, 0, sizeof(registers));
		registers[0x75] = identity;
	}
	else if(!(reject_config && reg == 0x19)) registers[reg] = value;
	return I2C_OK;
}

I2c_Status
i2c_bus_read(u8_t address, u8_t reg, u8_t* data, u16_t count)
{
	assert(address == MOTION_SENSOR_ADDR);
	assert(reg == 0x3a && count == 15);
	burst_count++;
	/* A failed transaction may already have filled part of the buffer. */
	if(bus_result != I2C_OK)
	{
		memset(data, 0xff, 7);
		return bus_result;
	}
	memcpy(data, &registers[reg], count);
	registers[0x3a] = 0;
	return I2C_OK;
}

static none_t
reset(none_t)
{
	memset(registers, 0, sizeof(registers));
	registers[0x75] = 0x71;
	now = 0;
	bus_result = I2C_OK;
	reject_config = 0;
	burst_count = 0;
}

static none_t
sample_ready(none_t)
{
	static const u8_t bytes[14] =
	{
		0x80, 0x00, 0x7f, 0xff, 0xff, 0xff,
		0x12, 0x34,
		0x00, 0x00, 0x00, 0x01, 0xff, 0xfe
	};
	registers[0x3a] = 1;
	memcpy(&registers[0x3b], bytes, sizeof(bytes));
}

int
main(void)
{
	Motion_Sensor sample = {11, 22, 33, 44, 55, 66};
	Motion_Sensor saved = sample;
	assert(motion_sensor_update(&sample) == IMU_NOT_READY);
	assert(motion_sensor_stale());

	reset();
	registers[0x75] = 0x70;
	assert(motion_sensor_init() == IMU_WRONG_DEVICE);
	assert(motion_sensor_update(&sample) == IMU_WRONG_DEVICE);
	assert(memcmp(&sample, &saved, sizeof(sample)) == 0);

	reset();
	bus_result = I2C_NO_ACK;
	assert(motion_sensor_init() == IMU_NO_ACK);
	reset();
	bus_result = I2C_CLOCK_ERROR;
	assert(motion_sensor_init() == IMU_CLOCK_ERROR);
	reset();
	reject_config = 1;
	assert(motion_sensor_init() == IMU_CONFIG_ERROR);

	reset();
	assert(motion_sensor_init() == IMU_OK);
	assert(now == 300);
	assert(registers[0x19] == 9 && registers[0x1a] == 3);
	assert(registers[0x1b] == 0 && registers[0x1c] == 0);
	assert(registers[0x1d] == 3 && registers[0x38] == 1);
	assert(motion_sensor_update(&sample) == IMU_NOT_READY);
	assert(memcmp(&sample, &saved, sizeof(sample)) == 0);
	assert(motion_sensor_update(0) == IMU_BAD_ARG);

	sample_ready();
	assert(motion_sensor_update(&sample) == IMU_OK);
	assert(sample.accel_x == -32768 && sample.accel_y == 32767);
	assert(sample.accel_z == -1 && sample.gyro_x == 0);
	assert(sample.gyro_y == 1 && sample.gyro_z == -2);
	assert(burst_count == 2);
	assert(!motion_sensor_stale());
	saved = sample;
	assert(motion_sensor_update(&sample) == IMU_NOT_READY);
	assert(memcmp(&sample, &saved, sizeof(sample)) == 0);

	static const I2c_Status errors[] = {I2C_NO_ACK, I2C_TIMEOUT, I2C_BUS_ERROR};
	static const Imu_Status expected[] = {IMU_NO_ACK, IMU_TIMEOUT, IMU_BUS_ERROR};
	for(u32_t i = 0; i < 3; i++)
	{
		bus_result = errors[i];
		assert(motion_sensor_update(&sample) == expected[i]);
		assert(memcmp(&sample, &saved, sizeof(sample)) == 0);
	}
	now += 99;
	assert(!motion_sensor_stale());
	now++;
	assert(motion_sensor_stale());
	bus_result = I2C_OK;
	sample_ready();
	assert(motion_sensor_update(&sample) == IMU_OK);
	assert(!motion_sensor_stale());

	now = UINT32_MAX - 20;
	sample_ready();
	assert(motion_sensor_update(&sample) == IMU_OK);
	now += 99;
	assert(!motion_sensor_stale());
	now++;
	assert(motion_sensor_stale());
	puts("IMU input tests passed");
	return 0;
}
