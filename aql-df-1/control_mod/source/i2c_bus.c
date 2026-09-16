#include "../include/i2c_bus.h"


/*
	This is real hardware when
	SIMILATOR_ON==0
	***

*/

#define RCC_BASE		0x40023800UL
#define GPIOB_BASE		0X40020400UL
#define I2C1_BASE		0x40005400UL


#define RCC_AHB1ENR		REG32(RCC_BASE + 0x30)
#define RCC_APB1ENR		REG32(RCC_BASE + 0x40)
#define RCC_APB1RSTR	REG32(RCC_BASE + 0x20)

#define GPIOB_MODER		REG32(GPIOB_BASE + 0x00)
#define GPIOB_OTYPER	REG32(GPIOB_BASE + 0x04)
#define GPIOB_OSPEEDR	REG32(GPIOB_BASE + 0x08)
#define GPIOB_PUPDR		REG32(GPIOB_BASE + 0x0c)
#define GPIOB_AFRL		REG32(GPIOB_BASE + 0x20)

#define I2C1_CR1		REG32(I2C1_BASE + 0x00)
#define I2C1_CR2		REG32(I2C1_BASE + 0x04)
#define I2C1_OAR1		REG32(I2C1_BASE + 0x08)
#define I2C1_DR			REG32(I2C1_BASE + 0x10)
#define I2C1_SR1		REG32(I2C1_BASE + 0x14)
#define I2C1_SR2		REG32(I2C1_BASE + 0x18)
#define I2C1_CCR		REG32(I2C1_BASE + 0x1C)
#define I2C1_TRISE		REG32(I2C1_BASE + 0x20)

#define RCC_AHB1ENR_GPIOBEN		(1U << 01)
#define RCC_APB1ENR_I2C1EN		(1U << 21)
#define RCC_APB1RSTR_I2C1RST	(1U << 21)

#define I2C_CR1_PE				(1U << 0)
#define I2C_CR1_START			(1U << 8)
#define I2C_CR1_STOP			(1U << 9)
#define I2C_CR1_ACK				(1U << 10)

#define I2C_SR1_SB				(1U << 0)
#define I2C_SR1_ADDR			(1U << 1)
#define I2C_SR1_BTF				(1U << 2)
#define I2C_SR1_RXNE			(1U << 6)
#define I2C_SR1_TXE				(1U << 7)
#define I2C_SR1_AF				(1U << 10)
#define I2C_SR2_BUSY			(1U << 01)

#define I2C_WRITE				0U
#define I2C_READ				1U

#define I2C_APB1_MHZ			16U
#define I2C_CCR_100KHZ			80U
#define I2C_TRISE_100KHZ		17U

#define I2C_TIMEOUT				100000UL



#if !SIMULATOR_ON

static u8_t
i2c_bus_wait_sr1_set(u32_t mask)
{
	u32_t timeout = I2C_TIMEOUT;

	while(!(I2C1_SR1 & mask))
	{
		if(I2C1_SR1 & I2C_SR1_AF)
		{
			I2C1_SR1 &= ~I2C_SR1_AF;
			I2C1_CR1 |= I2C_CR1_STOP;
			return (u8_t)0;
		}
		if((timeout--) == 0) 
		{
			I2C1_CR1 |= I2C_CR1_STOP;
			return (u8_t)0;
		}
	}

	return (u8_t)1;
}

static u8_t
i2c_bus_wait_sr2_clear(u32_t mask)
{
	u32_t timeout = I2C_TIMEOUT;
	while(I2C1_SR2 & mask)
	if((timeout--) == 0) return (u8_t)0;
	return (u8_t)1;
}

static none_t
i2c_bus_clear_addr(none_t)
{
	volatile u32_t temp;
	temp = I2C1_SR1;
	temp = I2C1_SR2;
	(none_t)temp;
	return;
}

#endif



