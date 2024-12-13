#include "SysTick.h"


static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������


//void delay_ms(int ms)
//{
//	int i = 0;
//	while(ms--)
//		for(i=0; i<8050; i++);
//}



/**************************************************************************
�������ܣ�ʱ������  
��ڲ�������
�� �� ֵ����
����˵��: ȷ��Ϊ72MHz
**************************************************************************/
void  Clock_Init(void) //����ʵʱʱ��
{
	RCC_DeInit();  //Ĭ��ʱ�ӳ�ʼ��
	RCC_HSEConfig(RCC_HSE_ON); //�����ⲿ���پ���
	while(SUCCESS != RCC_WaitForHSEStartUp() ); //�ȴ�����
	
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //����PLLʱ��Դ
	RCC_PLLCmd(ENABLE);  //ʹ�� Enables or disables the PLL.
	while(RESET ==  RCC_GetFlagStatus(RCC_FLAG_PLLRDY)); //����־λ������� PLL clock ready
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //����ϵͳʱ�� PLL selected as system clock
	RCC_HCLKConfig(RCC_SYSCLK_Div1); //Configures the AHB clock (HCLK).AHB clock = SYSCLK
	RCC_PCLK1Config(RCC_HCLK_Div2);//Configures the Low Speed APB clock (PCLK1).
	RCC_PCLK2Config(RCC_HCLK_Div1); //Configures the High Speed APB clock (PCLK2).	
}


/*******************************************************************************
* �� �� ��         : SysTick_Init
* ��������		   : SysTick��ʼ����SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
* ��    ��         : SYSCLK:ϵͳʱ��Ƶ��
* ��    ��         : ��
*******************************************************************************/
void SysTick_Init(u8 SYSCLK)
{
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK/8;						//SYSCLK��8��Ƶ ����1us����ļ�������
	fac_ms=(u16)fac_us*1000;				//ÿ��ms��Ҫ��systickʱ����   
}								    

/*******************************************************************************
* �� �� ��         : delay_us
* ��������		   : us��ʱ��
* ��    ��         : nus��Ҫ��ʱ��us��
					ע��:nus��ֵ,��Ҫ����798915us(���ֵ��2^24/fac_us@fac_us=21)
* ��    ��         : ��
*******************************************************************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=0x01 ; //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	
	//�ȴ�ʱ�䵽��   SysTick->CTRL��bit0Ϊ1����bit16λΪ1��������
	SysTick->CTRL&=~0x01; //�رռ�����
	SysTick->VAL =0X00;       				//��ռ����� 
}

/*******************************************************************************
* �� �� ��         : delay_nms
* ��������		   : ms��ʱ��
* ��    ��         : nms��Ҫ��ʱ��ms��
					ע��:nms��ֵ,SysTick->LOADΪ24λ�Ĵ�����
-------��			�������ֵ nms<=0xffffff/9/1000 
					��72M������,nms<=1864 
					��168M������,nms<=798ms 
* ��    ��         : ��
*******************************************************************************/
void delay_nms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL|=0x01 ; //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~0x01; //�رռ�����
	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
}

/*******************************************************************************
* �� �� ��         : delay_ms
* ��������		   : ms��ʱ��
* ��    ��         : nms��Ҫ��ʱ��ms�� 0~65535
* ��    ��         : ��
*******************************************************************************/
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;		//������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,
							//���糬Ƶ��248M��ʱ��,delay_nms���ֻ����ʱ541ms������
	u16 remain=nms%540;
	while(repeat)
	{
		delay_nms(540);
		repeat--;
	}
	if(remain)delay_nms(remain);
} 


