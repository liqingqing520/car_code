#include "stm32f10x.h"                  // Device header
#include "led.h"		//led
#include "SysTick.h"	//�δ�ʱ��
#include "tracking.h"	//ѭ��ģ��
#include "motor.h" 		//���
#include "encoder.h"	//������
#include "ultrasonic.h"	//������
#include "usart.h"		//����1
#include "pid.h"

//#include "sys.h"
//#include "string.h"
//#include "mpu6050.h"

//#include "AI_Hal.h"		
//#include "Ser_Hal.h"
#include "console.h"		//�������
#include "communication.h"	//ͨ��
#include "drive.h"       	//���˼�ʻ �ֶ���ʻ
#include "data_logger.h" 	//Ѳ��



extern int mode_flag; //communication.c

void my_delay_ms(int ms)
{
	int i;
	while(ms--)
		for(i=0;i<8050;i++);
}

int main()
{
	led_c13_init();   //led_PC13��ʼ��();
	led_c13_on();
	
	
	Clock_Init();     //ʱ�ӳ�ʼ��() 72MHz;
	SysTick_Init(72); //�δ�ʱ����ʼ��(����Ƶ��);
	
	
	Tracking_Init();  //ѭ��ģ���ʼ��();
	
	Motor_Init();     //�����ʼ��();
	Encoder_Init();   //��������ʼ��();
	
	
	UT_Init();        //��������ʼ��
	PID_init();
	
	communication_init(115200); //ͨ�ų�ʼ��  ai_hal_init  ai_hal_reg   ser_hal_init  ser_hal_reg

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
			case FLAG_TRACK:  Data_Logger(); break;  //Ѳ��
			case FLAG_DRIVE:  Auto_Drive(); break;  //���˼�ʻ
			default: break;
		}
	}
}






