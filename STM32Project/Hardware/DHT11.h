#ifndef __DHT11_H
#define	__DHT11_H
#include "sys.h" 

#define	DHT11_DQ_OUT PAout(8) //���ݶ˿�	PA15 
#define	DHT11_DQ_IN  PAin(8)  //���ݶ˿�	PA15

#define DHT11_GPIO_PORT    	GPIOA			              //GPIO�˿�
#define DHT11_GPIO_CLK 	    RCC_APB2Periph_GPIOA		//GPIO�˿�ʱ��
#define DHT11_GPIO_PIN		GPIO_Pin_8			        //���ӵ�SCLʱ���ߵ�GPIO

uint8_t DHT11_Init(void);//��ʼ��DHT11
uint8_t Read_DHT_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);//����һ���ֽ�
uint8_t DHT11_Read_Bit(void);//����һ��λ
uint8_t DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11 

#endif
