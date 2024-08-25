#ifndef _IIC_H
#define _IIC_H

#include "stm32f4xx.h"

#define IIC_SCL_H		GPIO_SetBits(GPIOF, GPIO_Pin_2)
#define IIC_SCL_L		GPIO_ResetBits(GPIOF, GPIO_Pin_2)
#define IIC_SDA_H		GPIO_SetBits(GPIOF, GPIO_Pin_3)
#define IIC_SDA_L		GPIO_ResetBits(GPIOF, GPIO_Pin_3)
#define IIC_SCL_OUT		iic_scl_port_init()
#define IIC_SDA_IN		GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3)
#define IIC_SDA_OUT		iic_sda_output()
#define IIC_SDA_INPUT	iic_sda_input()

void iic_scl_port_init(void);
void iic_sda_output(void);
void iic_sda_input(void);
void iic_start(void);
void iic_stop(void);
void iic_send_byte(u8 data);
u8 iic_recv_byte(void);
void iic_send_ack(u8 ack);
u8 iic_recv_ack(void);

#endif

