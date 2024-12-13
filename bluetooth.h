#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f10x.h"     
#include "stdio.h"

void Usart2_Init(int BaudRate);
void Usart2_Send_Byte(int ch);
void Usart2_Send_Str(char * ch);
unsigned char Usart2_Recv_Byte(void);
uint8_t Usart2_WaitReasFinish(void);
void Usart2_reg(void (*pfunc)(void));

#endif 
