/**
  ******************************************************************************
  * @file    	esp8266.c
  * @author  
  * @version	None
  * @date    	2023/5/10
  * @brief   	ʹ�ô���1��esp8266ͨ�ţ���ʽΪATָ�����2Ϊ��������˿�

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

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"HONOR\",\"qqqqqqqq\"\r\n"//WIFI��Ϣ

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"www.eisbre.xyz\",1883\r\n"//���ӵ�EMQX�����������Ͷ˿�


unsigned char esp8266_buf[128];//����8266�Ļ���
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

extern const char *SubTopics[];

/**
* @brief  ��ս���8266�Ļ���
  * @param  None
  * @retval None
  */
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

/**
  * @brief  8266�ȴ��������
  * @param  None
  * @retval REV_OK-�������		REV_WAIT-���ճ�ʱδ���
  */
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

/**
* @brief  ��8266���������⵽��ȷ�ķ���ָ�˵��ָ��ͳɹ�
  * @param  cmd������	res����Ҫ���ķ���ָ��
  * @retval 0-�ɹ�	1-ʧ��
  */
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART1, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

}

/**
  * @brief  ��������
  * @param  data������	len������
  * @retval None
  */
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART1, data, len);		//�����豸������������
	}

}

/**
  * @brief  ��ȡƽ̨���ص����ݣ���ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
			��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
  * @param  �ȴ���ʱ��(����10ms)
  * @retval ƽ̨���ص�ԭʼ����
  */
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		Delay_ms(5);													//��ʱ�ȴ�
		timeOut--;
	} while(timeOut > 0);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

/**
  * @brief  ��ʼ��ESP8266
  * @param  None
  * @retval None
  */
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//ESP8266��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-��λ
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
* @brief  ����������������
  * @param  None
  * @retval None
  */
void MQTT_Ping(void)
{
	unsigned char *dataPtr;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL,0,0,0};
	if(MQTT_PacketPing(&mqttPacket)==0)//���������
	{
		ESP8266_SendData(mqttPacket._data,mqttPacket._len);
		UsartPrintf(USART_DEBUG,"���͵���������:%#X",mqttPacket._data);
		UsartPrintf(USART_DEBUG,"Ping data:%x\r\n",mqttPacket._data);
		MQTT_DeleteBuffer(&mqttPacket);//ɾ�����ͷ��ڴ�
		dataPtr = ESP8266_GetIPD(100);//�ȴ���Ӧ
		if(dataPtr!=NULL){
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_PINGRESP){
					//ȷ����������Ӧ
					UsartPrintf(USART_DEBUG,"���յ���������:%#X",MQTT_PKT_PINGRESP);
					UsartPrintf(USART_DEBUG,"Ping succeed\r\n" );
			}
		
		}else{
			UsartPrintf(USART_DEBUG,"Ping fail\r\n");//��Ӧʧ��/��������ƽ̨
			while (EMQX_DevLink()){
					Delay_ms(500) ;
					UsartPrintf(USART_DEBUG,"����ESP8266\r\n");
					ESP8266_Init();
			}
			
			UsartPrintf (USART_DEBUG,"�����ɹ�!\r\n") ;
			EMQX_Subscribe(SubTopics, 1);
		}		
	}
}


/**
  * @brief  ����1�շ��ж�
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}

}
