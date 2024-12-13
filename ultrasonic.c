#include "ultrasonic.h"
#include "SysTick.h"

/**************************************************************************
�������ܣ���������ʼ������
��ڲ�������
�� �� ֵ����
����˵��: 
					PA11  Trig����  �����������ź�����--��Ƭ������Ϊ���ģʽ 
					PA12  Echo����  �����������ź����--��Ƭ������Ϊ����ģʽ
**************************************************************************/
void UT_Init(void)
{
		GPIO_InitTypeDef GPIOinitStructure;
		
		//1.ʹ��GPIOʱ�ӡ���ʱ��ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		//2.����GPIO
		//Trig A11
		GPIOinitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; //�������ģʽ
		GPIOinitStructure.GPIO_Pin   = GPIO_Pin_11;
		GPIOinitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOinitStructure);
	
		//Echo A12
		GPIOinitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; //��������ģʽ
		GPIOinitStructure.GPIO_Pin   = GPIO_Pin_12;
		GPIO_Init(GPIOA, &GPIOinitStructure);
}



/**************************************************************************
�������ܣ���ȡ����
��ڲ�������
�� �� ֵ�������ֵ(��λmm)
����˵��: 
**************************************************************************/
u32 UT_Get_Length(void)
{
		u32 t=0;
	
		//����1�������崥�����ţ�trig������һ����Ϊ20us�ĸߵ�ƽ����
		//PS:����60us��Ϊ�˱���
		Trig_Send_Open;
		delay_us(20);
		Trig_Send_Close;	

		//����2�����뷽����ģ����Զ�����8��40KHz������
		//���ͬʱ�ز�����echo�˵ĵ�ƽ����0��Ϊ1
		while( Echo_Receive == 0 )//�ȴ������ź�
		{
//			delay_us(8);
//			t++;
//			if(t > 50)
//				return 0;
		}			

		while( Echo_Receive ) //����3����ȡ�ߵ�ƽ�ĳ���ʱ�䣬�Ƴ�����
		{
			delay_us(8);		//8usʱ�����3mm�ľ���	
			t++;	
		}

		return 3*(t/2);			//���ز⵽�ľ��룬�������2���������������ε�ʱ��

}
