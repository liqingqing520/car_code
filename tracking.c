#include "tracking.h"   //红外模块 循迹

/*********************
红外传感器    黑光-1(灯灭)   白光-0（灯亮）

红外测试   白0-灯亮   黑1-灯灭   PB1-R   PA5-M     PA4-L
***********************/

/**************************************************************************
函数功能：红外GPIO初始化 
入口参数：无
返 回 值：无
解释说明: 
**************************************************************************/
void Tracking_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//1. 上电 使引脚能够工作
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.GPIO 配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4  |  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	//3.初始化GPIO配置
	GPIO_Init(GPIOA,  &GPIO_InitStructure);
	
	//2.GPIO 配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	//3.初始化GPIO配置
	GPIO_Init(GPIOB,  &GPIO_InitStructure);
}

/**************************************************************************
函数功能：读取中间红外数据 
入口参数：无
返 回 值：0-白色, 1-黑色
解释说明: 
**************************************************************************/
int Read_data_M(void)
{	
	return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5));
}

/**************************************************************************
函数功能：读取左边红外数据 
入口参数：无
返 回 值：0-白色, 1-黑色
解释说明: 
**************************************************************************/
int Read_data_L(void)
{	
	return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
}

/**************************************************************************
函数功能：读取右边红外数据 
入口参数：无
返 回 值：0-白色, 1-黑色
解释说明: 
**************************************************************************/
int Read_data_R(void)
{	
	return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1));
}
/**************************************************************************
函数功能：循迹 
入口参数：无
返 回 值：返回一个代表了转向、直行或者是停止的值
解释说明: 0-白色, 1-黑色， 
		  红外传感器排列顺序为左中右
		  二进制010 - 十六进制0x2	直行
		  二进制100 - 十六进制0x4	右转
		  二进制001 - 十六进制0x1	左转
		  二进制111 - 十六进制0x7	检测地点
		  二进制101	- 十六进制0x5	停止
**************************************************************************/
char Tracking(void)
{
	int r,l,m;
	l = Read_data_L();
	r = Read_data_R();
	m = Read_data_M();
	char flag = 0;
	flag = l << 2 | m << 1 | r; 
	
	return flag;
}










