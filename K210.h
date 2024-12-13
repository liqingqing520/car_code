#ifndef __K210_H
#define __K210_H

#include "stm32f10x.h"     
#include "stdio.h"

//????
void Usart3_Init(int BaudRate);
void Usart3_Send_Byte(int ch);
void Usart3_Send_Str(char * ch);
unsigned char Usart3_Recv_Byte(void);
uint8_t Usart3_WaitReasFinish(void);
void AI_reg(void (*pfunc)(void));

#endif 
