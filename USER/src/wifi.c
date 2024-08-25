#include "main.h"

WIFI wifi = {'\0',0};

const char wifi_name[] = "ddddd";
const char wifi_pswd[] = "drj429429";
char clientid[] = "h2fdIGSYIja.system|securemode=2,signmethod=hmacsha256,timestamp=1649831305896|";
char username[] = "system&h2fdIGSYIja";
char password[] = "28bcb3e63d876b25abd6d459dd6c122c2311bd507f8832eb0d9a8c07633555d6";
char mqtt_host[] = "iot-06z00byzdo6mr6k.mqtt.iothub.aliyuncs.com";

u8 wifi_send_cmd(char *str,const char *fmt,...)
{
	u16 timeout = 0;
	char *p;
	u8 buf[1024];
	usart3_send_string((u8 *)fmt);
	while(!wifi.flag)
	{
		if(timeout++ >= 1000)
			return 1;
		tim3_delay_ms(1);
	}
	wifi.flag = 0;
	memcpy(buf,wifi.rx_buf,strlen((char *)wifi.rx_buf));
	memset(wifi.rx_buf,0,200);
	p = strstr((const char *)buf,(const char *)str);
	if(p == NULL)
		return 2;
	else
		return 0;
}

u8 wifi_connect_ap(const char *fmt,...)
{
	int i;
	u16 timeout = 0;
	char *p;
	char *q;
	u8 buf[1024];
	usart3_send_string((u8 *)fmt);
	for(i = 0;i < 20;i++)
	{
		while(!wifi.flag)
		{
			if(timeout++ >= 1000)
				break;
			delay_ms(1);
		}
		timeout = 0;
		wifi.flag = 0;
		memcpy(buf,wifi.rx_buf,strlen((char *)wifi.rx_buf));
		memset(wifi.rx_buf,0,200);
		p = strstr((const char *)buf,"OK");
		if(p == NULL)
		{
			q = strstr((const char *)buf,"FAIL");
			if(q != NULL)
				return 2;
			else
			{
				if(i >= 19)
					return 3;
			}
		}
		else
			return 0;
	}
}

char web_data[200];

u8 get_web_data(char *fmt,...)
{
	u16 timeout = 0;
	usart3_send_string((u8 *)fmt);
	while(!wifi.flag)
	{
		if(timeout++ >= 1000)
				return 1;
		tim3_delay_ms(1);
	}
	wifi.flag = 0;
	tim3_delay_ms(100);
	memcpy(web_data,wifi.rx_buf,strlen((char *)wifi.rx_buf));
	memset(wifi.rx_buf,0,200);
	if(strlen((char *)web_data))
	{
		usart3_send_string((u8 *)"+++");
		return 0;
	}
	else
	{
		usart3_send_string((u8 *)"+++");
		return 2;
	}
}

/***************************************************
函数名：	wifi_connect_init
函数参数：	无
返回值：	0成功 1-5失败
函数功能：	WiFi连接热点并连接阿里云
备注：		无
***************************************************/
u8 wifi_connect_init(void)
{
	u8 pd;
	wifi_send_cmd("OK","+++");
	wifi_send_cmd("OK","AT+RESET\r\n");
	//测试
	pd = wifi_send_cmd("OK","AT\r\n");
	if(pd)
	{
		printf("wifi send AT error:%d\r\n",pd);
		printf("%s\r\n",wifi.rx_buf);
	}
	else
	{
		printf("wifi send AT sucess\r\n");
	}
	//设置sta
	pd = wifi_send_cmd("OK","AT+CWMODE=1\r\n");
	if(pd)
	{
		printf("wifi send CWMODE error:%d\r\n",pd);
		printf("%s\r\n",wifi.rx_buf);
	}
	else
	{
		printf("wifi send CWMODE sucess\r\n");
	}
	//连接热点
	pd = wifi_connect_ap("AT+CWJAP=\"QWERTASDF\",\"12345678\"\r\n");
	if(pd)
	{
		printf("wifi send CONNECT error:%d\r\n",pd);
		printf("%s\r\n",wifi.rx_buf);
		return 1;
	}
	else
	{
		printf("wifi send CONNECT sucess\r\n");
	}
	//透传
	pd = wifi_send_cmd("OK","AT+CIPMODE=1\r\n");
	if(pd)
	{
		printf("wifi send CIPMODE error:%d\r\n",pd);
		printf("%s\r\n",wifi.rx_buf);
		return 2;
	}
	else
	{
		printf("wifi send CIPMODE sucess\r\n");
	}
	pd = wifi_connect_ap("AT+CIPSTART=\"TCP\",\"iot-06z00byzdo6mr6k.mqtt.iothub.aliyuncs.com\",1883\r\n");
	if(pd)
	{
		printf("wifi CONNECT TCP error:%d\r\n",pd);
		printf("%s\r\n",wifi.rx_buf);
		return 3;
	}
	else
	{
		printf("wifi CONNECT TCP sucess\r\n");
	}
	//启动发送
	pd = wifi_send_cmd(">","AT+CIPSEND\r\n");
	if(pd)
	{
		printf("wifi CIPSEND error:%d\r\n",pd);
		printf("%s\r\n",wifi.rx_buf);
		return 4;
	}
	else
	{
		printf("wifi CIPSEND sucess\r\n");
	}
	//初始化收发字符串
	_mqtt.Init(rxbuf,0,txbuf,0);
	//连接阿里云云平台
	if(_mqtt.Connect(clientid,username,password))
	{
		printf("MQTT CONNECT SUCCESS\r\n");
	}
	else
	{
		printf("MQTT CONNECT FAIL\r\n");
		return 5;
	}
	return 0;
}

