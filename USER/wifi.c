#include "stm32f4xx.h"
#include "wifi.h"
#include "timer.h"
#include "string.h"
#include <stdio.h>
//#include "OLED.h"

typedef struct{
	char buf[1024]; //用于储存WIFI芯片最近一次响应的数据
	u32  count;//用于表示最近一次接收的字节数（持续增长）
	u8   flag;//标志，0表示最近一次响应还在接收中；1表示最近一次响应已经接收完毕
}UartRecv;

UartRecv u2recv={{0},0,0};

int global_h,global_m,global_s;//全局变量  时分秒
u8 global_wifista=0;//0表示未联网 ，1表示联网
extern const u8 pic_erweima[];


/*
函数名：my_arraystr
功能	：在多个字符串数组中寻找目标字符串
参数	：无
返回值：无
备注	：
		  
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
		else//当前从数组中提取的字符串中没有想要的内容，则找出数组的下一个字符串
		{
			i+=strlen(array)+1; //记录数组中已经访问字节数
			array+=strlen(array)+1;//访问到下一个字符串
		}
	}
}

/*
函数名：wifi_send_at
功能	：wifi模块发送AT指令
参数	：const char*str       发送的字符串 AT指令
			const char *ack		期待的应答
			u32 timeout_ms  	等待时间	
返回值：错误形式
备注	：
		  
*/
WIFI_Error wifi_send_at(const char*str,const char *ack, u32 timeout_ms)
{
	u32 t=timeout_ms/100;//以100毫秒作为时间间隔
	usart3_send_string(str);
	while(t--)
	{
		wifi_delayms(100);
		if(strstr (u2recv.buf,ack)!=NULL)//表示目前接收到的内容已经包含所期望的应答
		{
			
			//建议等待一段时间，因为期待的ack不一定是本次接受数据中的最后的内容
			u2recv.flag=1;//本次内容已经完全接收
			return WIIF_ERR_NONE;
		}
	}
	//超时情况下
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
函数名  ： wifi_init
功能	：wifi初始化
参数	：

返回值：错误形式
备注	：
		  
*/
WIFI_Error wifi_init()
{
	WIFI_Error err=WIIF_ERR_NONE;
	//接口初始化
	uart3_init(115200);
	uart3_irq_config();
	
	printf("\r\n--软复位--\r\n");
	//WiFi软复位
	if(WIIF_ERR_NONE!=wifi_send_at("AT+RST\r\n","WIFI GOT IP",5000))
	{
		global_wifista=0;
		err=WIIF_ERR_RST;
		
		printf("\r\n--软复位失败--\r\n");
		goto EXIT;
	}
	global_wifista=1;
	printf("\r\n--软复位-OK-\r\n");
	
	//WiFi进入sation模式
//	printf("\r\n--WIIF_2_ok--\r\n");
//	if(WIIF_ERR_NONE!=wifi_send_at("AT+CWMODE_CUR=1\r\n","OK",2000))
//	{
//		err=WIIF_ERR_CWMODE;
//		
//		printf("\r\n--WIIF_ERR_CWMODE--\r\n");
//		goto EXIT;
//	}
	
	//连接路由器
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
函数名：wifi_smartconfig
功能	：智能配网
参数	：无
返回值：出错类型
备注	：
*/
//WIFI_Error wifi_smartconfig()
//{
//	//屏显
//	OLED_disString(80,2,"扫码");
//	OLED_disString(80,4,"配网");
//	//屏左侧显示二维码
//	OLED_drawPic(0,0,64,64,(u8 *)pic_erweima);
//	//执行智能配网指令
//	if(WIIF_ERR_NONE!=wifi_send_at("AT+CWSTARTSMART\r\n","WIFI GOT IP",20000))
//	{
//		global_wifista=0;
//		printf("扫码联网失败\r\n");
//		return  WIIF_ERR_ERWEIMA;
//	}
//	global_wifista=1;
//	printf("扫码联网成功\r\n");
//	return WIIF_ERR_NONE;
//}




/*
函数名：wifi_getNetTime
功能	：获得网络时间
参数	：无
返回值：出错类型
备注	：
*/
WIFI_Error wifi_getNetTime()
{
	int i;
	char *p = NULL;
	WIFI_Error err=WIIF_ERR_NONE;
	
	//进入透传模式
	printf("\r\n--进入透传模式--\r\n");
	if(WIIF_ERR_NONE!=wifi_send_at("AT+CIPMODE=1\r\n","OK",2000))
	{
		
		err=WIIF_ERR_CIPMODE;
		printf("\r\n--进入透传模式失败--\r\n");
		goto EXIT;
	}
	printf("\r\n----进入透传模式_OK----\r\n");
	
	//连接淘宝服务器
	printf("\r\n--连接淘宝服务器--\r\n");
	if(WIIF_ERR_NONE!=wifi_send_at("AT+CIPSTART=\"TCP\",\"api.m.taobao.com\",80\r\n","OK",2000))
	{
		//若服务器已处于连接状态，再次连接时，会响应"ALREADY CONNECTED"
		if(NULL == my_arraystr(u2recv.buf , u2recv.count , "ALREADY CONNECTED") )
		{
			err=WIIF_ERR_CIPSTART;
			printf("\r\n--连接淘宝服务器失败--\r\n");
			goto EXIT;
		}
	}
	printf("\r\n----连接淘宝服务器_OK----\r\n");
	
	//进入发送状态
	printf("\r\n--进入发送状态--\r\n");
	if(WIIF_ERR_NONE!=wifi_send_at("AT+CIPSEND\r\n",">",2000))
	{
		err=WIIF_ERR_CIPSEND;
		
		printf("\r\n--进入发送状态失败--\r\n");
		goto EXIT;
	}
	
	//获取时间
	printf("\r\n--获取时间--\r\n");
	if(WIIF_ERR_NONE != wifi_send_at("GET http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp HTTP/1.0\r\n\r\n","}}",5000))
	{
		err=WIIF_ERR_GETTIME;
		
		printf("\r\n--获取时间失败--\r\n");
		goto EXIT;
	}
	printf("\r\n----获取时间_OK----\r\n");
	//解析时间   global_h , global_m , global_s
	p = my_arraystr(u2recv.buf , u2recv.count , "Date:");
	if(p == NULL)
	{
		printf("time error\r\n");
		goto EXIT;
	}
	//"Date: Fri, 04 Mar 2022 07:03:49 GMT"
	p+=23;//使指针恰好指向时分秒
	sscanf(p,"%d:%d:%d GMT",&global_h,&global_m,&global_s);
	global_h += 8;
	printf("time = %d:%d:%d\r\n",global_h , global_m , global_s);
	
	//wifi_delayms(1000);
	
	//发送3个'+',使WIFI模块退出发送状态
	uart3_sendByte('+');
	uart3_sendByte('+');
	uart3_sendByte('+');
EXIT:	
	return err;
}

/*
函数名：UART3_Init
功能	：UART初始化函数  设置引脚为复用功能
参数	：无
返回值：无
备注	：PB10    USART_TX  复用推挽输出
		  PB11    USART_RX  浮空输入/带上拉输入
		  工作频率（84MHZ）

*/
void uart3_init(u32 baudrate)
{
	GPIO_InitTypeDef   	gpio_InitStruct ;
	USART_InitTypeDef 	usart_InitStruct;
	/*将PB10和 PB11复用成USART3的功能引脚*/
	//激活GPIOB
	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB , ENABLE );
	//GPIOx库函数初始化：复用模式
	gpio_InitStruct.GPIO_Mode 	=	GPIO_Mode_AF ;
	gpio_InitStruct.GPIO_OType 	= GPIO_OType_PP ;
	gpio_InitStruct.GPIO_Pin 		= GPIO_Pin_10 | GPIO_Pin_11 ;
	gpio_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL ;
	gpio_InitStruct.GPIO_Speed 	=	GPIO_Low_Speed ;
	GPIO_Init (GPIOB , &gpio_InitStruct);
	//选择PA9和PA10的复用功能
	GPIO_PinAFConfig (GPIOB, GPIO_PinSource10 , GPIO_AF_USART3);
	GPIO_PinAFConfig (GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/*激活USARTx*/
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE);
	
	/*过采样配置（选做）*/
	//USART_OverSampling8Cmd (USART1, ENABLE);
	
	/*USARTx库函数初始化*/
	usart_InitStruct.USART_BaudRate							=	baudrate ;
	usart_InitStruct.USART_HardwareFlowControl 	=	USART_HardwareFlowControl_None ;
	usart_InitStruct.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx ;
	usart_InitStruct.USART_Parity 							= USART_Parity_No ;
	usart_InitStruct.USART_StopBits 						= USART_StopBits_1 ;    //停止位1
	usart_InitStruct.USART_WordLength 					=	USART_WordLength_8b ;  //数据位8
	USART_Init (USART3, &usart_InitStruct);
	
	/*使能USART外设*/
	USART_Cmd (USART3, ENABLE);
}

/*********************************
函数名：		uart3_sendByte
函数参数：	u8 Data  1.发送的数据
返回值：		无
函数功能：	串口3发送一字节数据
备注：			
*********************************/
//通过uart3发送一帧数据，并等待发送完毕
void uart3_sendByte(char data)
{
	USART_SendData (USART3, data);
	while(!USART_GetFlagStatus (USART3, USART_FLAG_TXE));
}

/*********************************
函数名：		uart3_recvByte
函数参数：	无
返回值：		u8 1.接收的数据
函数功能：	串口3接收一字节数据
备注：			
*********************************/
//等待已准备好接收数据，通过uart1接收一帧数据，
char uart3_recvByte(void)
{
	while(!USART_GetFlagStatus (USART3, USART_FLAG_RXNE));
	return USART_ReceiveData (USART3);
}

/*********************************
函数名：		usart3_send_string
函数参数：	无
返回值：		无
函数功能：	串口3发送多字节数据
备注：			
*********************************/
void usart3_send_string(const char * str)
{
	while(*str != '\0')		//0
	{
		uart3_sendByte(*str++);
		//指针偏移1个字节，指向下一个字符
	}
  uart3_sendByte('\0');
}

/*********************************
函数名：		usart3_recv_string
函数参数：	无
返回值：		无
函数功能：	串口3接收多字节数据
备注：			
*********************************/
//从UART1上接收一个字符串
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
	while(data != '\n');//因为XCOM不能发送'\0'，只能发送到'\r' '\n'。
	*p = '\0';
#endif
}

/*********************************
函数名：		uart3_irq_config
函数参数：	
返回值：		无
函数功能：	串口3中断
备注：			
*********************************/
void uart3_irq_config(void)
{
	NVIC_InitTypeDef nvic_InitStruct;
	//允许uart1在“已准备接收数据”事件后，发送中断请求
	USART_ITConfig (USART3, USART_IT_RXNE, ENABLE);
	//NVIC在接收来自UART1的中断请求后，发生中断
	nvic_InitStruct.NVIC_IRQChannel									= USART3_IRQn;
	nvic_InitStruct.NVIC_IRQChannelCmd								= ENABLE;
	nvic_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占2位
	nvic_InitStruct.NVIC_IRQChannelSubPriority 				= 1;//比较2位
	NVIC_Init (&nvic_InitStruct);
}

/*
函数名：UART3_中断处理函数
功能	：
参数	：无
返回值：无
备注	：
		  
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
		//清除标记为
		u2recv.buf[u2recv.count++] = USART_ReceiveData (USART3);
	}
}
/*******************************************************************************************/