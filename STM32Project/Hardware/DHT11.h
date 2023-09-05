#ifndef __DHT11_H
#define	__DHT11_H
#include "sys.h" 

#define	DHT11_DQ_OUT PAout(8) //数据端口	PA15 
#define	DHT11_DQ_IN  PAin(8)  //数据端口	PA15

#define DHT11_GPIO_PORT    	GPIOA			              //GPIO端口
#define DHT11_GPIO_CLK 	    RCC_APB2Periph_GPIOA		//GPIO端口时钟
#define DHT11_GPIO_PIN		GPIO_Pin_8			        //连接到SCL时钟线的GPIO

uint8_t DHT11_Init(void);//初始化DHT11
uint8_t Read_DHT_Data(u8 *temp,u8 *humi);//读取温湿度
uint8_t DHT11_Read_Byte(void);//读出一个字节
uint8_t DHT11_Read_Bit(void);//读出一个位
uint8_t DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11 

#endif
