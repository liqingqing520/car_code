#include "encoder.h"  //编码器模块 测速

/*********************
编码器,记录左轮脉冲数
  PA0(定时器2 TIM2_CH1)    
  PA1(定时器2 TIM2_CH2) 

编码器,记录右轮脉冲数
  PA6(定时器3 TIM3_CH1) 
  PA7(定时器3 TIM3_CH2)  
***********************/

/**************************************************************************
函数功能：定时器2初始化 
入口参数：无
返 回 值：无
解释说明: 配置为编码器模式, 记录左轮的转速
**************************************************************************/
void Tim2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
  TIM_ICInitTypeDef TIM_ICInitStructure;  
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器2的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PB端口时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA
  
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
  TIM_TimeBaseStructure.TIM_Period = 65535; //设定计数器自动重装值 最大值 AAR
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分割：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//使用编码器模式3 
  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICStructInit(&TIM_ICInitStructure); //使用默认值初始化结构体成员
  TIM_ICInitStructure.TIM_ICFilter = 10;  //滤波器系数
  TIM_ICInit(TIM2, &TIM_ICInitStructure); //输入捕获初始化
  
	
  //Reset counter
  TIM_SetCounter(TIM2,  0);  //将TIM2->CNT = 0
  TIM_Cmd(TIM2, ENABLE); 	
}

/**************************************************************************
函数功能：定时器3初始化 
入口参数：无
返 回 值：无
解释说明: 配置为编码器模式, 记录右轮的转速
**************************************************************************/
void Tim3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
  TIM_ICInitTypeDef TIM_ICInitStructure;  
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PB端口时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB
  
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
  TIM_TimeBaseStructure.TIM_Period = 65535-1; //设定计数器自动重装值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
 
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);

  TIM_SetCounter(TIM3,0);
  TIM_Cmd(TIM3, ENABLE); 
}

/**************************************************************************
函数功能：定时器1中断配置
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void TIM1_NVIC_Configuration(void) //中断配置
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    									
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	 //中断通道为定时器1中断 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //主优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //中断使能
    NVIC_Init(&NVIC_InitStructure);  //中断管理器初始化
}



/**************************************************************************
函数功能：定时器1初始化配置
入口参数：无
返 回 值：无
解释说明: 定时一段时间(2ms)获取编码器跳变沿数量
**************************************************************************/
void TIM1_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
    
	TIM_DeInit(TIM1);											/* 重新启动定时器 */

    TIM_TimeBaseStructure.TIM_Period= 2000;		 				/* 自动重装载寄存器周期的值(计数值)  2ms*/
    TIM_TimeBaseStructure.TIM_Prescaler= 72;						/* 时钟预分频数 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* 外部时钟采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	/* 向上计数模式 */
  	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);						/* 清除溢出中断标志 */

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);					/* 开启中断触发*/    
	
	TIM_Cmd(TIM1, ENABLE);		/* 使能定时器 */ 
}
/**************************************************************************
函数功能：定时器1开关函数
入口参数：0或者1，0代表关闭定时器1，1代表开启定时器1
返 回 值：无
解释说明: 
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
函数功能：编码器相关初始化
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void Encoder_Init(void)
{
	Tim2_Init();
	Tim3_Init();
	TIM1_NVIC_Configuration();
	TIM1_Configuration();
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器 (2-左轮, 3-右轮)
返 回 值：速度值
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
