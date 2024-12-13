#include "led.h"


void led_c13_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	//����Դ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//��ʼ�� PC13 �������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Ϩ�����е�
	GPIO_SetBits(GPIOC, GPIO_Pin_13);	
}

void led_c13_on(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void led_c13_off(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}


