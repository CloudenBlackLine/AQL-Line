#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "../../global/aqldf1.h"

typedef enum
{
	I2C_OK,
	I2C_NO_ACK,
	I2C_TIMEOUT,
	I2C_BUS_ERROR,
	I2C_BAD_ARG,
	I2C_CLOCK_ERROR,
	I2C_NOT_READY
} I2c_Status;

I2c_Status	i2c_bus_init		(none_t);
I2c_Status	i2c_bus_read_u8		(u8_t, u8_t, u8_t*);
/* One byte or a burst of at least three bytes. Two-byte reads are unused. */
I2c_Status	i2c_bus_read		(u8_t, u8_t, u8_t*, u16_t);
I2c_Status	i2c_bus_write_u8	(u8_t, u8_t, u8_t);

#endif
