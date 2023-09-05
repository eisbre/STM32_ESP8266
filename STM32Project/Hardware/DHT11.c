/**
  ******************************************************************************
  * @file    	DHT11.c
  * @author  
  * @version	None
  * @date    	2023/5/10
  * @brief   	DHT11��ʪ�ȴ���������

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "DHT11.h"

/**
* @brief  ��������
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
  * @brief  ��������
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
  * @brief  DHT11��λ
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
  * @brief  �豸��⣬����DHT11�ɼ�����
  * @param  None
  * @retval �豸��������1�����򷵻�0
  */
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	DHT11_IO_IN(); 
    while (DHT11_DQ_IN&&retry<100)//DHT11������40~80us
	{
		retry++;
		Delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

/**
  * @brief  DHT11��ȡλ
  * @param  None
  * @retval None
  */
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(DHT11_DQ_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//�ȴ�40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

/**
  * @brief  ��ȡ1Byte��DHT11����
  * @param  None
  * @retval 1Byte����
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
  * @brief  ������ȡ����ȡ��ʪ��ֵ
  * @param  temp���¶�	humi��ʪ��
  * @retval ����0Ϊ�ɹ�
  */
uint8_t Read_DHT_Data(uint8_t *temp,uint8_t *humi)   //ʹ����������ݣ������0λ���������֣� ��1λʾС������
{        
		uint8_t buf[5];
		uint8_t i;
		DHT11_Rst();
		if(DHT11_Check()==0)
		{
				for(i=0;i<5;i++)//��ȡ40λ����
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
  * @brief  DHT11�豸��ʼ��
  * @param  None
  * @retval �豸��������1�����򷵻�0
  */
uint8_t DHT11_Init(void)
{	 		    
		DHT11_Rst();  
		return DHT11_Check();
} 

