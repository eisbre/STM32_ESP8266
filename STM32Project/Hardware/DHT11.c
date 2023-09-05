/**
  ******************************************************************************
  * @file    	DHT11.c
  * @author  
  * @version	None
  * @date    	2023/5/10
  * @brief   	DHT11温湿度传感器驱动

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "DHT11.h"

/**
* @brief  主机发送
  * @param  None
  * @retval None
  */
void DHT11_IO_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE);	 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	
	
 	GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;				 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);				 
}

/**
  * @brief  主机接收
  * @param  None
  * @retval None
  */
void DHT11_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE);	 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);     
	
 	GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;				 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
 	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);				 
}

/**
  * @brief  DHT11复位
  * @param  None
  * @retval None
  */
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	
	DHT11_DQ_OUT=0; 	
	Delay_ms(20);    	
	DHT11_DQ_OUT=1; 	
	Delay_us(30);     	
}

/**
  * @brief  设备检测，触发DHT11采集数据
  * @param  None
  * @retval 设备正常返回1，否则返回0
  */
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	DHT11_IO_IN(); 
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		Delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

/**
  * @brief  DHT11读取位
  * @param  None
  * @retval None
  */
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(DHT11_DQ_IN&&retry<100)//等待变为低电平
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//等待变高电平
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//等待40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

/**
  * @brief  读取1Byte的DHT11数据
  * @param  None
  * @retval 1Byte数据
  */
uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
  }						    
  return dat;
}

/**
  * @brief  连续读取，获取温湿度值
  * @param  temp：温度	humi：湿度
  * @retval 返回0为成功
  */
uint8_t Read_DHT_Data(uint8_t *temp,uint8_t *humi)   //使用数组接数据，数组第0位是整数部分， 第1位示小数部分
{        
		uint8_t buf[5];
		uint8_t i;
		DHT11_Rst();
		if(DHT11_Check()==0)
		{
				for(i=0;i<5;i++)//读取40位数据
				{
						buf[i]=DHT11_Read_Byte();
				}
				if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
				{
						*humi=buf[0];
						*(humi + 1)=buf[1];
						*temp=buf[2];
						*(temp + 1)=buf[3];
				}
		}
		else
		{
				return 1;
		}			
		return 0;	    
}

/**
  * @brief  DHT11设备初始化
  * @param  None
  * @retval 设备正常返回1，否则返回0
  */
uint8_t DHT11_Init(void)
{	 		    
		DHT11_Rst();  
		return DHT11_Check();
} 

