#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"     
#include <stdio.h>

//????
void Usart1_Init(int BaudRate);
void Usart1_Send_Byte(int ch);
void Usart1_Send_Str(char * ch);
unsigned char Usart1_Recv_Byte(void);
uint8_t Usart1_WaitReasFinish(void);


#endif 
