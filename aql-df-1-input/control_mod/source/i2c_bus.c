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

#define I2C_TIMEOUT_MS		10U
#define RCC_CFGR			REG32(RCC_BASE + 0x08)
#define I2C_CR1_POS			(1U << 11)
#define I2C_SR1_BERR			(1U << 8)
#define I2C_SR1_ARLO			(1U << 9)
#define I2C_SR1_OVR			(1U << 11)
#define I2C_SR2_MSL			(1U << 0)
#define I2C_ERROR_MASK		(I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_OVR | I2C_SR1_AF)



#include "../include/timer.h"

#if !SIMULATOR_ON
static u8_t bus_ready;

static u32_t
i2c_bus_irq_save(none_t)
{
	u32_t state;
	__asm volatile ("mrs %0, primask\n\tcpsid i" : "=r"(state) :: "memory");
	return state;
}

static none_t
i2c_bus_irq_restore(u32_t state)
{
	__asm volatile ("msr primask, %0" :: "r"(state) : "memory");
}

static I2c_Status
i2c_bus_error(none_t)
{
	u32_t flags = I2C1_SR1;
	if(flags & (I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_OVR)) return I2C_BUS_ERROR;
	if(flags & I2C_SR1_AF) return I2C_NO_ACK;
	return I2C_OK;
}

static I2c_Status
i2c_bus_wait_sr1_set(u32_t mask)
{
	u32_t start = timer_now_ms();
	for(;;)
	{
		I2c_Status status = i2c_bus_error();
		if(status != I2C_OK) return status;
		if(I2C1_SR1 & mask) return I2C_OK;
		if((u32_t)(timer_now_ms() - start) >= I2C_TIMEOUT_MS) return I2C_TIMEOUT;
	}
}

static I2c_Status
i2c_bus_wait_idle(none_t)
{
	u32_t start = timer_now_ms();
	while((I2C1_CR1 & I2C_CR1_STOP) || (I2C1_SR2 & I2C_SR2_BUSY))
	{
		I2c_Status status = i2c_bus_error();
		if(status != I2C_OK) return status;
		if((u32_t)(timer_now_ms() - start) >= I2C_TIMEOUT_MS) return I2C_TIMEOUT;
	}
	return i2c_bus_error();
}

static none_t
i2c_bus_clear_addr(none_t)
{
	volatile u32_t temp;
	temp = I2C1_SR1;
	temp = I2C1_SR2;
	(none_t)temp;
}

static I2c_Status
i2c_bus_finish(I2c_Status status)
{
	if(status != I2C_OK)
	{
		if(I2C1_SR2 & I2C_SR2_MSL) I2C1_CR1 |= I2C_CR1_STOP;
		I2C1_SR1 &= ~I2C_ERROR_MASK;
		(void)i2c_bus_wait_idle();
		/* Reset our peripheral; a line held low still needs wiring/sensor recovery. */
		(void)i2c_bus_init();
	}
	I2C1_CR1 &= ~I2C_CR1_POS;
	I2C1_CR1 |= I2C_CR1_ACK;
	return status;
}

static I2c_Status
i2c_bus_address(u8_t address)
{
	I2c_Status status;
	I2C1_CR1 |= I2C_CR1_START;
	status = i2c_bus_wait_sr1_set(I2C_SR1_SB);
	if(status != I2C_OK) return status;
	I2C1_DR = address;
	return i2c_bus_wait_sr1_set(I2C_SR1_ADDR);
}

static I2c_Status
i2c_bus_select(u8_t device_addr, u8_t reg_addr)
{
	I2c_Status status = i2c_bus_wait_idle();
	if(status != I2C_OK) return status;
	I2C1_CR1 &= ~I2C_CR1_POS;
	I2C1_CR1 |= I2C_CR1_ACK;
	status = i2c_bus_address((u8_t)(device_addr << 1U));
	if(status != I2C_OK) return status;
	i2c_bus_clear_addr();
	status = i2c_bus_wait_sr1_set(I2C_SR1_TXE);
	if(status != I2C_OK) return status;
	I2C1_DR = reg_addr;
	return i2c_bus_wait_sr1_set(I2C_SR1_BTF);
}
#endif

