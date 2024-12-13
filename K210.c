#include "K210.h"      //���� ��ӡ������Ϣ

uint8_t Usart3_ReadBuf[256];	//����1 ��������
uint8_t Usart3_ReadCount = 0;	//����1 �����ֽڼ���

static void (*AI_func)(void);

/**************************************************************************
�������ܣ�����3��ʼ������
��ڲ�������
�� �� ֵ����
����˵��: PB10 USART3_TX        PB11 USART3_RX
**************************************************************************/
void Usart3_Init(int BaudRate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1. �ϵ� ʹ�����ܹ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.GPIO ����	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.��ʼ��GPIO����
	GPIO_Init(GPIOB,  &GPIO_InitStructure);
	
	//2.GPIO ����	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.��ʼ��GPIO����
	GPIO_Init(GPIOB,  &GPIO_InitStructure);
	
	//�ϵ� ʹ�����ܹ�����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	USART_InitStructure.USART_BaudRate = BaudRate; //���ò����ʣ��ⲿ����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�����ֳ�
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; //��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //�����շ�ģʽ
	
	USART_Init(USART3, &USART_InitStructure); //����1��ʼ��
	
	USART_Cmd(USART3, ENABLE); //ʹ�ܴ����豸
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//���ڽ����ж�ʹ��
	
	//�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************
�������ܣ�����3���͵����ֽ�����
��ڲ����������ַ�
�� �� ֵ����
����˵��: 
**************************************************************************/
void Usart3_Send_Byte(int ch)
{
	USART_SendData(USART3, ch); 
	//����Ƿ������
	while(SET != USART_GetFlagStatus(USART3, USART_FLAG_TXE)); 	
}

/**************************************************************************
�������ܣ�����3�����ַ���
��ڲ������ַ���
�� �� ֵ����
����˵��: 
**************************************************************************/
void Usart3_Send_Str(char * ch)
{
	while(*ch != '\0')
	{
		USART_SendData(USART3, *ch); 
		//����Ƿ������
		while(SET != USART_GetFlagStatus(USART3, USART_FLAG_TXE)); 	
		ch++;
	}
}

/**************************************************************************
�������ܣ�����3���յ����ַ�
��ڲ�������
�� �� ֵ���ַ�
����˵��: 
**************************************************************************/
unsigned char Usart3_Recv_Byte(void)
{
	//�ȴ������ݽ���
	while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
	
	//���ؽ��յ�������
	return USART_ReceiveData(USART3);
}


/**************************************************************************
�������ܣ�����3�жϴ�����: �����ж�
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void USART3_IRQHandler(void)
{
	//�����жϲ���
	if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
	{
		USART_ReceiveData(USART3);
	}
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		 AI_func();
	}
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);//��������жϱ�־λ
}

/**************************************************************************
�������ܣ��жϷ������һ֡����
��ڲ�������
�� �� ֵ������0��ʾ�����������  ����1��ʾû���ڽ�������  ����0��ʾ�������ڽ�����
����˵��: 
**************************************************************************/
uint8_t Usart3_WaitReasFinish(void)
{
	static uint16_t Usart_LastReadCount = 0;//��¼�ϴεļ���ֵ
	
	if(Usart3_ReadCount == 0)
	{
		Usart_LastReadCount = 0;
		return 1;//��ʾû���ڽ�������
	}
	
	if(Usart3_ReadCount == Usart_LastReadCount)//�����μ���ֵ�����ϴμ���ֵ
	{
		Usart3_ReadCount = 0;
		Usart_LastReadCount = 0;
		return 0;//�Ѿ����������
	}
	
	Usart_LastReadCount = Usart3_ReadCount;
	
	return 2;//��ʾ���ڽ�����
}

/**************************************************************************
�������ܣ��жϷ������һ֡����
��ڲ�������
�� �� ֵ������0��ʾ�����������  ����1��ʾû���ڽ�������  ����0��ʾ�������ڽ�����
����˵��: 
**************************************************************************/
void AI_reg(void (*pfunc)(void))
{
	AI_func = pfunc;
}

