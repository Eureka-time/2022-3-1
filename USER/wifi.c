#include "stm32f4xx.h"
#include "wifi.h"
#include "timer.h"
#include "string.h"
#include <stdio.h>
//#include "OLED.h"

typedef struct{
	char buf[1024]; //���ڴ���WIFIоƬ���һ����Ӧ������
	u32  count;//���ڱ�ʾ���һ�ν��յ��ֽ���������������
	u8   flag;//��־��0��ʾ���һ����Ӧ���ڽ����У�1��ʾ���һ����Ӧ�Ѿ��������
}UartRecv;

UartRecv u2recv={{0},0,0};

int global_h,global_m,global_s;//ȫ�ֱ���  ʱ����
u8 global_wifista=0;//0��ʾδ���� ��1��ʾ����
extern const u8 pic_erweima[];


/*
��������my_arraystr
����	���ڶ���ַ���������Ѱ��Ŀ���ַ���
����	����
����ֵ����
��ע	��
		  
*/
char* my_arraystr(const char*array,u32 size,const char*str)
{
	u32 i=0;
	char*p=NULL;
	while(i<size)
	{
		p=strstr(array ,str );
		if(p!=NULL)
			return p;
		else//��ǰ����������ȡ���ַ�����û����Ҫ�����ݣ����ҳ��������һ���ַ���
		{
			i+=strlen(array)+1; //��¼�������Ѿ������ֽ���
			array+=strlen(array)+1;//���ʵ���һ���ַ���
		}
	}
}

/*
��������wifi_send_at
����	��wifiģ�鷢��ATָ��
����	��const char*str       ���͵��ַ��� ATָ��
			const char *ack		�ڴ���Ӧ��
			u32 timeout_ms  	�ȴ�ʱ��	
����ֵ��������ʽ
��ע	��
		  
*/
WIFI_Error wifi_send_at(const char*str,const char *ack, u32 timeout_ms)
{
	u32 t=timeout_ms/100;//��100������Ϊʱ����
	usart3_send_string(str);
	while(t--)
	{
		wifi_delayms(100);
		if(strstr (u2recv.buf,ack)!=NULL)//��ʾĿǰ���յ��������Ѿ�������������Ӧ��
		{
			
			//����ȴ�һ��ʱ�䣬��Ϊ�ڴ���ack��һ���Ǳ��ν��������е���������
			u2recv.flag=1;//���������Ѿ���ȫ����
			return WIIF_ERR_NONE;
		}
	}
	//��ʱ�����
	if(my_arraystr(u2recv.buf,u2recv.count,ack)!=NULL)
	{
		//u2recv.count=0;
		return WIIF_ERR_NONE;
	}
	u2recv.flag = 1;
	if(u2recv.count == 0)
		return  WIFI_ERR_USART;
	else
		return  WIFI_ERR_AT;
}

/*
������  �� wifi_init
����	��wifi��ʼ��
����	��

����ֵ��������ʽ
��ע	��
		  
*/
WIFI_Error wifi_init()
{
	WIFI_Error err=WIIF_ERR_NONE;
	//�ӿڳ�ʼ��
	uart3_init(115200);
	uart3_irq_config();
	
	printf("\r\n--��λ--\r\n");
	//WiFi��λ
	if(WIIF_ERR_NONE!=wifi_send_at("AT+RST\r\n","WIFI GOT IP",5000))
	{
		global_wifista=0;
		err=WIIF_ERR_RST;
		
		printf("\r\n--��λʧ��--\r\n");
		goto EXIT;
	}
	global_wifista=1;
	printf("\r\n--��λ-OK-\r\n");
	
	//WiFi����sationģʽ
//	printf("\r\n--WIIF_2_ok--\r\n");
//	if(WIIF_ERR_NONE!=wifi_send_at("AT+CWMODE_CUR=1\r\n","OK",2000))
//	{
//		err=WIIF_ERR_CWMODE;
//		
//		printf("\r\n--WIIF_ERR_CWMODE--\r\n");
//		goto EXIT;
//	}
	
	//����·����
//	printf("\r\n--WIIF_3_ok--\r\n");
//	if(WIIF_ERR_NONE!=wifi_send_at("AT+CWJAP_CUR=\"FAST_zhou\",\"ZZ51921YGR\"\r\n","OK",10000))
//	{
//		err=WIIF_ERR_CWJAP;
//		
//		printf("\r\n--WIIF_ERR_CWJAP--\r\n");
//		goto EXIT;
//	}
	

	
EXIT:
	return err;
}

