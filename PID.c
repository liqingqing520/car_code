#include "pid.h"
#include "usart.h"
#include "encoder.h"
#include "motor.h"


//定义结构体类型变量
tPid pidMotor1Speed;
tPid pidMotor2Speed;

float Motor1Speed = 0.00; //转速
float Motor2Speed = 0.00;

int Encoder_Average = 0;  //编码器,记录左轮脉冲数(定时器2 )   /  右轮脉冲数(定时器3 )
short Encoder_Left=0, Encoder_Right=0;

char PID_flag = 0;


//给结构体类型变量赋初值
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



////比例p调节控制函数
//int P_realize(tPid * pid,int actual_val)
//{
////	pid->actual_val = actual_val;//传递真实值
////	pid->err = pid->target_val - pid->actual_val;//当前误差=目标值-真实值
////	//比例控制调节   输出=Kp*当前误差
////	pid->actual_val = pid->Kp*pid->err;
//	return pid->actual_val;
//}


////比例P 积分I 控制函数
//int PI_realize(tPid * pid,int actual_val)
//{
////	pid->actual_val = actual_val;//传递真实值
////	pid->err = pid->target_val - pid->actual_val;//当前误差=目标值-真实值
////	pid->err_sum += pid->err;//误差累计值 = 当前误差累计和
////	//使用PI控制 输出=Kp*当前误差+Ki*误差累计值
////	pid->actual_val = pid->Kp*pid->err + pid->Ki*pid->err_sum;
////
//	return pid->actual_val;
//}


// PID控制函数
int PID_realize(char flag)
{
    int pwm = 0;

    tPid * pid = NULL;

    switch(flag)
    {
    case 'L':
        pid = &pidMotor1Speed;
        pid->actual_val = Motor1Speed;
        break;//传递真实值
    case 'R':
        pid = &pidMotor2Speed;
        pid->actual_val = Motor2Speed;
        break;
    default:
        break;
    }

    pid->err = pid->target_val - pid->actual_val;////当前误差=目标值-真实值
    pid->err_sum += pid->err;//误差累计值 = 当前误差累计和
    //使用PID控制 输出 = Kp*当前误差  +  Ki*误差累计值 + Kd*(当前误差-上次误差)

    if(pid->err_sum > 100)  pid->err_sum = 0; //积分限幅
    if(pid->err_sum < -100)  pid->err_sum = 0; //积分限幅

    pwm = pid->Kp*pid->err + pid->Ki*pid->err_sum + pid->Kd*(pid->err - pid->err_last);

//	printf("%c  target_val: %f  actual_val: %f  err: %f  err_num: %f  pwm: %d\r\n\r\n", flag, pid->target_val, pid->actual_val, pid->err, pid->err_sum, pwm);

    //保存当前误差（过去）
    pid->err_last = pid->err;

    return pwm;
}



/**************************************************************************
函数功能：定时器1中断处理函数  2ms时间
入口参数：无
返回  值：无
**************************************************************************/
void TIM1_UP_IRQHandler(void)
{
    static int count = 0;
    int pwml = 0, pwmr = 0;


    if(TIM_GetITStatus(TIM1,TIM_IT_Update) != RESET) //检查中断是否产生
    {
        TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);  //清除中断标志

        count++;

        if((count % 5) == 0)  //10ms 速度采样
        {
            //===读取编码器的值，因为两个电机的旋转了180度的，所以对其中一个取反，保证输出极性一致

            Encoder_Left  = -Read_Encoder(2);
            Encoder_Right = Read_Encoder(3);
//			printf("Encoder_Left = %d  Encoder_Right = %d\r\n",Encoder_Left, Encoder_Right);

            Encoder_Average += ((Encoder_Right+Encoder_Left) / 2);

            Motor1Speed = ((float)Encoder_Left*10.0)/52.0;
            Motor2Speed = ((float)Encoder_Right*10.0)/52.0;

//			printf("Motor1Speed = %f  Motor2Speed = %f\r\n\r\n", Motor1Speed, Motor2Speed);
        }
        if((count % 10) == 0)  //20ms PID调速
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
函数功能：改变当前目标速度
入口参数：Motor_l_SetSpeed 左轮目标速度     Motor_r_SetSpeed 右轮目标速度
返回  值：无
**************************************************************************/
void  Set_Target_Speed(float  Motor_l_SetSpeed, float Motor_r_SetSpeed)
{
    //改变电机PID参数的目标速度
    pidMotor1Speed.target_val = Motor_l_SetSpeed;
    pidMotor2Speed.target_val = Motor_r_SetSpeed;
}


/**************************************************************************
函数功能：小车行驶距离查询函数
入口参数：无
返 回 值：小车行驶的距离, cm
解释说明:
**************************************************************************/
int Car_Distance_Inquiry(void)
{
    int distance = 255.0*((float)Encoder_Average / 1569.0);
    Encoder_Average = 0;

    return distance;
}

