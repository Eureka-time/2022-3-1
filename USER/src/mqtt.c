#include "main.h"

u8 txbuf[256]={0};
u8 rxbuf[256]={0};

//连接成功服务器回应 20 02 00 00
//客户端主动断开连接 e0 00
const u8 parket_connetAck[] = {0x20,0x02,0x00,0x00};//由于在接收字符时，当我接收到'\0'就转变成' '来存储，所接收到的回应则是 0x20 02 0x20 0x20
const u8 parket_disconnet[] = {0xe0,0x00};
const u8 parket_subAck[] = {0x90,0x03};
const u8 parket_heart[] = {0xc0,0x00};
const u8 parket_heart_reply[] = {0xd0,0x00};//由于在接收字符时，当我接收到'\0'就转变成' '来存储，所以接收到的回应则是 0xd0 0x20

_typdef_mqtt _mqtt = 
{
	0,0,
	0,0,
	Init,
	Connect,
	Disconnect,
	SubscribeTopic,
	PublishData,
	SentHeart,
	SendData,
};

u8 BYTE1(int num)
{
	return (u8)((num&0xFF00)>>8);
}
u8 BYTE0(int num)
{
	return (u8)(num&0xFF);
}

void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen)
{
	_mqtt.rxbuf = prx;_mqtt.rxlen = rxlen;
	_mqtt.txbuf = ptx;_mqtt.txlen = txlen;
// memset(_mqtt.rxbuf,0,_mqtt.rxlen);
// memset(_mqtt.txbuf,0,_mqtt.txlen);
//
// //无条件先主动断开,发送 4 次，保证能断开
// _mqtt.Disconnect();delay_ms(100);
// _mqtt.Disconnect();delay_ms(100);
// _mqtt.Disconnect();delay_ms(100);
// _mqtt.Disconnect();delay_ms(100);
}

uint8_t Connect(char *ClientID,char *Username,char *Password)
{
	int ClientIDLen = strlen(ClientID);
	int UsernameLen = strlen(Username);
	int PasswordLen = strlen(Password);
	int DataLen;
	_mqtt.txlen=0;
//Variable Header(可变报头)+Payload(有效负荷) 每个字段包含两个字节的长度标识
	DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
//固定报头
//控制报文类型
	_mqtt.txbuf[_mqtt.txlen++] = 0x10; //MQTT Message Type CONNECT  
//剩余长度(不包括固定头部)
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
	// if there are more data to encode, set the top bit of this byte
		if( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );
 
//可变报头
//协议名
	_mqtt.txbuf[_mqtt.txlen++] = 0x00; // Protocol Name Length MSB 
	_mqtt.txbuf[_mqtt.txlen++] = 0x04; // Protocol Name Length LSB 
	_mqtt.txbuf[_mqtt.txlen++] = 'M'; // ASCII Code for M 
	_mqtt.txbuf[_mqtt.txlen++] = 'Q'; // ASCII Code for Q 
	_mqtt.txbuf[_mqtt.txlen++] = 'T'; // ASCII Code for T 
	_mqtt.txbuf[_mqtt.txlen++] = 'T'; // ASCII Code for T 
//协议级别
	_mqtt.txbuf[_mqtt.txlen++] = 4; // MQTT Protocol version = 4 
//连接标志
	_mqtt.txbuf[_mqtt.txlen++] = 0xc2; // conn flags 
//心跳间隔时间
	_mqtt.txbuf[_mqtt.txlen++] = 0x00; // Keep-alive Time Length MSB 
	_mqtt.txbuf[_mqtt.txlen++] = 0x32; // Keep-alive Time Length LSB 500S 心跳包 300S  50s
	
	_mqtt.txbuf[_mqtt.txlen++] = BYTE1(ClientIDLen);// Client ID length MSB 
	_mqtt.txbuf[_mqtt.txlen++] = BYTE0(ClientIDLen);// Client ID length LSB 
	memcpy(&_mqtt.txbuf[_mqtt.txlen],ClientID,ClientIDLen);
	_mqtt.txlen += ClientIDLen;
 
	if(UsernameLen > 0)
	{ 
		_mqtt.txbuf[_mqtt.txlen++] = BYTE1(UsernameLen); //username length MSB 
		_mqtt.txbuf[_mqtt.txlen++] = BYTE0(UsernameLen); //username length LSB 
		memcpy(&_mqtt.txbuf[_mqtt.txlen],Username,UsernameLen);
		_mqtt.txlen += UsernameLen;
	}
 
	if(PasswordLen > 0)
	{ 
		_mqtt.txbuf[_mqtt.txlen++] = BYTE1(PasswordLen); //password length MSB 
		_mqtt.txbuf[_mqtt.txlen++] = BYTE0(PasswordLen); //password length LSB 
		memcpy(&_mqtt.txbuf[_mqtt.txlen],Password,PasswordLen);
		_mqtt.txlen += PasswordLen; 
	} 
	
//	printf("%s",_mqtt.txbuf);
	_mqtt.SendData(_mqtt.txbuf,_mqtt.txlen);
	u16 time=0;
	while(!wifi.flag)//等待传输完成标志
	{
		delay_ms(1);
		if(++time>500) //超时
		break;
	}
	if(wifi.flag)
	{
		wifi.flag=0;//清标志
//if(rxbuf[0]==parket_connetAck[0] && rxbuf[1]==parket_connetAck[1]) //连接成功
		if(wifi.rx_buf[0]==parket_connetAck[0] && wifi.rx_buf[1]==parket_connetAck[1]) //连接成功
		{
			return 1;//连接成功
		} 
	}
	return 0;
}

