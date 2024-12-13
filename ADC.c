#include "ADC.h"

//uint16_t AD_Value[3];//���������������ADC����ͨ��ת��������
vu16 AD_Value[50][3]; //���ADCֵ���ڴ�ռ䣬��Ҫ��ʹ�õ�λ��ͨ��extern����

//��ɫһ������
//��ɫ�˰�����
//��ɫ3000����

/* */
/*********************************************************************
�������ܣ�ADC���ų�ʼ��
������������
�� �� ֵ����
˵	  ��������ADC1��ͨ��4 ͨ��5 ͨ��9 ����ΪPA^4 PA^5 PB^1 ģʽ������ģ������
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
�������ܣ�ADC��ʼ������Ϊ��Ҫʹ�ö�ͨ��������Ҫʹ��DMA
������������
�� �� ֵ����
*********************************************************************/
void ADC_Config(void)
{
	ADC_InitTypeDef  ADC_InitStruct;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	
	
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//��������ת�� 
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//����ģʽ������ֻ��ADC1
	ADC_InitStruct.ADC_NbrOfChannel = Channel_Num; //3��ͨ��ת��
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ������ɨ����ͨ����
	ADC_Init(ADC1,&ADC_InitStruct);
	
	

	/*ADC_RegularChannelConfig��ADCx,ͨ��y������z,����ʱ�䣩��ͨ����0~17�����У�1~18*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 3, ADC_SampleTime_55Cycles5);
	
	DMA_DeInit(DMA1_Channel1);
	//DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC1�����ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16λ����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;//�洢����ַ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢��������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢����ַ������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//������Ϊ����Դ
	DMA_InitStructure.DMA_BufferSize = Channel_Num;//��������һ��ͨ���ͼ�һ�Σ�����ʹ������ͨ��
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ģʽ
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//�Ǵ洢�����洢��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//DMAͨ����Ӧ�����ȼ�����
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
    /*ʹ��*/
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
    /*ADC����У׼*/ 
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
    /*ADC�������*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/*********************************************************************
�������ܣ�ADC��ʼ������
������������
�� �� ֵ����
*********************************************************************/
void Gray_Init(void)
{
	ADC_GPIO_Config();
	ADC_Config();
}
/*********************************************************************
�������ܣ�ADCֵת������
����������ת��ǰ��ֵ--ADC��⵽��ֵ
�� �� ֵ��ת������ĵ�ѹֵ
*********************************************************************/
u16 Gray_GetVolt(u16 advalue)
{
	return (u16)(advalue * 330 / 4096); //��Ľ��������100���������������С��
}







