#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"     

void Motor_Init(void);

void Motor_Run_R(void);
void Motor_Rerun_R(void);
void Motor_Stop_R(void);

void Motor_Run_L(void);
void Motor_Rerun_L(void);
void Motor_Stop_L(void);

void Motor_Run_A(void);
void Motor_Return_A(void);
void Motor_Stop_A(void);

void Motor_Set (int motor1,int motor2);

#endif 
