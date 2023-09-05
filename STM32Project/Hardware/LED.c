/**
  ******************************************************************************
  * @file    	LED.c
  * @author  	eisbre
  * @version	None
  * @date    	2023/5/10
  * @brief   	���ư���LED��PC4ΪLED1��PC5ΪLED2���͵�ƽ����

  ******************************************************************************
  * @attention	None
  ******************************************************************************
  */
#include "stm32f10x.h"                  // Device header

void LED_Init(void){//��ʼ��LED��ʹ�õ���PC4��PC5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);//Ĭ�ϵ͵�ƽ�����LED����������óɸߵ�ƽ������ر�
}
//LED1�򿪹رպ���PC4ΪLED1
void LED1_ON(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

void LED1_OFF(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}
//LED1��ƽ��ת����
void LED1_Turn(void){
	if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4) == 0){
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
	else{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	}
}

//LED2�򿪹رպ���PC5ΪLED2
void LED2_ON(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}

void LED2_OFF(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}
//LED2��ƽ��ת����
void LED2_Turn(void){
	if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_5) == 0){
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
	}
	else{
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	}
}
