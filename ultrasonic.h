#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H


#include "stm32f10x.h"



#define	Trig_Send_Open GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define	Trig_Send_Close GPIO_ResetBits(GPIOA, GPIO_Pin_11) 
#define Echo_Receive GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)

void UT_Init(void);
u32 UT_Get_Length(void);

#endif
