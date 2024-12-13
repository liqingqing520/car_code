#include "bluetooth.h"      //���� ��ӡ������Ϣ

static void (*Ser_func)(void);

/*********************************************************************************************************
ע�⣺��ʹ������ģ��HC-05֮ǰ����Ҫ������һ�£���������Ĳ���������

��Ҫ������ ������USB-TTL���Ű���      PS:��Ҫ���ڵ������� ��XCOM����������Ҳ���ԣ�


HC-05����ģ�飺ʹ��AT����������ϸ����

����AT����ģʽ������
�Ȱ�ס�������ٸ�ģ���ϵ磬��ʱ��������������AT����ģʽ��Ĭ�ϲ�������38400��

�򿪴˵���---����---�豸������---�˿ڣ�COM��LPT���ῴ����USB-SERIAL CH340(COM?)���ּ���

ps:���û�г��֣���Ҫ��װ������USBת����CH340����

ע�⣺���������ť��ģ���ϵ磬��ͨ��ģʽ���ƹ�Ϊ������ ʹ��usbתttl ��Դ��5V�� XCOM��Ҫ�ѷ������й��ϡ�
 
����AT���
AT+ORGL��������������������  �ָ�����Ĭ������
AT+VERSION?�������� �������� ����HC-05������汾��

AT+NAME��������������������  ����HC-05������
AT+NAME=car_uart��������    �޸�����ģ�������Ϊcar_uart

AT+UART?��������������������  ��������������
AT+UART=115200,0,0�������������ô��ڲ�����115200,1λֹͣλ����У��
�������ʡ���ֹͣλ�� 0=1λ��1=2λ���� У��λ�� 0=��У�飬1=��У�飬2=żУ�顡��Ĭ������Ϊ9600,0,0��

AT+PSWD��������������������  ��ѯ�������
AT+PSWD="1234"����������  ��������1234
AT+ROLE?�������������������� ��ѯ����״̬��=1�����ó�����=0�����óɴӣ�=2�����óɻػ�
AT+ROLE=0������������������  ���óɴ�



���ú�֮�󣬰�����ģ�����ӵ�STM32�����ϣ�RXD--A2   TXD--A3 ��Դ��5V  �ؽӵ�

�򿪵��Ե��������ҵ�car_uart, ��������1234����Ժá�

��XCOM�� ѡ����һ���˿��أ�  �򿪵����������ã���������ѡ�COM�˿ڣ� ������COM?  ����  car_uart 'SerialPort' ��

��XCOMѡ���Ӧ�Ķ˿ڣ����ǵ�������״̬�����ܽ��յ������������Ե�������



PS�� ���ʹ��printf������ӡ��������ħ����--Target--Use MicroLIB�����ϼ��ɣ�
*****************************************************************************************************************/


uint8_t Usart2_ReadBuf[256];	//����2 ��������
uint8_t Usart2_ReadCount = 0;	//����2 �����ֽڼ���

/**************************************************************************
�������ܣ�������ʼ������
��ڲ�������
�� �� ֵ����
����˵��: PA2 USART2_TX        PA3 USART2_RX
**************************************************************************/
void Usart2_Init(int BaudRate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//���ڶ�ӦGPIO�ڳ�ʼ��
	//1. �ϵ� ʹ�����ܹ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//2.GPIO ����	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.��ʼ��GPIO����
	GPIO_Init(GPIOA,  &GPIO_InitStructure);
	
	//2.GPIO ����	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.��ʼ��GPIO����
	GPIO_Init(GPIOA,  &GPIO_InitStructure);
	
	//�ϵ� ʹ�����ܹ�����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_InitStructure.USART_BaudRate = BaudRate; //���ò����ʣ��ⲿ����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�����ֳ�
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //����żУ��
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; //��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //�����շ�ģʽ
	
	USART_Init(USART2, &USART_InitStructure); //����1��ʼ��
	
	USART_Cmd(USART2, ENABLE); //ʹ�ܴ����豸
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//���ڽ����ж�ʹ��
	
	//�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************
�������ܣ��������͵����ֽ�����
��ڲ����������ַ�
�� �� ֵ����
����˵��: 
**************************************************************************/
void Usart2_Send_Byte(int ch)
{
	USART_SendData(USART2, ch); 
	//����Ƿ������
	while(SET != USART_GetFlagStatus(USART2, USART_FLAG_TXE)); 	
}

/**************************************************************************
�������ܣ����������ַ���
��ڲ������ַ���
�� �� ֵ����
����˵��: 
**************************************************************************/
void Usart2_Send_Str(char * ch)
{
	while(*ch != '\0')
	{
		USART_SendData(USART2, *ch); 
		//����Ƿ������
		while(SET != USART_GetFlagStatus(USART2, USART_FLAG_TXE)); 	
		ch++;
	}
}

/**************************************************************************
�������ܣ��������յ����ַ�
��ڲ�������
�� �� ֵ���ַ�
����˵��: 
**************************************************************************/
unsigned char Usart2_Recv_Byte(void)
{
	//�ȴ������ݽ���
	while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	
	//���ؽ��յ�������
	return USART_ReceiveData(USART2);
}



/**************************************************************************
�������ܣ�����2�жϴ�����: �����ж�
��ڲ�������
�� �� ֵ����
����˵��: 
**************************************************************************/
void USART2_IRQHandler(void)
{
	//�����жϲ���
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
	{
		USART_ReceiveData(USART2);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		Ser_func(); 
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);//��������жϱ�־λ
}

/**************************************************************************
�������ܣ��жϷ������һ֡����
��ڲ�������
�� �� ֵ������0��ʾ�����������  ����1��ʾû���ڽ�������  ����0��ʾ�������ڽ�����
����˵��: 
**************************************************************************/
uint8_t Usart2_WaitReasFinish(void)
{
	static uint16_t Usart_LastReadCount = 0;//��¼�ϴεļ���ֵ
	
	if(Usart2_ReadCount == 0)
	{
		Usart_LastReadCount = 0;
		return 1;//��ʾû���ڽ�������
	}
	
	if(Usart2_ReadCount == Usart_LastReadCount)//�����μ���ֵ�����ϴμ���ֵ
	{
		Usart2_ReadCount = 0;
		Usart_LastReadCount = 0;
		return 0;//�Ѿ����������
	}
	
	Usart_LastReadCount = Usart2_ReadCount;
	
	return 2;//��ʾ���ڽ�����
}

void Usart2_reg(void (*pfunc)(void))
{
		Ser_func = pfunc;
}

