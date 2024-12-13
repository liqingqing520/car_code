#include "pid.h"
#include "usart.h"
#include "encoder.h"
#include "motor.h"


//����ṹ�����ͱ���
tPid pidMotor1Speed;
tPid pidMotor2Speed;

float Motor1Speed = 0.00; //ת��
float Motor2Speed = 0.00;

int Encoder_Average = 0;  //������,��¼����������(��ʱ��2 )   /  ����������(��ʱ��3 )
short Encoder_Left=0, Encoder_Right=0;

char PID_flag = 0;


//���ṹ�����ͱ�������ֵ
void PID_init(void)
{
    pidMotor1Speed.actual_val=0;
    pidMotor1Speed.target_val=0;
    pidMotor1Speed.err=0;
    pidMotor1Speed.err_last=0;
    pidMotor1Speed.err_sum=0;
    pidMotor1Speed.Kp=5;
    pidMotor1Speed.Ki=1;
    pidMotor1Speed.Kd=0;

    pidMotor2Speed.actual_val=0;
    pidMotor2Speed.target_val=0;
    pidMotor2Speed.err=0;
    pidMotor2Speed.err_last=0;
    pidMotor2Speed.err_sum=0;
    pidMotor2Speed.Kp=5;
    pidMotor2Speed.Ki=1;
    pidMotor2Speed.Kd=0;
}



////����p���ڿ��ƺ���
//int P_realize(tPid * pid,int actual_val)
//{
////	pid->actual_val = actual_val;//������ʵֵ
////	pid->err = pid->target_val - pid->actual_val;//��ǰ���=Ŀ��ֵ-��ʵֵ
////	//�������Ƶ���   ���=Kp*��ǰ���
////	pid->actual_val = pid->Kp*pid->err;
//	return pid->actual_val;
//}


////����P ����I ���ƺ���
//int PI_realize(tPid * pid,int actual_val)
//{
////	pid->actual_val = actual_val;//������ʵֵ
////	pid->err = pid->target_val - pid->actual_val;//��ǰ���=Ŀ��ֵ-��ʵֵ
////	pid->err_sum += pid->err;//����ۼ�ֵ = ��ǰ����ۼƺ�
////	//ʹ��PI���� ���=Kp*��ǰ���+Ki*����ۼ�ֵ
////	pid->actual_val = pid->Kp*pid->err + pid->Ki*pid->err_sum;
////
//	return pid->actual_val;
//}


// PID���ƺ���
int PID_realize(char flag)
{
    int pwm = 0;

    tPid * pid = NULL;

    switch(flag)
    {
    case 'L':
        pid = &pidMotor1Speed;
        pid->actual_val = Motor1Speed;
        break;//������ʵֵ
    case 'R':
        pid = &pidMotor2Speed;
        pid->actual_val = Motor2Speed;
        break;
    default:
        break;
    }

    pid->err = pid->target_val - pid->actual_val;////��ǰ���=Ŀ��ֵ-��ʵֵ
    pid->err_sum += pid->err;//����ۼ�ֵ = ��ǰ����ۼƺ�
    //ʹ��PID���� ��� = Kp*��ǰ���  +  Ki*����ۼ�ֵ + Kd*(��ǰ���-�ϴ����)

    if(pid->err_sum > 100)  pid->err_sum = 0; //�����޷�
    if(pid->err_sum < -100)  pid->err_sum = 0; //�����޷�

    pwm = pid->Kp*pid->err + pid->Ki*pid->err_sum + pid->Kd*(pid->err - pid->err_last);

//	printf("%c  target_val: %f  actual_val: %f  err: %f  err_num: %f  pwm: %d\r\n\r\n", flag, pid->target_val, pid->actual_val, pid->err, pid->err_sum, pwm);

    //���浱ǰ����ȥ��
    pid->err_last = pid->err;

    return pwm;
}



/**************************************************************************
�������ܣ���ʱ��1�жϴ�������  2msʱ��
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM1_UP_IRQHandler(void)
{
    static int count = 0;
    int pwml = 0, pwmr = 0;


    if(TIM_GetITStatus(TIM1,TIM_IT_Update) != RESET) //����ж��Ƿ����
    {
        TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);  //����жϱ�־

        count++;

        if((count % 5) == 0)  //10ms �ٶȲ���
        {
            //===��ȡ��������ֵ����Ϊ�����������ת��180�ȵģ����Զ�����һ��ȡ������֤�������һ��

            Encoder_Left  = -Read_Encoder(2);
            Encoder_Right = Read_Encoder(3);
//			printf("Encoder_Left = %d  Encoder_Right = %d\r\n",Encoder_Left, Encoder_Right);

            Encoder_Average += ((Encoder_Right+Encoder_Left) / 2);

            Motor1Speed = ((float)Encoder_Left*10.0)/52.0;
            Motor2Speed = ((float)Encoder_Right*10.0)/52.0;

//			printf("Motor1Speed = %f  Motor2Speed = %f\r\n\r\n", Motor1Speed, Motor2Speed);
        }
        if((count % 10) == 0)  //20ms PID����
        {
            if( PID_flag == 1)
            {
                pwml = PID_realize('L');
                pwmr = PID_realize('R');
                Motor_Set(pwml, pwmr);
            }

            count = 0;
        }
    }
}

/**************************************************************************
�������ܣ��ı䵱ǰĿ���ٶ�
��ڲ�����Motor_l_SetSpeed ����Ŀ���ٶ�     Motor_r_SetSpeed ����Ŀ���ٶ�
����  ֵ����
**************************************************************************/
void  Set_Target_Speed(float  Motor_l_SetSpeed, float Motor_r_SetSpeed)
{
    //�ı���PID������Ŀ���ٶ�
    pidMotor1Speed.target_val = Motor_l_SetSpeed;
    pidMotor2Speed.target_val = Motor_r_SetSpeed;
}


/**************************************************************************
�������ܣ�С����ʻ�����ѯ����
��ڲ�������
�� �� ֵ��С����ʻ�ľ���, cm
����˵��:
**************************************************************************/
int Car_Distance_Inquiry(void)
{
    int distance = 255.0*((float)Encoder_Average / 1569.0);
    Encoder_Average = 0;

    return distance;
}
