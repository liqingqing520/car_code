#include "SysTick.h"


static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数


//void delay_ms(int ms)
//{
//	int i = 0;
//	while(ms--)
//		for(i=0; i<8050; i++);
//}



/**************************************************************************
函数功能：时钟配置  
入口参数：无
返 回 值：无
解释说明: 确保为72MHz
**************************************************************************/
void  Clock_Init(void) //配置实时时钟
{
	RCC_DeInit();  //默认时钟初始化
	RCC_HSEConfig(RCC_HSE_ON); //设置外部高速晶振
	while(SUCCESS != RCC_WaitForHSEStartUp() ); //等待起振
	
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //设置PLL时钟源
	RCC_PLLCmd(ENABLE);  //使能 Enables or disables the PLL.
	while(RESET ==  RCC_GetFlagStatus(RCC_FLAG_PLLRDY)); //检查标志位设置与否 PLL clock ready
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //设置系统时钟 PLL selected as system clock
	RCC_HCLKConfig(RCC_SYSCLK_Div1); //Configures the AHB clock (HCLK).AHB clock = SYSCLK
	RCC_PCLK1Config(RCC_HCLK_Div2);//Configures the Low Speed APB clock (PCLK1).
	RCC_PCLK2Config(RCC_HCLK_Div1); //Configures the High Speed APB clock (PCLK2).	
}


/*******************************************************************************
* 函 数 名         : SysTick_Init
* 函数功能		   : SysTick初始化，SYSTICK的时钟固定为AHB时钟的1/8
* 输    入         : SYSCLK:系统时钟频率
* 输    出         : 无
*******************************************************************************/
void SysTick_Init(u8 SYSCLK)
{
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK/8;						//SYSCLK的8分频 保存1us所需的计数次数
	fac_ms=(u16)fac_us*1000;				//每个ms需要的systick时钟数   
}								    

/*******************************************************************************
* 函 数 名         : delay_us
* 函数功能		   : us延时，
* 输    入         : nus：要延时的us数
					注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
* 输    出         : 无
*******************************************************************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL|=0x01 ; //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	
	//等待时间到达   SysTick->CTRL的bit0为1并且bit16位为1条件成立
	SysTick->CTRL&=~0x01; //关闭计数器
	SysTick->VAL =0X00;       				//清空计数器 
}

/*******************************************************************************
* 函 数 名         : delay_nms
* 函数功能		   : ms延时，
* 输    入         : nms：要延时的ms数
					注意:nms的值,SysTick->LOAD为24位寄存器，
-------》			参数最大值 nms<=0xffffff/9/1000 
					对72M条件下,nms<=1864 
					对168M条件下,nms<=798ms 
* 输    出         : 无
*******************************************************************************/
void delay_nms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           			//清空计数器
	SysTick->CTRL|=0x01 ; //开始倒数 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~0x01; //关闭计数器
	SysTick->VAL =0X00;     		  		//清空计数器	  	    
}

/*******************************************************************************
* 函 数 名         : delay_ms
* 函数功能		   : ms延时，
* 输    入         : nms：要延时的ms数 0~65535
* 输    出         : 无
*******************************************************************************/
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;		//这里用540,是考虑到某些客户可能超频使用,
							//比如超频到248M的时候,delay_nms最大只能延时541ms左右了
	u16 remain=nms%540;
	while(repeat)
	{
		delay_nms(540);
		repeat--;
	}
	if(remain)delay_nms(remain);
} 


