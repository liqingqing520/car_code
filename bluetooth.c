#include "bluetooth.h"      //串口 打印调试信息

static void (*Ser_func)(void);

/*********************************************************************************************************
注意：在使用蓝牙模块HC-05之前，需要先设置一下，按照下面的步骤来即可

需要器件： 蓝牙、USB-TTL、杜邦线      PS:需要串口调试助手 （XCOM或者其他的也可以）


HC-05蓝牙模块：使用AT命令配置详细步骤

进入AT命令模式方法：
先按住按键，再给模块上电，此时灯是慢闪，进入AT命令模式，默认波特率是38400。

打开此电脑---管理---设备管理器---端口（COM和LPT）会看到有USB-SERIAL CH340(COM?)出现即可

ps:如果没有出现，需要安装开发板USB转串口CH340驱动

注意：如果不按按钮给模块上电，是通信模式，灯光为快闪。 使用usb转ttl 电源接5V。 XCOM需要把发送新行勾上。
 
常用AT命令：
AT+ORGL　　　　　　　　　　  恢复出厂默认设置
AT+VERSION?　　　　 　　　　 返回HC-05的软件版本号

AT+NAME？　　　　　　　　　  返回HC-05的名字
AT+NAME=car_uart　　　　    修改蓝牙模块的名字为car_uart

AT+UART?　　　　　　　　　　  返回蓝牙波特率
AT+UART=115200,0,0　　　　　设置串口波特率115200,1位停止位，无校验
【波特率　　停止位： 0=1位，1=2位　　 校验位： 0=无校验，1=奇校验，2=偶校验　　默认设置为9600,0,0】

AT+PSWD？　　　　　　　　　  查询配对密码
AT+PSWD="1234"　　　　　  设置密码1234
AT+ROLE?　　　　　　　　　　 查询主从状态，=1：设置成主，=0：设置成从，=2：设置成回环
AT+ROLE=0　　　　　　　　　  设置成从



设置好之后，把蓝牙模块连接到STM32板子上，RXD--A2   TXD--A3 电源接5V  地接地

打开电脑的蓝牙，找到car_uart, 输入密码1234，配对好。

打开XCOM， 选择哪一个端口呢？  打开电脑蓝牙设置，更多蓝牙选项，COM端口， 看到【COM?  传出  car_uart 'SerialPort' 】

在XCOM选择对应的端口，这是灯是慢闪状态，就能接收到蓝牙发给电脑的数据了



PS： 如果使用printf函数打印不出来，魔术棒--Target--Use MicroLIB（勾上即可）
*****************************************************************************************************************/


uint8_t Usart2_ReadBuf[256];	//串口2 缓冲数组
uint8_t Usart2_ReadCount = 0;	//串口2 接收字节计数

/**************************************************************************
函数功能：蓝牙初始化函数
入口参数：无
返 回 值：无
解释说明: PA2 USART2_TX        PA3 USART2_RX
**************************************************************************/
void Usart2_Init(int BaudRate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//串口对应GPIO口初始化
	//1. 上电 使引脚能够工作
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//2.GPIO 配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.初始化GPIO配置
	GPIO_Init(GPIOA,  &GPIO_InitStructure);
	
	//2.GPIO 配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//3.初始化GPIO配置
	GPIO_Init(GPIOA,  &GPIO_InitStructure);
	
	//上电 使串口能够工作
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_InitStructure.USART_BaudRate = BaudRate; //设置波特率，外部传入
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //设置字长
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; //无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //数据收发模式
	
	USART_Init(USART2, &USART_InitStructure); //串口1初始化
	
	USART_Cmd(USART2, ENABLE); //使能串口设备
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//串口接收中断使能
	
	//中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************
函数功能：蓝牙发送单个字节数据
入口参数：单个字符
返 回 值：无
解释说明: 
**************************************************************************/
void Usart2_Send_Byte(int ch)
{
	USART_SendData(USART2, ch); 
	//检测是否发送完成
	while(SET != USART_GetFlagStatus(USART2, USART_FLAG_TXE)); 	
}

/**************************************************************************
函数功能：蓝牙发送字符串
入口参数：字符串
返 回 值：无
解释说明: 
**************************************************************************/
void Usart2_Send_Str(char * ch)
{
	while(*ch != '\0')
	{
		USART_SendData(USART2, *ch); 
		//检测是否发送完成
		while(SET != USART_GetFlagStatus(USART2, USART_FLAG_TXE)); 	
		ch++;
	}
}

/**************************************************************************
函数功能：蓝牙接收单个字符
入口参数：无
返 回 值：字符
解释说明: 
**************************************************************************/
unsigned char Usart2_Recv_Byte(void)
{
	//等待有数据进来
	while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	
	//返回接收到的数据
	return USART_ReceiveData(USART2);
}



/**************************************************************************
函数功能：串口2中断处理函数: 接收中断
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void USART2_IRQHandler(void)
{
	//接收中断产生
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
	{
		USART_ReceiveData(USART2);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		Ser_func(); 
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除接收中断标志位
}

/**************************************************************************
函数功能：判断否接收完一帧数据
入口参数：无
返 回 值：返回0表示接收数据完成  返回1表示没有在接收数据  返回0表示数据正在接收中
解释说明: 
**************************************************************************/
uint8_t Usart2_WaitReasFinish(void)
{
	static uint16_t Usart_LastReadCount = 0;//记录上次的计数值
	
	if(Usart2_ReadCount == 0)
	{
		Usart_LastReadCount = 0;
		return 1;//表示没有在接收数据
	}
	
	if(Usart2_ReadCount == Usart_LastReadCount)//如果这次计数值等于上次计数值
	{
		Usart2_ReadCount = 0;
		Usart_LastReadCount = 0;
		return 0;//已经接收完成了
	}
	
	Usart_LastReadCount = Usart2_ReadCount;
	
	return 2;//表示正在接收中
}

void Usart2_reg(void (*pfunc)(void))
{
		Ser_func = pfunc;
}

