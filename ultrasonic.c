#include "ultrasonic.h"
#include "SysTick.h"

/**************************************************************************
函数功能：超声波初始化函数
入口参数：无
返 回 值：无
解释说明: 
					PA11  Trig引脚  超声波触发信号输入--单片机设置为输出模式 
					PA12  Echo引脚  超声波回响信号输出--单片机设置为输入模式
**************************************************************************/
void UT_Init(void)
{
		GPIO_InitTypeDef GPIOinitStructure;
		
		//1.使能GPIO时钟、计时器时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		//2.配置GPIO
		//Trig A11
		GPIOinitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; //推挽输出模式
		GPIOinitStructure.GPIO_Pin   = GPIO_Pin_11;
		GPIOinitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOinitStructure);
	
		//Echo A12
		GPIOinitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; //浮空输入模式
		GPIOinitStructure.GPIO_Pin   = GPIO_Pin_12;
		GPIO_Init(GPIOA, &GPIOinitStructure);
}



/**************************************************************************
函数功能：获取距离
入口参数：无
返 回 值：距离的值(单位mm)
解释说明: 
**************************************************************************/
u32 UT_Get_Length(void)
{
		u32 t=0;
	
		//步骤1：给脉冲触发引脚（trig）输入一个长为20us的高电平方波
		//PS:这里60us是为了保险
		Trig_Send_Open;
		delay_us(20);
		Trig_Send_Close;	

		//步骤2：输入方波后，模块会自动发射8个40KHz的声波
		//与此同时回波引脚echo端的电平会由0变为1
		while( Echo_Receive == 0 )//等待回响信号
		{
//			delay_us(8);
//			t++;
//			if(t > 50)
//				return 0;
		}			

		while( Echo_Receive ) //步骤3：获取高电平的持续时间，推出距离
		{
			delay_us(8);		//8us时间就是3mm的距离	
			t++;	
		}

		return 3*(t/2);			//返回测到的距离，这里除以2就是声波来回两次的时间

}
