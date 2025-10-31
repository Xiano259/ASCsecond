#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "RP.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"

float Target, Actual, Out;
float Kp = 0.2, Ki = 0.2, Kd = 0;
float Error0, Error1, Errorint;

/*电机测试*/
/*下载此段程序后，按下K1，电机速度增加，按下K2，电机速度减小，按下K3，电机停止*/
uint8_t KeyNum;
int16_t PWM;
int16_t speed;
int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Key_Init();			//非阻塞式按键初始化
	Motor_Init();		//电机初始化
	Timer_Init();		//定时器初始化，1ms定时中断一次
	Encoder_Init();	
	Serial_Init();	
	
	OLED_Printf(0, 0, OLED_8X16, "Speed Control");
	OLED_Update();
	while (1)
	{
		KeyNum = Key_GetNum();		//获取键码
  	if (KeyNum == 1)			//如果K1按下
		{
			Target += 10;				//PWM变量加10
			
		}
		if (KeyNum == 2)			//如果K2按下
		{
			Target -= 10;				//PWM变量减10
		}
		if (KeyNum == 3)			//如果K3按下
		{
			Target = 0;				//PWM变量归0
		}
		
		
		OLED_Printf(0, 16, OLED_8X16, "Tar:%+04.0f", Target);		//OLED显示PWM变量值
		OLED_Printf(0, 32, OLED_8X16, "Act:%+04.0f", Actual);	
		OLED_Printf(0, 48, OLED_8X16, "Out:%+04.0f", Out);		
	  OLED_Update();		//OLED更新
		
		Serial_Printf("%f, %f, %f\r\n", Target, Actual, Out);
	}
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
	/*定时中断函数1ms自动执行一次*/
		static uint16_t Count;
		Key_Tick();		//调用按键模块的Tick函数，用于驱动按键模块工作
		
		Count++;
		if (Count >= 40)
		{
			 Count = 0;
			speed = Encoder_Get();
			Actual = speed;
			Error1 = Error0;
			Error0 = Target - Actual;
			Errorint += Error0;
			
			Out = Kp * Error0 + Ki * Errorint + Kd * (Error0 - Error1);
			if (Out > 100) {Out = 100;}
			if (Out < -100) {Out = -100;}
			
			Motor_SetPWM(Out);
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


