#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"   


//声明一个结构体类型
typedef struct 
{
	float target_val;//目标值
	float actual_val;//实际值
	float err;//当前偏差
	float err_last;//上次偏差
	float err_sum;//误差累计值
	float Kp,Ki,Kd;//比例，积分，微分系数
	
}tPid;



void PID_init(void);
//int P_realize(tPid * pid,int actual_val);
//int PI_realize(tPid * pid,int actual_val);
//int PID_realize(tPid * pid,int actual_val);
int PID_realize( char flag);

void  Set_Target_Speed(float  Motor_l_SetSpeed, float Motor_r_SetSpeed);

int Car_Distance_Inquiry(void);

#endif 
