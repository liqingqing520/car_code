#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"   


//����һ���ṹ������
typedef struct 
{
	float target_val;//Ŀ��ֵ
	float actual_val;//ʵ��ֵ
	float err;//��ǰƫ��
	float err_last;//�ϴ�ƫ��
	float err_sum;//����ۼ�ֵ
	float Kp,Ki,Kd;//���������֣�΢��ϵ��
	
}tPid;



void PID_init(void);
//int P_realize(tPid * pid,int actual_val);
//int PI_realize(tPid * pid,int actual_val);
//int PID_realize(tPid * pid,int actual_val);
int PID_realize( char flag);

void  Set_Target_Speed(float  Motor_l_SetSpeed, float Motor_r_SetSpeed);

int Car_Distance_Inquiry(void);

#endif 
