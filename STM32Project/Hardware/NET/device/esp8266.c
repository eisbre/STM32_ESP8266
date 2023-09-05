/**
  ******************************************************************************
  * @file    	esp8266.c
  * @author  
  * @version	None
  * @date    	2023/5/10
  * @brief   	使用串口1与esp8266通信，方式为AT指令，串口2为调试输出端口

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"

#include "esp8266.h"
#include "MqttKit.h"
#include "emqx.h"

#include "Delay.h"
#include "USART.h"

#include <string.h>
#include <stdio.h>

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"HONOR\",\"qqqqqqqq\"\r\n"//WIFI信息

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"www.eisbre.xyz\",1883\r\n"//连接的EMQX服务器域名和端口


unsigned char esp8266_buf[128];//接收8266的缓存
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

extern const char *SubTopics[];

/**
* @brief  清空接收8266的缓存
  * @param  None
  * @retval None
  */
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

/**
  * @brief  8266等待接收完成
  * @param  None
  * @retval REV_OK-接收完成		REV_WAIT-接收超时未完成
  */
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

/**
* @brief  向8266发送命令，检测到正确的返回指令，说明指令发送成功
  * @param  cmd：命令	res：需要检查的返回指令
  * @retval 0-成功	1-失败
  */
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART1, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

}

/**
  * @brief  发送数据
  * @param  data：数据	len：长度
  * @retval None
  */
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART1, data, len);		//发送设备连接请求数据
	}

}

/**
  * @brief  获取平台返回的数据，不同网络设备返回的格式不同，需要去调试
			如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
  * @param  等待的时间(乘以10ms)
  * @retval 平台返回的原始数据
  */
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		Delay_ms(5);													//延时等待
		timeOut--;
	} while(timeOut > 0);
	
	return NULL;														//超时还未找到，返回空指针

}

/**
  * @brief  初始化ESP8266
  * @param  None
  * @retval None
  */
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266复位引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	Delay_ms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	Delay_ms(500);
	
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "0. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "1. RST\r\n");
	while(ESP8266_SendCmd("AT+RST\r\n", ""))
		Delay_ms(500);
	while(ESP8266_SendCmd("AT+CIPCLOSE\r\n", ""))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");

}

/**
* @brief  发送心跳包，重连
  * @param  None
  * @retval None
  */
void MQTT_Ping(void)
{
	unsigned char *dataPtr;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL,0,0,0};
	if(MQTT_PacketPing(&mqttPacket)==0)//心跳包组包
	{
		ESP8266_SendData(mqttPacket._data,mqttPacket._len);
		UsartPrintf(USART_DEBUG,"发送的心跳数据:%#X",mqttPacket._data);
		UsartPrintf(USART_DEBUG,"Ping data:%x\r\n",mqttPacket._data);
		MQTT_DeleteBuffer(&mqttPacket);//删除包释放内存
		dataPtr = ESP8266_GetIPD(100);//等待响应
		if(dataPtr!=NULL){
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_PINGRESP){
					//确定是心跳响应
					UsartPrintf(USART_DEBUG,"接收的心跳数据:%#X",MQTT_PKT_PINGRESP);
					UsartPrintf(USART_DEBUG,"Ping succeed\r\n" );
			}
		
		}else{
			UsartPrintf(USART_DEBUG,"Ping fail\r\n");//响应失败/重新连接平台
			while (EMQX_DevLink()){
					Delay_ms(500) ;
					UsartPrintf(USART_DEBUG,"加载ESP8266\r\n");
					ESP8266_Init();
			}
			
			UsartPrintf (USART_DEBUG,"重连成功!\r\n") ;
			EMQX_Subscribe(SubTopics, 1);
		}		
	}
}


/**
  * @brief  串口1收发中断
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}

}
