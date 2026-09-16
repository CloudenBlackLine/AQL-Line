#ifndef ASDF1_H
#define ASDF1_H

#include <stdint.h>


#ifndef SIMULATOR_ON
#define SIMULATOR_ON 		1
#endif

#ifndef AQL_DEBUG
#define AQL_DEBUG			0
#endif


#if SIMULATOR_ON
#include <stdio.h>
#endif


#ifndef SIMULATOR_ON
	#error "SIMULATOR_ON must be defined 0 or 1"
#endif


typedef uint32_t			u32_t;
typedef uint16_t			u16_t;
typedef uint8_t				u8_t;

typedef int32_t				s32_t;
typedef int16_t				s16_t;
typedef int8_t				s8_t;

typedef void				none_t;


#define REG32(addr) (*(volatile u32_t*)	(addr))
#define REG16(addr) (*(volatile u16_t*)	(addr))
#define REG8(addr) 	(*(volatile u8_t*)	(addr))


#endif
