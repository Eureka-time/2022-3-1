#ifndef _TIM_H
#define _TIM_H

#include "stm32f4xx.h"

typedef struct
{
	u8 year[10];
	u8 month[10];
	u8 day[10];
	u8 hour[10];
	u8 min[10];
	u8 sec[10];
}_TIME;

extern _TIME _time;

void tim3_delay_ms(u16 ms);
void tim3_delay_s(u32 s);
void tim3_delay_us(u32 us);
void tim6_init(u16 arr,u16 psc);

#endif