/*
��������wifi_smartconfig
����	����������
����	����
����ֵ����������
��ע	��
*/
//WIFI_Error wifi_smartconfig()
//{
//	//����
//	OLED_disString(80,2,"ɨ��");
//	OLED_disString(80,4,"����");
//	//�������ʾ��ά��
//	OLED_drawPic(0,0,64,64,(u8 *)pic_erweima);
//	//ִ����������ָ��
//	if(WIIF_ERR_NONE!=wifi_send_at("AT+CWSTARTSMART\r\n","WIFI GOT IP",20000))
//	{
//		global_wifista=0;
//		printf("ɨ������ʧ��\r\n");
//		return  WIIF_ERR_ERWEIMA;
//	}
//	global_wifista=1;
//	printf("ɨ�������ɹ�\r\n");
//	return WIIF_ERR_NONE;
//}




/*
��������wifi_getNetTime
����	���������ʱ��
����	����
����ֵ����������
��ע	��
*/
WIFI_Error wifi_getNetTime()
{
	int i;
	char *p = NULL;
	WIFI_Error err=WIIF_ERR_NONE;
	
	//����͸��ģʽ
	printf("\r\n--����͸��ģʽ--\r\n");
	if(WIIF_ERR_NONE!=wifi_send_at("AT+CIPMODE=1\r\n","OK",2000))
	{
		
		err=WIIF_ERR_CIPMODE;
		printf("\r\n--����͸��ģʽʧ��--\r\n");
		goto EXIT;
	}
	printf("\r\n----����͸��ģʽ_OK----\r\n");
	
	//�����Ա�������
	printf("\r\n--�����Ա�������--\r\n");
	if(WIIF_ERR_NONE!=wifi_send_at("AT+CIPSTART=\"TCP\",\"api.m.taobao.com\",80\r\n","OK",2000))
	{
		//���������Ѵ�������״̬���ٴ�����ʱ������Ӧ"ALREADY CONNECTED"
		if(NULL == my_arraystr(u2recv.buf , u2recv.count , "ALREADY CONNECTED") )
		{
			err=WIIF_ERR_CIPSTART;
			printf("\r\n--�����Ա�������ʧ��--\r\n");
			goto EXIT;
		}
	}
	printf("\r\n----�����Ա�������_OK----\r\n");
	
	//���뷢��״̬
	printf("\r\n--���뷢��״̬--\r\n");
	if(WIIF_ERR_NONE!=wifi_send_at("AT+CIPSEND\r\n",">",2000))
	{
		err=WIIF_ERR_CIPSEND;
		
		printf("\r\n--���뷢��״̬ʧ��--\r\n");
		goto EXIT;
	}
	
	//��ȡʱ��
	printf("\r\n--��ȡʱ��--\r\n");
	if(WIIF_ERR_NONE != wifi_send_at("GET http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp HTTP/1.0\r\n\r\n","}}",5000))
	{
		err=WIIF_ERR_GETTIME;
		
		printf("\r\n--��ȡʱ��ʧ��--\r\n");
		goto EXIT;
	}
	printf("\r\n----��ȡʱ��_OK----\r\n");
	//����ʱ��   global_h , global_m , global_s
	p = my_arraystr(u2recv.buf , u2recv.count , "Date:");
	if(p == NULL)
	{
		printf("time error\r\n");
		goto EXIT;
	}
	//"Date: Fri, 04 Mar 2022 07:03:49 GMT"
	p+=23;//ʹָ��ǡ��ָ��ʱ����
	sscanf(p,"%d:%d:%d GMT",&global_h,&global_m,&global_s);
	global_h += 8;
	printf("time = %d:%d:%d\r\n",global_h , global_m , global_s);
	
	//wifi_delayms(1000);
	
	//����3��'+',ʹWIFIģ���˳�����״̬
	uart3_sendByte('+');
	uart3_sendByte('+');
	uart3_sendByte('+');
EXIT:	
	return err;
}