I2c_Status
i2c_bus_init(none_t)
{
#if !SIMULATOR_ON
	bus_ready = 0;
	/* HSI system clock, no AHB/APB1 divider: both drivers expect 16 MHz. */
	if(RCC_CFGR & ((3U << 2) | (15U << 4) | (7U << 10))) return I2C_CLOCK_ERROR;
	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;
	(void)RCC_AHB1ENR;
	(void)RCC_APB1ENR;

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
	bus_ready = 1;
#endif
	return I2C_OK;
}

I2c_Status
i2c_bus_read(u8_t device_addr, u8_t reg_addr, u8_t* data, u16_t count)
{
	if(!data || !count || count == 2 || device_addr > 0x7f) return I2C_BAD_ARG;
#if !SIMULATOR_ON
	I2c_Status status;
	u32_t irq;
	if(!bus_ready) return I2C_NOT_READY;
	status = i2c_bus_select(device_addr, reg_addr);
	if(status != I2C_OK) return i2c_bus_finish(status);
	status = i2c_bus_address((u8_t)((device_addr << 1U) | I2C_READ));
	if(status != I2C_OK) return i2c_bus_finish(status);

	if(count == 1)
	{
		irq = i2c_bus_irq_save();
		I2C1_CR1 &= ~I2C_CR1_ACK;
		i2c_bus_clear_addr();
		I2C1_CR1 |= I2C_CR1_STOP;
		i2c_bus_irq_restore(irq);
		status = i2c_bus_wait_sr1_set(I2C_SR1_RXNE);
		if(status != I2C_OK) return i2c_bus_finish(status);
		*data = (u8_t)I2C1_DR;
	}
	else
	{
		i2c_bus_clear_addr();
		while(count > 3)
		{
			status = i2c_bus_wait_sr1_set(I2C_SR1_RXNE);
			if(status != I2C_OK) return i2c_bus_finish(status);
			*data++ = (u8_t)I2C1_DR;
			count--;
		}
		/* RM0368: finish the last three bytes with BTF and ACK disabled. */
		status = i2c_bus_wait_sr1_set(I2C_SR1_BTF);
		if(status != I2C_OK) return i2c_bus_finish(status);
		irq = i2c_bus_irq_save();
		I2C1_CR1 &= ~I2C_CR1_ACK;
		*data++ = (u8_t)I2C1_DR;
		i2c_bus_irq_restore(irq);
		status = i2c_bus_wait_sr1_set(I2C_SR1_BTF);
		if(status != I2C_OK) return i2c_bus_finish(status);
		irq = i2c_bus_irq_save();
		I2C1_CR1 |= I2C_CR1_STOP;
		*data++ = (u8_t)I2C1_DR;
		*data = (u8_t)I2C1_DR;
		i2c_bus_irq_restore(irq);
	}
	return i2c_bus_finish(i2c_bus_wait_idle());
#else
	(none_t)reg_addr;
	return I2C_NOT_READY;
#endif
}

I2c_Status
i2c_bus_read_u8(u8_t device_addr, u8_t reg_addr, u8_t* value)
{
	u8_t data;
	if(!value) return I2C_BAD_ARG;
	I2c_Status status = i2c_bus_read(device_addr, reg_addr, &data, 1);
	if(status == I2C_OK) *value = data;
	return status;
}

I2c_Status
i2c_bus_write_u8(u8_t device_addr, u8_t reg_addr, u8_t value)
{
	if(device_addr > 0x7f) return I2C_BAD_ARG;
#if !SIMULATOR_ON
	if(!bus_ready) return I2C_NOT_READY;
	I2c_Status status = i2c_bus_select(device_addr, reg_addr);
	if(status != I2C_OK) return i2c_bus_finish(status);
	I2C1_DR = value;
	status = i2c_bus_wait_sr1_set(I2C_SR1_BTF);
	if(status != I2C_OK) return i2c_bus_finish(status);
	I2C1_CR1 |= I2C_CR1_STOP;
	return i2c_bus_finish(i2c_bus_wait_idle());
#else
	(none_t)reg_addr;
	(none_t)value;
	return I2C_NOT_READY;
#endif
}
