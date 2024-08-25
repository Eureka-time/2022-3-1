#include "main.h"

void usart1_init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA,  &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	USART_Cmd(USART1, ENABLE);
}

void usart1_send_byte(u8 data)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART1->DR = data;
}

u8 usart1_recv_byte(void)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
	return USART1->DR;
}

void usart1_send_string(u8 *buf)
{
	while(*buf != '\0')
	{
		usart1_send_byte(*buf);
		buf++;
	}
}

void usart1_recv_string(u8 *buf)
{
	while(1)
	{
		*buf = usart1_recv_byte();
		if(*buf == '\0' || *buf == '\n' || *buf == '$')
			break;
		buf++;
	}
	*buf = '\0';
}

void usart3_init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB,  &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStruct);
	USART_Cmd(USART3, ENABLE);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
//	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
}

void usart3_send_byte(u8 data)
{
	while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
	USART3->DR = data;
}

u8 usart3_recv_byte(void)
{
	while(!USART_GetFlagStatus(USART3, USART_FLAG_RXNE));
	return USART3->DR;
}

void usart3_send_string(u8 *buf)
{
	while(*buf != '\0')
	{
		usart3_send_byte(*buf);
		buf++;
	}
}

void usart3_recv_string(u8 *buf)
{
	while(1)
	{
		*buf = usart3_recv_byte();
		if(*buf == '\0' || *buf == '\n' || *buf == '$')
			break;
		buf++;
	}
	*buf = '\0';
}


void USART3_IRQHandler(void)
{
	u8 re;
	static u16 point = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		re = USART3->DR;
		if(re == 0)
			wifi.rx_buf[point++] = ' ';
		else
			wifi.rx_buf[point++] = re;
	}
	else if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
	{
		re = USART3->SR;
		re = USART3->DR;
		wifi.rx_buf[point] = '\0';
		wifi.flag = 1;
		point = 0;
	}
}
#pragma import(__use_no_semihosting_swi) 														//取消半主机状态

struct __FILE { int handle;/* Add whatever you need here */ };
FILE __stdout;

int fputc(int ch, FILE *f) {
	usart1_send_byte(ch);																							// 更换成自己的字符发送函数
  return (ch);
}
int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int ch) {
  usart1_send_byte(ch);																							// 更换成自己的字符发送函数
}

void _sys_exit(int x)
{
	x = x;
}