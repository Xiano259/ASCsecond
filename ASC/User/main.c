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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>                
#include <string.h>  

float Target, Actual, Out1, Out2, Act;
float Kp = 0.5f, Ki = 0.1f, Kd = 0.0f;
float Error0, Error1, Error2, E0, E1, E2;

/*电机测试*/
/*下载此段程序后，按下K1，电机速度增加，按下K2，电机速度减小，按下K3，电机停止*/
uint8_t KeyNum;
int16_t PWM;
float speed =  0.0f;  //-180 ~ 180

float Extract_Speed_Value(const char* packet)
{
    char* percent_ptr = strchr(packet, '%');  // 查找%位置
    if (percent_ptr != NULL)
    {
        // %后面的第一个字符开始
        char* number_start = percent_ptr + 1;
        
        // 跳过可能的前导空格
        while (*number_start == ' ') {
            number_start++;
        }
        
        // 提取数字
        if (*number_start >= '0' && *number_start <= '9' || *number_start == '-')
        {
            return atof(number_start);
        }
    }
    
    return 0.0f;
}

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Key_Init();			//非阻塞式按键初始化
	Motor_Init();		//电机初始化
	Timer_Init();		//定时器初始化，1ms定时中断一次
	Encoder2_Init();	
	Serial_Init();
  Encoder1_Init();	

	OLED_Update();
	while (1)
	{
		KeyNum = Key_GetNum();		//获取键码
  	if (KeyNum == 1)			//如果K1按下
		{
			Target += 10;				//PWM变量加10
			
		}
    if (Serial_RxFlag == 1)		//如果接收到数据包
		{
		   speed = Extract_Speed_Value((const char*)Serial_RxPacket[100]) * 3 * 20;
			  Serial_RxFlag = 0;
		}
		OLED_Printf(0, 0, OLED_8X16, "Motor2:%+05.0f", Encoder2_Get());	
		OLED_Printf(0, 16, OLED_8X16, "Tar:%+05.0f", Target);	
		OLED_Printf(0, 32, OLED_8X16, "Act:%+05.0f", Actual);	
		OLED_Printf(0, 48, OLED_8X16, "Out:%+05.0f", Out1);	
    
    // 提取速度值
    speed = Extract_Speed_Value((const char*)Serial_RxPacket);		
	  OLED_Update();
		
		Serial_Printf("%f, %f, %f\r\n", Target, Actual, Act);
	}
}

void TIM1_UP_IRQHandler(void)
{
	/*定义静态变量（默认初值为0，函数退出后保留值和存储空间）*/
	static uint16_t Count;		//用于计次分频
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		/*每隔1ms，程序执行到这里一次*/
		
		Key_Tick();				//调用按键的Tick函数
		
		/*计次分频*/
		Count ++;				//计次自增
		if (Count >= 10)		//如果计次10次，则if成立，即if每隔10ms进一次
		{
			Count = 0;			//计次清零，便于下次计次
			
			/*获取实际位置值*/
			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
			/*计次值增量进行累加，即可得到计次值本身（即实际位置）*/
			/*这里先获取增量，再进行累加，实际上是绕了个弯子*/
			/*如果只需要得到编码器的位置，而不需要得到速度*/
			/*则Encode_Get函数内部的代码可以修改为return TIM_GetCounter(TIM3);直接返回CNT计数器的值*/
			/*修改后，此处代码改为Actual = Encoder_Get();直接得到位置，就不再需要累加了，这样更直接*/
			Actual += Encoder1_Get() * 3;
			Target += speed;
			/*获取本次误差、上次误差和上上次误差*/
			Error2 = Error1;			//获取上上次误差
			Error1 = Error0;			//获取上次误差
			Error0 = Target - Actual;	//获取本次误差，目标值减实际值，即为误差值
			Act += Encoder2_Get() * 3;
			
			E2 = E1;			//获取上上次误差
			E1 = E0;			//获取上次误差
			E0 = Actual - Act;
			/*PID计算*/
			/*使用增量式PID公式，计算得到输出值*/
			Out1 += Kp * (Error0 - Error1) + Ki * Error0 + Kd * (Error0 - 2 * Error1 + Error2);
			Out2 += Kp * (E0 - E1) + Ki * E0 + Kd * (E0 - 2 * E1 + E2);
			if (E0 == 0.0f)
			{
				Out2 = 0.0f;
			}
			/*输出限幅*/
			if (Out1 > 100) {Out1 = 100;}		//限制输出值最大为100
			if (Out1 < -100) {Out1 = -100;}	//限制输出值最小为100
			
			if (Out2 > 100) {Out2 = 100;}		//限制输出值最大为100
			if (Out2 < -100) {Out2 = -100;}	//限制输出值最小为100
			/*执行控制*/
			/*输出值给到电机PWM*/
			/*因为此函数的输入范围是-100~100，所以上面输出限幅，需要给Out值限定在-100~100*/
			
			Motor1_SetPWM(Out1);
			Motor2_SetPWM(Out2);
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


