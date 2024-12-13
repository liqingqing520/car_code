#include "ADC.h"

//uint16_t AD_Value[3];//定义数据用来存放ADC三个通道转换的数据
vu16 AD_Value[50][3]; //存放ADC值的内存空间，需要再使用的位置通过extern引用

//红色一百以内
//白色八百左右
//黑色3000左右

/* */
/*********************************************************************
函数功能：ADC引脚初始化
函数参数：无
返 回 值：无
说	  明：采用ADC1的通道4 通道5 通道9 引脚为PA^4 PA^5 PB^1 模式必须是模拟输入
*********************************************************************/
void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,  ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN ;
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN ;
	GPIO_Init(GPIOB , &GPIO_InitStruct);
}


/*********************************************************************
函数功能：ADC初始化，因为需要使用多通道所以需要使用DMA
函数参数：无
返 回 值：无
*********************************************************************/
void ADC_Config(void)
{
	ADC_InitTypeDef  ADC_InitStruct;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	
	
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//允许连续转换 
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式，就是只用ADC1
	ADC_InitStruct.ADC_NbrOfChannel = Channel_Num; //3个通道转换
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//扫描模式，就是扫描多个通道的
	ADC_Init(ADC1,&ADC_InitStruct);
	
	

	/*ADC_RegularChannelConfig（ADCx,通道y，序列z,采样时间），通道：0~17，序列：1~18*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 3, ADC_SampleTime_55Cycles5);
	
	DMA_DeInit(DMA1_Channel1);
	//DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC1的数据寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16位半字
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不自增
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;//存储器地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器、半字
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器地址不自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设作为传输源
	DMA_InitStructure.DMA_BufferSize = Channel_Num;//计数器，一个通道就计一次，这里使用三个通道
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//非存储器到存储器
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//DMA通道响应的优先级设置
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
    /*使能*/
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
    /*ADC数据校准*/ 
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
    /*ADC软件触发*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/*********************************************************************
函数功能：ADC初始化函数
函数参数：无
返 回 值：无
*********************************************************************/
void Gray_Init(void)
{
	ADC_GPIO_Config();
	ADC_Config();
}
/*********************************************************************
函数功能：ADC值转化函数
函数参数：转换前的值--ADC检测到的值
返 回 值：转换过后的电压值
*********************************************************************/
u16 Gray_GetVolt(u16 advalue)
{
	return (u16)(advalue * 330 / 4096); //求的结果扩大了100倍，方便下面求出小数
}







