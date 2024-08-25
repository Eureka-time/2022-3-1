#ifndef	_USART_H
#define _USART_H

#include "stm32f4xx.h"

void usart1_init(u32 baud);
void usart1_send_byte(u8 data);
u8 usart1_recv_byte(void);
void usart1_send_string(u8 *buf);
void usart1_recv_string(u8 *buf);
void usart3_init(u32 baud);
void usart3_send_byte(u8 data);
u8 usart3_recv_byte(void);
void usart3_send_string(u8 *buf);
void usart3_recv_string(u8 *buf);

#endif