#include "K210.h"      //串口 打印调试信息

uint8_t Usart3_ReadBuf[256];	//串口1 缓冲数组
uint8_t Usart3_ReadCount = 0;	//串口1 接收字节计数

static void (*AI_func)(void);

/**************************************************************************
函数功能：串口3初始化函数
入口参数：无
返 回 值：无
解释说明: PB10 USART3_TX        PB11 USART3_RX
**************************************************************************/
void Usart3_Init(int BaudRate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1. 上电 使引脚能够工作
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.GPIO 配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.初始化GPIO配置
	GPIO_Init(GPIOB,  &GPIO_InitStructure);
	
	//2.GPIO 配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.初始化GPIO配置
	GPIO_Init(GPIOB,  &GPIO_InitStructure);
	
	//上电 使串口能够工作
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	USART_InitStructure.USART_BaudRate = BaudRate; //设置波特率，外部传入
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //设置字长
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; //无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //数据收发模式
	
	USART_Init(USART3, &USART_InitStructure); //串口1初始化
	
	USART_Cmd(USART3, ENABLE); //使能串口设备
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//串口接收中断使能
	
	//中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************
函数功能：串口3发送单个字节数据
入口参数：单个字符
返 回 值：无
解释说明: 
**************************************************************************/
void Usart3_Send_Byte(int ch)
{
	USART_SendData(USART3, ch); 
	//检测是否发送完成
	while(SET != USART_GetFlagStatus(USART3, USART_FLAG_TXE)); 	
}

/**************************************************************************
函数功能：串口3发送字符串
入口参数：字符串
返 回 值：无
解释说明: 
**************************************************************************/
void Usart3_Send_Str(char * ch)
{
	while(*ch != '\0')
	{
		USART_SendData(USART3, *ch); 
		//检测是否发送完成
		while(SET != USART_GetFlagStatus(USART3, USART_FLAG_TXE)); 	
		ch++;
	}
}

/**************************************************************************
函数功能：串口3接收单个字符
入口参数：无
返 回 值：字符
解释说明: 
**************************************************************************/
unsigned char Usart3_Recv_Byte(void)
{
	//等待有数据进来
	while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
	
	//返回接收到的数据
	return USART_ReceiveData(USART3);
}


/**************************************************************************
函数功能：串口3中断处理函数: 接收中断
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void USART3_IRQHandler(void)
{
	//接收中断产生
	if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
	{
		USART_ReceiveData(USART3);
	}
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		 AI_func();
	}
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清除接收中断标志位
}

/**************************************************************************
函数功能：判断否接收完一帧数据
入口参数：无
返 回 值：返回0表示接收数据完成  返回1表示没有在接收数据  返回0表示数据正在接收中
解释说明: 
**************************************************************************/
uint8_t Usart3_WaitReasFinish(void)
{
	static uint16_t Usart_LastReadCount = 0;//记录上次的计数值
	
	if(Usart3_ReadCount == 0)
	{
		Usart_LastReadCount = 0;
		return 1;//表示没有在接收数据
	}
	
	if(Usart3_ReadCount == Usart_LastReadCount)//如果这次计数值等于上次计数值
	{
		Usart3_ReadCount = 0;
		Usart_LastReadCount = 0;
		return 0;//已经接收完成了
	}
	
	Usart_LastReadCount = Usart3_ReadCount;
	
	return 2;//表示正在接收中
}

/**************************************************************************
函数功能：判断否接收完一帧数据
入口参数：无
返 回 值：返回0表示接收数据完成  返回1表示没有在接收数据  返回0表示数据正在接收中
解释说明: 
**************************************************************************/
void AI_reg(void (*pfunc)(void))
{
	AI_func = pfunc;
}

