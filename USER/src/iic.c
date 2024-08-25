#include "main.h"

void iic_scl_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}

void iic_sda_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}

void iic_sda_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}

void iic_start(void)
{
	iic_scl_out();
	iic_sda_out();
	IIC_SCL_H;
	IIC_SDA_H;
	delay_us(4);
	IIC_SDA_L;
	delay_us(4);
	IIC_SCL_L;
}

void iic_stop(void)
{
	iic_scl_out();
	iic_sda_out();
	IIC_SCL_H;
	IIC_SDA_L;
	delay_us(4);
	IIC_SDA_H;
	delay_us(4);
}

void iic_send_byte(u8 data)
{
	iic_scl_out();
	iic_sda_out();
	for(u8 i = 0;i < 8;i++)
	{
		IIC_SCL_L;
		if((data & (1 << (7 - i))))
			IIC_SDA_H;
		else
			IIC_SDA_L;	
		delay_us(4);
		IIC_SCL_H;	
		delay_us(4);
	}
	delay_us(4);
	IIC_SCL_L;
	IIC_SDA_L;
}

u8 iic_recv_byte(void)
{
	u8 data;
	iic_scl_out();
	iic_sda_in();
	for(u8 i = 0;i < 8;i++)
	{
		IIC_SCL_L;
		delay_us(4);
		IIC_SCL_H;	
		if(IIC_SDA_IN)
			data |= 1 << (7 - i);
		else
			data &= ~(1 << (7 - i));
		delay_us(4);
	}
	IIC_SCL_L;
	return data;
}

void iic_send_ack(u8 ack)
{
	iic_scl_out();
	iic_sda_out();
	IIC_SCL_L;
	
	if(ack)
		IIC_SDA_H;
	else
		IIC_SDA_L;
	delay_us(4);
	IIC_SCL_H;
	delay_us(4);
	IIC_SCL_L;
}

u8 iic_recv_ack(void)
{
	u8 ack;
	iic_scl_out();
	iic_sda_in();
	IIC_SCL_L;
	delay_us(4);
	IIC_SCL_H;
	if(IIC_SDA_IN)
		ack = 1;
	else
		ack = 0;
	delay_us(4);
	IIC_SCL_L;
	return ack;
}

