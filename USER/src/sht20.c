#include "main.h"

float sht20_read_data(u8 cmd)
{
	u8 buf[5];
	float t_or_h;
	u16 data = 0;
	iic_start();
	iic_send_byte(0x80);
	if(iic_recv_ack())
	{
		iic_stop();
		return 0;
	}
	iic_send_byte(cmd);
	if(iic_recv_ack())
	{
		iic_stop();
		return 0;
	}
	iic_start();
	iic_send_byte(0x81);
	if(iic_recv_ack())
	{
		iic_stop();
		return 0;
	}
	delay_ms(100);
	buf[0] = iic_recv_byte();
	iic_send_ack(0);
	buf[1] = iic_recv_byte();
	iic_send_ack(0);
	buf[3] = iic_recv_byte();
	iic_send_ack(1);
	iic_stop();
	data = (buf[0] << 8) | buf[1];
	if(cmd == T_CMD)
		t_or_h = 175.72 * data / 65536 - 46.85;
	else if(cmd == H_CMD)
		t_or_h = (125.0 * data / 65536 - 6.0);
	return t_or_h;
}

void get_sht20_data(float *t,float *h)
{
	*t = sht20_read_data(T_CMD);
	*h = sht20_read_data(H_CMD);
}