none_t
i2c_bus_init(none_t)
{
#if !SIMULATOR_ON

	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;

	RCC_APB1RSTR |= RCC_APB1RSTR_I2C1RST;	
	RCC_APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;	


	GPIOB_MODER &= ~((3U << (6U * 2U)) | (3U << (7U * 2U)));
	GPIOB_MODER |= ((2U << (6U * 2U)) | (2U << (7U * 2U)));

	GPIOB_OTYPER |= (1U << 6U) | (1U << 7U);

	GPIOB_OSPEEDR &= ~((3U << (6U * 2U)) | (3U << (7U * 2U)));
	GPIOB_OSPEEDR |= ((2U << (6U * 2U)) | (2U << (7U * 2U)));

	GPIOB_PUPDR &= ~((3U << (6U * 2U)) | (3U << (7U * 2U)));
	GPIOB_PUPDR |=  ((1U << (6U * 2U)) | (1U << (7U * 2U)));

	GPIOB_AFRL &= ~((0xFU << (6U * 4U)) | (0xFU << (7U * 4U)));
	GPIOB_AFRL |=  ((4U   << (6U * 4U)) | (4U   << (7U * 4U)));

	I2C1_CR1 &= ~I2C_CR1_PE;
	
	I2C1_CR2 = I2C_APB1_MHZ;
	I2C1_CCR = I2C_CCR_100KHZ;
	I2C1_TRISE = I2C_TRISE_100KHZ;

	I2C1_OAR1 = (1U << 14U);

	I2C1_CR1 |= I2C_CR1_PE;
#endif	
	return;
}

u8_t
i2c_bus_read_u8(u8_t device_addr, u8_t reg_addr)
{
	u8_t value = (u8_t)0;
	
#if !SIMULATOR_ON



	if(!i2c_bus_wait_sr2_clear(I2C_SR2_BUSY)) return (u8_t)0;


	// Start and device write address
	I2C1_CR1 |= I2C_CR1_START;
	if(!i2c_bus_wait_sr1_set(I2C_SR1_SB)) return (u8_t)0;

	I2C1_DR = (u8_t)((device_addr << 1U) | I2C_WRITE);
	if(!i2c_bus_wait_sr1_set(I2C_SR1_ADDR)) return (u8_t)0;
	i2c_bus_clear_addr();

	// Send reg addr
	if(!i2c_bus_wait_sr1_set(I2C_SR1_TXE)) return (u8_t)0;
	I2C1_DR = reg_addr;
	if(!i2c_bus_wait_sr1_set(I2C_SR1_BTF)) return (u8_t)0;

	// Repeated start and device read addr
	I2C1_CR1 |= I2C_CR1_START;
	if(!i2c_bus_wait_sr1_set(I2C_SR1_SB)) return (u8_t)0;

	I2C1_DR = (u8_t)((device_addr << 1U) | I2C_READ);
	if(!i2c_bus_wait_sr1_set(I2C_SR1_ADDR)) return (u8_t)0;

	// Single byte read, disable ACK, clear ADDR, send STOP, then read DR

	I2C1_CR1 &= ~I2C_CR1_ACK;
	i2c_bus_clear_addr();
	I2C1_CR1 |= I2C_CR1_STOP;

	if(!i2c_bus_wait_sr1_set(I2C_SR1_RXNE)) return (u8_t)0;
	value = (u8_t)I2C1_DR;

	I2C1_CR1 |= I2C_CR1_ACK;


#else
	(none_t)device_addr;	
	(none_t)reg_addr;	
#endif
	
	return value;
}

none_t
i2c_bus_write_u8(u8_t device_addr, u8_t reg_addr, u8_t value)
{
#if !SIMULATOR_ON

	if(!i2c_bus_wait_sr2_clear(I2C_SR2_BUSY)) return;

	// Start and device write addr
	I2C1_CR1 |= I2C_CR1_START;
	if(!i2c_bus_wait_sr1_set(I2C_SR1_SB)) return;

	I2C1_DR = (u8_t)((device_addr << 1U) | I2C_WRITE);
	if(!i2c_bus_wait_sr1_set(I2C_SR1_ADDR)) return;
	i2c_bus_clear_addr();

	// Sned reg addr
	if(!i2c_bus_wait_sr1_set(I2C_SR1_TXE)) return;
	I2C1_DR = reg_addr;

	// Sned value
	if(!i2c_bus_wait_sr1_set(I2C_SR1_TXE)) return;
	I2C1_DR = value;

	// Wait until done, then stop
	if(!i2c_bus_wait_sr1_set(I2C_SR1_BTF)) return;
	I2C1_CR1 |= I2C_CR1_STOP;


#else
	(none_t)device_addr;
	(none_t)reg_addr;
	(none_t)value;
#endif	
	return;
}
