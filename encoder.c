#include "encoder.h"  //������ģ�� ����

/*********************
������,��¼����������
  PA0(��ʱ��2 TIM2_CH1)    
  PA1(��ʱ��2 TIM2_CH2) 

������,��¼����������
  PA6(��ʱ��3 TIM3_CH1) 
  PA7(��ʱ��3 TIM3_CH2)  
***********************/

/**************************************************************************
�������ܣ���ʱ��2��ʼ�� 
��ڲ�������
�� �� ֵ����
����˵��: ����Ϊ������ģʽ, ��¼���ֵ�ת��
**************************************************************************/
void Tim2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
  TIM_ICInitTypeDef TIM_ICInitStructure;  
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ�ܶ�ʱ��2��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PB�˿�ʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOA
  
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Ԥ��Ƶ�� 
  TIM_TimeBaseStructure.TIM_Period = 65535; //�趨�������Զ���װֵ ���ֵ AAR
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷָ����Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//ʹ�ñ�����ģʽ3 
  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICStructInit(&TIM_ICInitStructure); //ʹ��Ĭ��ֵ��ʼ���ṹ���Ա
  TIM_ICInitStructure.TIM_ICFilter = 10;  //�˲���ϵ��
  TIM_ICInit(TIM2, &TIM_ICInitStructure); //���벶���ʼ��
  
	
  //Reset counter
  TIM_SetCounter(TIM2,  0);  //��TIM2->CNT = 0
  TIM_Cmd(TIM2, ENABLE); 	
}

/**************************************************************************
�������ܣ���ʱ��3��ʼ�� 
��ڲ�������
�� �� ֵ����
����˵��: ����Ϊ������ģʽ, ��¼���ֵ�ת��
**************************************************************************/
void Tim3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
  TIM_ICInitTypeDef TIM_ICInitStructure;  
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��3��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PB�˿�ʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOB
  
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Ԥ��Ƶ�� 
  TIM_TimeBaseStructure.TIM_Period = 65535-1; //�趨�������Զ���װֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
 
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);

  TIM_SetCounter(TIM3,0);
  TIM_Cmd(TIM3, ENABLE); 
}

/**************************************************************************
�������ܣ���ʱ��1�ж�����
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void TIM1_NVIC_Configuration(void) //�ж�����
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    									
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	 //�ж�ͨ��Ϊ��ʱ��1�ж� 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //�ж�ʹ��
    NVIC_Init(&NVIC_InitStructure);  //�жϹ�������ʼ��
}



/**************************************************************************
�������ܣ���ʱ��1��ʼ������
��ڲ�������
�� �� ֵ����
����˵��: ��ʱһ��ʱ��(2ms)��ȡ����������������
**************************************************************************/
void TIM1_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
    
	TIM_DeInit(TIM1);											/* ����������ʱ�� */

    TIM_TimeBaseStructure.TIM_Period= 2000;		 				/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ)  2ms*/
    TIM_TimeBaseStructure.TIM_Prescaler= 72;						/* ʱ��Ԥ��Ƶ�� 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* �ⲿʱ�Ӳ�����Ƶ */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	/* ���ϼ���ģʽ */
  	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);						/* �������жϱ�־ */

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);					/* �����жϴ���*/    
	
	TIM_Cmd(TIM1, ENABLE);		/* ʹ�ܶ�ʱ�� */ 
}
/**************************************************************************
�������ܣ���ʱ��1���غ���
��ڲ�����0����1��0����رն�ʱ��1��1��������ʱ��1
�� �� ֵ����
����˵��: 
**************************************************************************/
void TIM1_Switch(int flag)
{
	if(flag == 1)
	{
		TIM_Cmd(TIM1, ENABLE);
	}
	else if(flag == 0)
		TIM_Cmd(TIM1, DISABLE);
}
/**************************************************************************
�������ܣ���������س�ʼ��
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void Encoder_Init(void)
{
	Tim2_Init();
	Tim3_Init();
	TIM1_NVIC_Configuration();
	TIM1_Configuration();
}

/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ�� (2-����, 3-����)
�� �� ֵ���ٶ�ֵ
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
	int Encoder_TIM;    
	switch(TIMX)
	{
		case 2:  
			Encoder_TIM = (short)TIM2 -> CNT;  
			TIM2 -> CNT=0;
			break;	
		case 3:  
			Encoder_TIM = (short)TIM3 -> CNT;  
			TIM3 -> CNT=0;
			break;	
		default:  
			Encoder_TIM = 0; 
			break;
	 }
	 return Encoder_TIM;
}
