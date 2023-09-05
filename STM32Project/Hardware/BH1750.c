/**
  ******************************************************************************
  * @file    	BH1750.c
  * @author  	
  * @version	None
  * @date    	2023/5/10
  * @brief   	BH1750光照度传感器驱动，使用I2C总线

  ******************************************************************************
  * @attention
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
  ******************************************************************************
  */
  
#include "BH1750.h"
#include "sys.h"

static void I2C_BH1750_GPIOConfig(void);

/**
  * @brief  I2C总线位延迟，最快400KHz，使用循环实现
  * @param  None
  * @retval None
  */
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 10; i++);
}

/**
  * @brief  I2C总线起始信号
  * @param  None
  * @retval None
  */
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	BH1750_I2C_SDA_1();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_0();
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
}

/**
  * @brief  I2C总线停止信号
  * @param  None
  * @retval None
  */
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	BH1750_I2C_SDA_0();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_1();
}

/**
  * @brief  I2C总线设备发送1Byte数据
  * @param  _ucByte ： 等待发送的字节
  * @retval None
  */
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			BH1750_I2C_SDA_1();
		}
		else
		{
			BH1750_I2C_SDA_0();
		}
		i2c_Delay();
		BH1750_I2C_SCL_1();
		i2c_Delay();	
		BH1750_I2C_SCL_0();
		if (i == 7)
		{
			 BH1750_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

/**
  * @brief  主机从I2C总线设备读1Byte数据
  * @param  None
  * @retval 读到的数据
  */
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		BH1750_I2C_SCL_1();
		i2c_Delay();
		if (BH1750_I2C_SDA_READ())
		{
			value++;
		}
		BH1750_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/**
  * @brief  读取器件的ACK应答信号
  * @param  None
  * @retval 返回0表示正确应答，1表示无器件响应
  */
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	BH1750_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (BH1750_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
		re = 1;
	else
		re = 0;
	BH1750_I2C_SCL_0();
	i2c_Delay();
	return re;
}

/**
* @brief  主机发送ACK应答信号
  * @param  None
  * @retval None
  */
void i2c_Ack(void)
{
	BH1750_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
	BH1750_I2C_SDA_1();	/* CPU释放SDA总线 */
}

/**
* @brief  主机发送非应答信号
  * @param  None
  * @retval None
  */
void i2c_NAck(void)
{
	BH1750_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();	
}

/**
  * @brief  配置I2C总线的GPIO，采用模拟IO的方式实现
  * @param  None
  * @retval None
  */
static void I2C_BH1750_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BH1750_RCC_I2C_PORT, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = BH1750_I2C_SCL_PIN | BH1750_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(BH1750_GPIO_PORT_I2C, &GPIO_InitStructure);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop();
}

/**
  * @brief  检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
  * @param  _Address：设备的I2C总线地址
  * @retval 返回值 0 表示正确， 返回1表示未探测到
  */
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	i2c_Start();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | BH1750_I2C_WR);
	ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

	i2c_Stop();			/* 发送停止信号 */

	return ucAck;
}

/**
  * @brief  BH1750写一个字节
  * @param  None
  * @retval 成功：0		失败：非0
  */
uint8_t BH1750_Byte_Write(uint8_t data)
{
	i2c_Start();
	//发送写地址
	i2c_SendByte(BH1750_Addr|0);
	if(i2c_WaitAck()==1)
		return 1;
	//发送控制命令
	i2c_SendByte(data);
	if(i2c_WaitAck()==1)
		return 2;
	i2c_Stop();
	return 0;
}

/**
  * @brief  BH1750读取测量数据
  * @param  None
  * @retval 成功：返回光照强度 	失败：返回0
  */
uint16_t BH1750_Read_Measure(void)
{
	uint16_t receive_data=0; 
	i2c_Start();
	//发送读地址
	i2c_SendByte(BH1750_Addr|1);
	if(i2c_WaitAck()==1)
		return 0;
	//读取高八位
	receive_data=i2c_ReadByte();
	i2c_Ack();
	//读取低八位
	receive_data=(receive_data<<8)+i2c_ReadByte();
	i2c_NAck();
	i2c_Stop();
	return receive_data;	//返回读取到的数据
}


//BH1750s上电
void BH1750_Power_ON(void)
{
	BH1750_Byte_Write(POWER_ON);
}

//BH1750s断电
void BH1750_Power_OFF(void)
{
	BH1750_Byte_Write(POWER_OFF);
}

//BH1750复位	仅在上电时有效
void BH1750_RESET(void)
{
	BH1750_Byte_Write(MODULE_RESET);
}

//BH1750初始化
void BH1750_Init(void)
{
	I2C_BH1750_GPIOConfig();		/* 配置GPIO */
	
	BH1750_Power_ON();	//BH1750s上电
	//BH1750_RESET();			//BH1750复位
	BH1750_Byte_Write(Measure_Mode);
	//SysTick_Delay_ms(120);
}

//获取光照强度
float Light_Intensity(void)
{
	return (float)(BH1750_Read_Measure()/1.1f*Resolurtion);
}
