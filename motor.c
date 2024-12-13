 #include "motor.h"  //���  ����С���ٶ�
#include "usart.h"

/*********************
������     stm32
  VM    --  5V  
  VCC   --  3.3V
  GND   --  GND 
  
  AIN1  --  PB12
  AIN2  --  PB13
  BIN1  --  PB14
  BIN2  --  PB15
  
  PWMA  --  PB8  ��ʱ��4 ͨ��3    ����
  PWMB  --  PB9  ��ʱ��4 ͨ��4    ����
***********************/

/**************************************************************************
�������ܣ������ʼ�� 
��ڲ�������
�� �� ֵ����
����˵��: �õ���ʱ��4ͨ��3��ͨ��4���PWM
**************************************************************************/
void Motor_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 99; //ARR ���� 100us
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //72Mʱ�� Ԥ��Ƶ72 1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInit(TIM4, & TIM_TimeBaseStructure); 
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ʹ�ö�ʱ����PWM����
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //���״̬ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //CCR ����ֵ����ƽ��ת��ʱ�䣬ռ�ձ�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������Ϊ��
	
	TIM_OC3Init(TIM4, & TIM_OCInitStructure); //��ʼ����ͬʱѡ��ʱ��4ͨ��3
	TIM_OC4Init(TIM4, & TIM_OCInitStructure); //��ʼ����ͬʱѡ��ʱ��4ͨ��4

	TIM_Cmd(TIM4, ENABLE); 
	
	//TIM_SetCompare3(TIM4, 40);   //��
	//TIM_SetCompare4(TIM4, 40);   //��
}

/**************************************************************************
�������ܣ����������ת 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Run_R(void)
{	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

/**************************************************************************
�������ܣ�������ַ�ת 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Rerun_R(void)
{	
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/**************************************************************************
�������ܣ��������ֹͣ 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Stop_R(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}



/**************************************************************************
�������ܣ����������ת 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Run_L(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/**************************************************************************
�������ܣ�������ַ�ת 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Rerun_L(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

/**************************************************************************
�������ܣ��������ֹͣ 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Stop_L(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

/**************************************************************************
�������ܣ�������� ���ֶ���ת 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Run_A(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);

	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/**************************************************************************
�������ܣ�������� ���ֶ�ֹͣ 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Stop_A(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);	

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

/**************************************************************************
�������ܣ�������� ���ֶ���ת 
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void  Motor_Return_A(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}


/*******************
*  @brief  �����������ת�ٺͷ���
*  @param  motor_l:�������ò�����motor_r:�������ò���
*  @param  ����1~100   ��Ӧ���Ƶ���������ٶ���1%-100%
*  @param  ����-1~-100 ��Ӧ���Ƶ���������ٶ���1%-100%
*  @param  motor_rͬ��
*  @return  ��
*
*******************/
void Motor_Set (int motor_l,int motor_r)
{
	//����ʱ�����ֵԽС  PWMռ�ձ�Խ��    ps:��������Ȳ���һ��
	//���ڵ�motor_l      -1   -99
	//���Ĵ������ߺ���  99  1 
	//���ݲ������� ����ѡ����
	if(motor_l < 0)
	{
		motor_l = -motor_l;
	}
	if(motor_r < 0)
	{
		motor_r = -motor_r;
	}

	
	if(motor_l > 99)  //����PWM��ֵ   �޷�
	{
		motor_l = 99;
	}
	if(motor_r > 99)  //����PWM��ֵ   �޷�
	{
		motor_r = 99;
	}
	

	//motor_l �������ֵ�ת��
	TIM_SetCompare3(TIM4, motor_r);
	
	//motor2 �������ֵ�ת��
	TIM_SetCompare4(TIM4, motor_l);
	printf("left:%d  right:%d\r\n", motor_l, motor_r);
}  
