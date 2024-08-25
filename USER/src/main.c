#include "main.h"
int main(void)
{
 	u8 adcx;
	u8 pd,i;
	float tem,rh;
	char publish[1024];
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	usart1_init(115200);	//初始化串口波特率为115200
	usart3_init(115200);
	LED_Init();					//初始化LED 
 	LCD_Init();					//初始化LCD
	Lsens_Init(); 			//初始化光敏传感器 
	relay_init();
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(0,0,200,16,16,(u8 *)"WIFI CONNECT...");
	if(wifi_connect_init()==0)
		LCD_ShowString(0,0,200,16,16,(u8 *)"WIFI CONNECT SUCCESS");
	else
		LCD_ShowString(0,0,200,16,16,(u8 *)"WIFI CONNECT FAIL");
	
	LCD_ShowString(30,130,200,16,16,(u8 *)"LSENS_VAL:");
	LCD_ShowString(30,150,200,16,16,(u8 *)"TEMP_VAL :");
	LCD_ShowString(30,170,200,16,16,(u8 *)"HUMID_VAL:");
	while(1)
	{
		pd = wifi_get_cloud_data();
		if(pd == 0)
		{
			if(led_flag)
			{
				LED_ON;
				LCD_ShowString(30,210,200,16,16,(u8 *)"LED STATE: ON ");
			}
			else
			{
				LED_OFF;
				LCD_ShowString(30,210,200,16,16,(u8 *)"LED STATE: OFF ");
			}
			if(air_flag)
			{
				AIR_ON;
				LCD_ShowString(30,230,200,16,16,(u8 *)"AIR STATE: ON ");
			}
			else
			{
				AIR_OFF;
				LCD_ShowString(30,230,200,16,16,(u8 *)"AIR STATE: OFF ");
			}
			if(hum_flag)
			{
				DEHUM_ON;
				LCD_ShowString(30,250,200,16,16,(u8 *)"DEHUM STATE: ON ");
			}
			else
			{
				DEHUM_OFF;
				LCD_ShowString(30,250,200,16,16,(u8 *)"DEHUM STATE: OFF ");
			}
		}
		adcx=Lsens_Get_Val();
		get_sht20_data(&tem,&rh);
		LCD_ShowxNum(30+10*8,130,adcx,3,16,0);//显示光照值 
		LCD_ShowxNum(30+10*8,150,tem,3,16,0);//显示温度值 
		LCD_ShowxNum(30+10*8,170,rh,3,16,0);//显示湿度的值 
		
		if(i%2==0)
		{
		 printf("adcx:%d   %f   %f\r\n",adcx,tem,rh);
//		printf("adcx:%f",tem);
//		printf("adcx:%f/r/n",rh);
		
		}
		if(adcx < 60)//根据光照值控制灯光开关
		{
			if(cnt_flag == 0)
			{
				led_flag = 1;
				LED_ON;
				LCD_ShowString(30,210,200,16,16,(u8 *)"LED STATE: ON ");
			}
		}
		else
		{
			if(cnt_flag == 0)
			{
				led_flag = 0;
				LED_OFF;
				LCD_ShowString(30,210,200,16,16,(u8 *)"LED STATE: OFF ");
			}
		}
		
		if(air_flag == 0 && cnt_flag == 0)
		{
			if(tem >= 28.0 || tem <= 18.0)
			{
				air_flag = 1;
				AIR_ON;
				LCD_ShowString(30,230,200,16,16,(u8 *)"AIR STATE: ON ");
			}
			else
				LCD_ShowString(30,230,200,16,16,(u8 *)"AIR STATE: OFF ");
		}
		else if(air_flag == 1 && cnt_flag == 0)
		{
			if(tem <= 26.0 && tem >= 22.0)
			{
				air_flag = 0;
				AIR_OFF;
				LCD_ShowString(30,230,200,16,16,(u8 *)"AIR STATE: OFF ");
			}
			else
				LCD_ShowString(30,230,200,16,16,(u8 *)"AIR STATE: ON ");
		}
		if(hum_flag == 0 && cnt_flag == 0)
		{
			if(rh >= 67.0)
			{
				hum_flag = 1;
				DEHUM_ON;
				LCD_ShowString(30,250,200,16,16,(u8 *)"DEHUM STATE: ON ");
			}
			else
				LCD_ShowString(30,250,200,16,16,(u8 *)"DEHUM STATE: OFF ");
		}
		else if(hum_flag == 1 && cnt_flag == 0)
		{
			if(rh <= 60.0)
			{
				hum_flag = 0;
				DEHUM_OFF;
				LCD_ShowString(30,250,200,16,16,(u8 *)"DEHUM STATE: OFF ");
			}
			else
				LCD_ShowString(30,250,200,16,16,(u8 *)"DEHUM STATE: ON ");
		}
		sprintf(publish,"{\"id\":1649831305896,\"params\":{\"temperature\":%.3f},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",tem);
		_mqtt.PublishData((char *)"/sys/h2fdIGSYIja/system/thing/event/property/post",publish,0);
		
		sprintf(publish,"{\"id\":1649831305896,\"params\":{\"Humidity\":%.3f},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",rh);
		_mqtt.PublishData((char *)"/sys/h2fdIGSYIja/system/thing/event/property/post",publish,0);
		
		sprintf(publish,"{\"id\":1649831305896,\"params\":{\"LightLux\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",adcx);
		_mqtt.PublishData((char *)"/sys/h2fdIGSYIja/system/thing/event/property/post",publish,0);
		
		sprintf(publish,"{\"id\":1649831305896,\"params\":{\"RunningState\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",air_flag);
		_mqtt.PublishData((char *)"/sys/h2fdIGSYIja/system/thing/event/property/post",publish,0);
		
		sprintf(publish,"{\"id\":1649831305896,\"params\":{\"LedState\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",led_flag);
		_mqtt.PublishData((char *)"/sys/h2fdIGSYIja/system/thing/event/property/post",publish,0);
	
		sprintf(publish,"{\"id\":1649831305896,\"params\":{\"Dehum_State\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",hum_flag);
		_mqtt.PublishData((char *)"/sys/h2fdIGSYIja/system/thing/event/property/post",publish,0);
		
	}
}