/*
��������UART3_Init
����	��UART��ʼ������  ��������Ϊ���ù���
����	����
����ֵ����
��ע	��PB10    USART_TX  �����������
		  PB11    USART_RX  ��������/����������
		  ����Ƶ�ʣ�84MHZ��

*/
void uart3_init(u32 baudrate)
{
	GPIO_InitTypeDef   	gpio_InitStruct ;
	USART_InitTypeDef 	usart_InitStruct;
	/*��PB10�� PB11���ó�USART3�Ĺ�������*/
	//����GPIOB
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB , ENABLE );
	//GPIOx�⺯����ʼ��������ģʽ
	gpio_InitStruct.GPIO_Mode 	=	GPIO_Mode_AF ;
	gpio_InitStruct.GPIO_OType 	= GPIO_OType_PP ;
	gpio_InitStruct.GPIO_Pin 		= GPIO_Pin_10 | GPIO_Pin_11 ;
	gpio_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL ;
	gpio_InitStruct.GPIO_Speed 	=	GPIO_Low_Speed ;
	GPIO_Init (GPIOB , &gpio_InitStruct);
	//ѡ��PA9��PA10�ĸ��ù���
	GPIO_PinAFConfig (GPIOB, GPIO_PinSource10 , GPIO_AF_USART3);
	GPIO_PinAFConfig (GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/*����USARTx*/
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE);
	
	/*���������ã�ѡ����*/
	//USART_OverSampling8Cmd (USART1, ENABLE);
	
	/*USARTx�⺯����ʼ��*/
	usart_InitStruct.USART_BaudRate							=	baudrate ;
	usart_InitStruct.USART_HardwareFlowControl 	=	USART_HardwareFlowControl_None ;
	usart_InitStruct.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx ;
	usart_InitStruct.USART_Parity 							= USART_Parity_No ;
	usart_InitStruct.USART_StopBits 						= USART_StopBits_1 ;    //ֹͣλ1
	usart_InitStruct.USART_WordLength 					=	USART_WordLength_8b ;  //����λ8
	USART_Init (USART3, &usart_InitStruct);
	
	/*ʹ��USART����*/
	USART_Cmd (USART3, ENABLE);
}

/*********************************
��������		uart3_sendByte
����������	u8 Data  1.���͵�����
����ֵ��		��
�������ܣ�	����3����һ�ֽ�����
��ע��			
*********************************/
//ͨ��uart3����һ֡���ݣ����ȴ��������
void uart3_sendByte(char data)
{
	USART_SendData (USART3, data);
	while(!USART_GetFlagStatus (USART3, USART_FLAG_TXE));
}

/*********************************
��������		uart3_recvByte
����������	��
����ֵ��		u8 1.���յ�����
�������ܣ�	����3����һ�ֽ�����
��ע��			
*********************************/
//�ȴ���׼���ý������ݣ�ͨ��uart1����һ֡���ݣ�
char uart3_recvByte(void)
{
	while(!USART_GetFlagStatus (USART3, USART_FLAG_RXNE));
	return USART_ReceiveData (USART3);
}

/*********************************
��������		usart3_send_string
����������	��
����ֵ��		��
�������ܣ�	����3���Ͷ��ֽ�����
��ע��			
*********************************/
void usart3_send_string(const char * str)
{
	while(*str != '\0')		//0
	{
		uart3_sendByte(*str++);
		//ָ��ƫ��1���ֽڣ�ָ����һ���ַ�
	}
  uart3_sendByte('\0');
}

/*********************************
��������		usart3_recv_string
����������	��
����ֵ��		��
�������ܣ�	����3���ն��ֽ�����
��ע��			
*********************************/
//��UART1�Ͻ���һ���ַ���
void usart3_recv_string( char *p)
{
	char data;
#ifndef	XCOM
	do{
		data = uart1_recvByte();
		*p = data;
		p++;
	}
	while(data != '\0');//data // data != 0
#else
	do{
		data = uart3_recvByte();
		*p = data;
		p++;
	}
	while(data != '\n');//��ΪXCOM���ܷ���'\0'��ֻ�ܷ��͵�'\r' '\n'��
	*p = '\0';
#endif
}

/*********************************
��������		uart3_irq_config
����������	
����ֵ��		��
�������ܣ�	����3�ж�
��ע��			
*********************************/
void uart3_irq_config(void)
{
	NVIC_InitTypeDef nvic_InitStruct;
	//����uart1�ڡ���׼���������ݡ��¼��󣬷����ж�����
	USART_ITConfig (USART3, USART_IT_RXNE, ENABLE);
	//NVIC�ڽ�������UART1���ж�����󣬷����ж�
	nvic_InitStruct.NVIC_IRQChannel									= USART3_IRQn;
	nvic_InitStruct.NVIC_IRQChannelCmd								= ENABLE;
	nvic_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//��ռ2λ
	nvic_InitStruct.NVIC_IRQChannelSubPriority 				= 1;//�Ƚ�2λ
	NVIC_Init (&nvic_InitStruct);
}

/*
��������UART3_�жϴ�����
����	��
����	����
����ֵ����
��ע	��
		  
*/
void USART3_IRQHandler(void)
{
	if( USART_GetITStatus (USART3, USART_IT_RXNE) )
	{
		if(u2recv.flag == 1)
		{
			u2recv.count = 0;
			u2recv.flag = 0;
		}
		//������Ϊ
		u2recv.buf[u2recv.count++] = USART_ReceiveData (USART3);
	}
}
/*******************************************************************************************/