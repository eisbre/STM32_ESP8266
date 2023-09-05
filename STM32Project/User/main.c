/**
  ******************************************************************************
  * @file    	main.c
  * @author  	eisbre
  * @version	None
  * @date    	2023/5/10
  * @brief   	主函数，硬件初始化，连接EMQX服务器，数据获取，屏幕刷新

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "Delay.h"

#include "OLED.h"
#include "Beep.h"
#include "DHT11.h"
#include "BH1750.h"
#include "USART.h"
#include "Timer.h"
#include "LED.h"

#include "esp8266.h"
#include "emqx.h"

#include <string.h>
#include <stdio.h>

uint8_t Temp[2]={0};
uint8_t Humi[2]={0};
float light;
uint8_t LED;
const char *SubTopics[] = {"my/smarthome/sub"};
const char PubTopics[] = "my/smarthome/pub";
char PUB_BUF[156];//上传数据
int heart_pack = 0;//心跳时间

int main(void){
	OLED_Init();
	Beep_Init();
	LED_Init();
	DHT11_Init();
	BH1750_Init();
	Usart1_Init(115200);//USART1 esp8266
	Usart2_Init(115200);//DEBUG USART2
	TIM2_Int_Init(2499, 7199);//屏幕定时刷新
	ESP8266_Init();
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
	while(EMQX_DevLink())			//接入EMQX
		Delay_ms(500);
	
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	
	EMQX_Subscribe(SubTopics, 1);
	
	while(1){
		heart_pack ++;
		if(heart_pack == 1000)//心跳检测
		{
			heart_pack = 0;
			MQTT_Ping();
			ESP8266_Clear();
		}
		Read_DHT_Data(Temp, Humi);//获取温湿度
		if(timeCount % 40==0)
		{
			if (!i2c_CheckDevice(BH1750_Addr))//获取光照度
			{
				light = Light_Intensity();
			}
		}
		if(++timeCount >= 200)//发送间隔5s
		{
			UsartPrintf(USART_DEBUG, "Publish\r\n");
			sprintf(PUB_BUF, "{\"Temp\":%d.%d,\"Hum\":%d.%d,\"Light\":%.2f,\"LED\":%d}",Temp[0],Temp[1],Humi[0],Humi[1],light,LED);
			EMQX_Publish(PubTopics, PUB_BUF);

			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			EMQX_RevPro(dataPtr);
		
		Delay_ms(10);
	}
}

void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
			//刷新屏幕
			OLED_ShowString(1, 1, "Temp: ");
			OLED_ShowNum(1, 7, Temp[0], 2);
			OLED_ShowChar(1, 9, '.');
			OLED_ShowNum(1, 10, Temp[1], 2);
			OLED_ShowString(2, 1, "Humi: ");
			OLED_ShowNum(2, 7, Humi[0], 2);
			OLED_ShowChar(2, 9, '.');
			OLED_ShowNum(2, 10, Humi[1], 2);
			OLED_ShowChar(2, 12, '%');
			OLED_ShowString(3, 1, "Light: ");
			OLED_ShowNum(3, 8, light, 5);
			OLED_ShowString(3, 13, "Lx");
		}
}
