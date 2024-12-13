#ifndef __ENCODER_h
#define __ENCODER_h

#include "stm32f10x.h"  

void Encoder_Init(void);
int  Read_Encoder(u8 TIMX);
void TIM1_Switch(int flag);

#endif
