#ifndef _WIFI_H
#define _WIFI_H

#include "stm32f4xx.h"

#define AL_Cloud_PORT 1883

typedef struct
{
	u8 rx_buf[200];
	u8 flag;
}WIFI;

extern WIFI wifi;
extern char web_data[200];
extern const char wifi_name[];
extern const char wifi_pswd[];
extern char password[];
extern char username[];
extern char clientid[];
extern u8 air_flag;
extern u8 hum_flag;
extern u8 led_flag;
extern u8 cnt_flag;

u8 wifi_send_cmd(char *str,const char *fmt,...);
u8 wifi_connect_ap(const char *fmt,...);
u8 get_web_data(char *fmt,...);
u8 wifi_connect_init(void);
void wifi_publish_ele_info(void);
u8 wifi_get_cloud_data(void);

#endif