u8 SubscribeTopic(char *topic,u8 qos,u8 whether)
{ 
	_mqtt.txlen=0;
	int topiclen = strlen(topic);
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//可变报头的长度（2 字节）加上有效载荷的长度
//固定报头
//控制报文类型
	if(whether)
		_mqtt.txbuf[_mqtt.txlen++] = 0x82; //消息类型和标志订阅
	else 
		_mqtt.txbuf[_mqtt.txlen++] = 0xA2; //取消订阅
//剩余长度
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
// if there are more data to encode, set the top bit of this byte
		if( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );
//可变报头
	_mqtt.txbuf[_mqtt.txlen++] = 0; //消息标识符 MSB
	_mqtt.txbuf[_mqtt.txlen++] = 0x01; //消息标识符 LSB
//有效载荷
	_mqtt.txbuf[_mqtt.txlen++] = BYTE1(topiclen);//主题长度 MSB
	_mqtt.txbuf[_mqtt.txlen++] = BYTE0(topiclen);//主题长度 LSB 
	memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topiclen);
	_mqtt.txlen += topiclen;
 
	if(whether)
	{
	_mqtt.txbuf[_mqtt.txlen++] = qos;//QoS 级别
	}
	_mqtt.SendData(_mqtt.txbuf,_mqtt.txlen);
	u16 time=0;
	while(!wifi.flag)//等待传输完成标志
	{
		delay_ms(1);
		if(++time>500) //超时
			break;
	}
	
	if(wifi.flag)
	{
		wifi.flag=0;//清标志
//if(_mqtt.rxbuf[0]==parket_subAck[0] && _mqtt.rxbuf[1]==parket_subAck[1]) //订阅成功
		if(wifi.rx_buf[0]==parket_subAck[0] && wifi.rx_buf[1]==parket_subAck[1]) //订阅成功 
		{
			return 1;//订阅成功
		}
	}
	return 0;
}

/*****************************************************
函数名：		PublishData
函数参数：	1.topic 2.message 3.qos
返回值：		
函数功能：
备注：
******************************************************/
uint8_t PublishData(char *topic, char *message, uint8_t qos)
{ 
	int topicLength = strlen(topic); 
	int messageLength = strlen(message); 
	static u16 id=0;
	int DataLen;
	_mqtt.txlen=0;
//有效载荷的长度这样计算：用固定报头中的剩余长度字段的值减去可变报头的长度
//QOS 为 0 时没有标识符
//数据长度 主题名 报文标识符 有效载荷
 if(qos) 
	 DataLen = (2+topicLength) + 2 + messageLength; 
 else 
	 DataLen = (2+topicLength) + messageLength; 
	//固定报头
	//控制报文类型
	_mqtt.txbuf[_mqtt.txlen++] = 0x30; // MQTT Message Type PUBLISH 
//剩余长度
	do
	{
	u8 encodedByte = DataLen % 128;
	DataLen = DataLen / 128;
// if there are more data to encode, set the top bit of this byte
	if ( DataLen > 0 )
		encodedByte = encodedByte | 128;
	_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );
	_mqtt.txbuf[_mqtt.txlen++] = BYTE1(topicLength);//主题长度 MSB
	_mqtt.txbuf[_mqtt.txlen++] = BYTE0(topicLength);//主题长度 LSB 
	memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topicLength);//拷贝主题
	_mqtt.txlen += topicLength;
 
//报文标识符
	if(qos)
	{
		_mqtt.txbuf[_mqtt.txlen++] = BYTE1(id);
		_mqtt.txbuf[_mqtt.txlen++] = BYTE0(id);
		id++;
	}
	memcpy(&_mqtt.txbuf[_mqtt.txlen],message,messageLength);
	_mqtt.txlen += messageLength;
 
	_mqtt.SendData(_mqtt.txbuf,_mqtt.txlen);
 return _mqtt.txlen;
}

/********************************
函数名：		SentHeart
函数参数：	无
返回值：		无
函数功能：	发送心跳包
备注：			5s一次
*********************************/
void SentHeart(void)
{
	_mqtt.SendData((u8 *)parket_heart,sizeof(parket_heart));
}

/********************************
函数名：		Disconnect
函数参数：	无
返回值：		无
函数功能：	断开服务器连接
备注：			无
*********************************/
void Disconnect(void)
{
	_mqtt.SendData((u8 *)parket_disconnet,sizeof(parket_disconnet));
}

/********************************
函数名：		SendData
函数参数：	1.数据包 2.数据长度
返回值：		无
函数功能：	断开服务器连接
备注：			无
*********************************/
void SendData(u8* p,u16 len)
{
	while(len--) 
	{
		while((USART3->SR & (0X01<<7)) == 0); //等待发送缓冲区为空
		USART3->DR = *p;//USART1->DR=*p;
		p++;
	}
}
