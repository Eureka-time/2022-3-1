#ifndef _MQTT_H
#define _MQTT_H

#include "stm32f4xx.h"

//#define delay_ms(x)	tim3_delay_ms(x)

typedef struct
{
	u8 *rxbuf;u16 rxlen;
	u8 *txbuf;u16 txlen;
	void (*Init)(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
	u8 (*Connect)(char *ClientID,char *Username,char *Password);
	void (*Disconnect)(void);
	u8 (*SubscribeTopic)(char *topic,u8 qos,u8 whether);
	u8 (*PublishData)(char *topic, char *message, u8 qos);
	void (*SendHeart)(void);
	void (*SendData)(u8* p,u16 len);
}_typdef_mqtt;

extern u8 txbuf[256];
extern u8 rxbuf[256];
extern _typdef_mqtt _mqtt;

u8 BYTE0(int num);
u8 BYTE1(int num);
void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
u8 Connect(char *ClientID,char *Username,char *Password);
void Disconnect(void);
u8 SubscribeTopic(char *topic,u8 qos,u8 whether);
u8 PublishData(char *topic, char *message, u8 qos);
void SentHeart(void);
void SendData(u8* p,u16 len);


#ifdef __cplusplus
}
#endif

#endif