void wifi_publish_ele_info(void)
{
//	char publish[200];
//	sprintf(publish,"{\"id\":1649599085666,\"params\":{\"CurrentVoltage\":%.3f},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",);
//	_mqtt.PublishData((char *)"/sys/gzydN5D79JE/smart_socket/thing/event/property/post",publish,0);
//	sprintf(publish,"{\"id\":1649599085666,\"params\":{\"Current\":%.3f},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",);
//	_mqtt.PublishData((char *)"/sys/gzydN5D79JE/smart_socket/thing/event/property/post",publish,0);
//	sprintf(publish,"{\"id\":1649599085666,\"params\":{\"RealTimePower\":%.3f},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",);
//	_mqtt.PublishData((char *)"/sys/gzydN5D79JE/smart_socket/thing/event/property/post",publish,0);
}

u8 air_flag = 0;
u8 hum_flag = 0;
u8 led_flag = 0;
u8 cnt_flag = 0;
u8 cloud[512];
u8 wifi_get_cloud_data(void)
{
	u16 timeout = 0;
	char *p;
	char *q;
	char *r;
	char *h;
	char *k;
	while(!wifi.flag)
	{
		if(timeout++ >= 100)		//预留100ms判断是否有云端下发数据
		{
			return 1;
		}
		delay_ms(1);
	}
	wifi.flag = 0;					//清除WiFi接收标志
	memcpy(cloud,wifi.rx_buf,200);	//拷贝接收的数据
	memset(wifi.rx_buf,0,200);		//清空WiFi接收缓冲区
	p = strstr((const char *)cloud,"RunningState");//判断下发的指令是否为所需指令
	if(p == NULL)
	{
		r = strstr((const char *)cloud,"LedState");
		if(r == NULL)
		{
			h = strstr((const char *)cloud,"Dehum_State");
			if(h == NULL)
			{
				k = strstr((const char *)cloud,"StatusSwitching");
				if(k == NULL)
					return 2;
				else
				{
					q = strstr(k,":");
					if(q == NULL)
					{
						return 3;
					}
					else
					{
						if(*(q + 1) == '1')		//判断指令作用
						{
							cnt_flag = 0;
						}
						else if(*(q + 1) == '0')
						{
							cnt_flag = 1;
						}
						return 0;
					}
				}
			}
			else
			{
				q = strstr(h,":");
				if(q == NULL)
				{
					return 3;
				}
				else
				{
					if(*(q + 1) == '1')		//判断指令作用
					{
						hum_flag = 1;
					}
					else if(*(q + 1) == '0')
					{
						hum_flag = 0;
					}
					return 0;
				}
			}
		}
		else
		{
			q = strstr(r,":");
			if(q == NULL)
			{
				return 3;
			}
			else
			{
				if(*(q + 1) == '1')		//判断指令作用
				{
					led_flag = 1;
				}
				else if(*(q + 1) == '0')
				{
					led_flag = 0;
				}
				return 0;
			}
		}
	}
	else
	{
		q = strstr(p,":");			//获取指令位置
		if(q == NULL)
		{
			return 3;
		}
		else
		{
			if(*(q + 1) == '1')		//判断指令作用
			{
				air_flag = 1;
			}
			else if(*(q + 1) == '0')
			{
				air_flag = 0;
			}
			return 0;
		}
	}
}