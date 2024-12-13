 #include "motor.h"  //电机  控制小车速度
#include "usart.h"

/*********************
驱动板     stm32
  VM    --  5V  
  VCC   --  3.3V
  GND   --  GND 
  
  AIN1  --  PB12
  AIN2  --  PB13
  BIN1  --  PB14
  BIN2  --  PB15
  
  PWMA  --  PB8  定时器4 通道3    右轮
  PWMB  --  PB9  定时器4 通道4    左轮
***********************/

/**************************************************************************
函数功能：电机初始化 
入口参数：无
返 回 值：无
解释说明: 用到定时器4通道3和通道4输出PWM
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
	
	TIM_TimeBaseStructure.TIM_Period = 99; //ARR 周期 100us
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //72M时钟 预分频72 1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInit(TIM4, & TIM_TimeBaseStructure); 
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //使用定时器的PWM功能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //输出状态使能
	TIM_OCInitStructure.TIM_Pulse = 0; //CCR 脉冲值，电平翻转的时间，占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性为高
	
	TIM_OC3Init(TIM4, & TIM_OCInitStructure); //初始化的同时选择定时器4通道3
	TIM_OC4Init(TIM4, & TIM_OCInitStructure); //初始化的同时选择定时器4通道4

	TIM_Cmd(TIM4, ENABLE); 
	
	//TIM_SetCompare3(TIM4, 40);   //右
	//TIM_SetCompare4(TIM4, 40);   //左
}

/**************************************************************************
函数功能：电机右轮正转 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Run_R(void)
{	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

/**************************************************************************
函数功能：电机右轮反转 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Rerun_R(void)
{	
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/**************************************************************************
函数功能：电机右轮停止 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Stop_R(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}



/**************************************************************************
函数功能：电机左轮正转 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Run_L(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/**************************************************************************
函数功能：电机左轮反转 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Rerun_L(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

/**************************************************************************
函数功能：电机左轮停止 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Stop_L(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

/**************************************************************************
函数功能：电机左轮 右轮都正转 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Run_A(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);

	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/**************************************************************************
函数功能：电机左轮 右轮都停止 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Stop_A(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);	

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

/**************************************************************************
函数功能：电机左轮 右轮都反转 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void  Motor_Return_A(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}


/*******************
*  @brief  设置两个电机转速和方向
*  @param  motor_l:左轮设置参数、motor_r:右轮设置参数
*  @param  输入1~100   对应控制电机正方向速度在1%-100%
*  @param  输入-1~-100 对应控制电机反方向速度在1%-100%
*  @param  motor_r同理
*  @return  无
*
*******************/
void Motor_Set (int motor_l,int motor_r)
{
	//负的时候绝对值越小  PWM占空比越大    ps:这个可以先测试一下
	//现在的motor_l      -1   -99
	//给寄存器或者函数  99  1 
	//根据参数正负 设置选择方向
	if(motor_l < 0)
	{
		motor_l = -motor_l;
	}
	if(motor_r < 0)
	{
		motor_r = -motor_r;
	}

	
	if(motor_l > 99)  //超过PWM幅值   限幅
	{
		motor_l = 99;
	}
	if(motor_r > 99)  //超过PWM幅值   限幅
	{
		motor_r = 99;
	}
	

	//motor_l 设置右轮的转速
	TIM_SetCompare3(TIM4, motor_r);
	
	//motor2 设置左轮的转速
	TIM_SetCompare4(TIM4, motor_l);
	printf("left:%d  right:%d\r\n", motor_l, motor_r);
}  
