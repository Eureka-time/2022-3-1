#ifndef _SHT20_H
#define _SHT20_H

#include "stm32f4xx.h"

#define T_CMD	0xE3
#define H_CMD	0xE5


void get_sht20_data(float *t,float *h);
float sht20_read_data(u8 cmd);

#endif
