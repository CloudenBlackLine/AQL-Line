#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "../../global/aqldf1.h"

none_t	i2c_bus_init		(none_t);
u8_t	i2c_bus_read_u8		(u8_t, u8_t);
none_t	i2c_bus_write_u8	(u8_t, u8_t, u8_t);

#endif
