/**
  ******************************************************************************
  * @file    	Beep.c
  * @author  	eisbre
  * @version	None
  * @date    	2023/5/10
  * @brief   	蜂鸣器驱动，PB12，低电平触发

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"                  // Device header

void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//置高
}

void Beep_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void Beep_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void Beep_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 1)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
	else
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
}
