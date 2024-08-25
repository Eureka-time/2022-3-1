#ifndef _RELAY_H
#define _RELAY_H

#include "stm32f4xx.h"

#define AIR_ON		GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define AIR_OFF		GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define DEHUM_ON	GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define DEHUM_OFF	GPIO_ResetBits(GPIOA,GPIO_Pin_6)

void relay_init(void);

#endif