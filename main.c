#include "stm32f10x.h"                  // Device header
#include "led.h"		//led
#include "SysTick.h"	//滴答定时器
#include "tracking.h"	//循迹模块
#include "motor.h" 		//电机
#include "encoder.h"	//编码器
#include "ultrasonic.h"	//超声波
#include "usart.h"		//串口1
#include "pid.h"

//#include "sys.h"
//#include "string.h"
//#include "mpu6050.h"

//#include "AI_Hal.h"		
//#include "Ser_Hal.h"
#include "console.h"		//电机控制
#include "communication.h"	//通信
#include "drive.h"       	//无人驾驶 手动驾驶
#include "data_logger.h" 	//巡检



extern int mode_flag; //communication.c

void my_delay_ms(int ms)
{
	int i;
	while(ms--)
		for(i=0;i<8050;i++);
}

int main()
{
	led_c13_init();   //led_PC13初始化();
	led_c13_on();
	
	
	Clock_Init();     //时钟初始化() 72MHz;
	SysTick_Init(72); //滴答定时器初始化(晶振频率);
	
	
	Tracking_Init();  //循迹模块初始化();
	
	Motor_Init();     //电机初始化();
	Encoder_Init();   //编码器初始化();
	
	
	UT_Init();        //超声波初始化
	PID_init();
	
	communication_init(115200); //通信初始化  ai_hal_init  ai_hal_reg   ser_hal_init  ser_hal_reg

	Usart1_Init(115200);
	printf("usart init ok\r\n");

	led_c13_off();
	
	
//	Motor_Run_A();
//	Motor_Set(0,60);
//	Car_Start_Pwm();
//	Car_Start();

	while(1)
	{	
//		Car_Start();
//		delay_ms(500);
		switch(mode_flag)
		{
			case FLAG_TRACK:  Data_Logger(); break;  //巡检
			case FLAG_DRIVE:  Auto_Drive(); break;  //无人驾驶
			default: break;
		}
	}
}






