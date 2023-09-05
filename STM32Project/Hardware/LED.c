/**
  ******************************************************************************
  * @file    	LED.c
  * @author  	eisbre
  * @version	None
  * @date    	2023/5/10
  * @brief   	控制板载LED，PC4为LED1，PC5为LED2，低电平点亮

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"                  // Device header

void LED_Init(void){//初始化LED，使用的是PC4，PC5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);//默认低电平会点亮LED，这里给设置成高电平，让其关闭
}
//LED1打开关闭函数PC4为LED1
void LED1_ON(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

void LED1_OFF(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}
//LED1电平翻转函数
void LED1_Turn(void){
	if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 0){
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
	else{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	}
}

//LED2打开关闭函数PC5为LED2
void LED2_ON(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}

void LED2_OFF(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}
//LED2电平翻转函数
void LED2_Turn(void){
	if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_5) == 0){
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
	}
	else{
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	}
}
