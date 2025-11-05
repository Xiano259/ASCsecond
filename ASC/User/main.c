#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>                
#include <string.h>  

double Target, Out1, Out2;
int32_t Actual = 0; 
int32_t Act = 0;
double Kp = 0.4f, Ki = 0.1f, Kd = 0.2f;
double Error0, Error1, Error2, E0, E1, E2;

static uint16_t lastEnc1 = 0;
static uint16_t lastEnc2 = 0;

/*电机测试*/
/*下载此段程序后，按下K1，电机速度增加，按下K2，电机速度减小，按下K3，电机停止*/
uint8_t KeyNum;
int16_t PWM;
uint8_t mode = 0;
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
        if ((*number_start >= '0' && *number_start <= '9' ) || *number_start == '-')
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

	    // 初始化 last 值，避免第一次 delta 大跳
  lastEnc1 = Encoder1_GetRaw();
  lastEnc2 = Encoder2_GetRaw();
	OLED_Update();
	while (1)
	{
		KeyNum = Key_GetNum();		//获取键码
  	if (KeyNum == 1)			//如果K1按下
		{
			mode = !mode;
			TIM_SetCounter(TIM3, 0);
			TIM_SetCounter(TIM4, 0);
			
		}
    if (Serial_RxFlag == 1)		//如果接收到数据包
		{
		   speed = Extract_Speed_Value((const char*)Serial_RxPacket);
			  Serial_RxFlag = 0;
		}
		OLED_Printf(0, 0, OLED_8X16, "mode:%d", mode);	
		OLED_Printf(0, 16, OLED_8X16, "Tar:%+05.0f", Target);	
		OLED_Printf(0, 32, OLED_8X16, "Act:%+05.0f", Actual);	
		OLED_Printf(0, 48, OLED_8X16, "speed:%+05.0f", speed);	
    
    // 提取速度值
    speed = Extract_Speed_Value((const char*)Serial_RxPacket);		
	  OLED_Update();
		
		Serial_Printf("%d\r\n", Encoder2_GetRaw());
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
			
			uint16_t now1 = Encoder1_GetRaw();
      int32_t delta1 = CalcDelta16(lastEnc1, now1);
      lastEnc1 = now1;
      Actual += delta1 * 3;
      uint16_t now2 = Encoder2_GetRaw();
      int32_t delta2 = CalcDelta16(lastEnc2, now2);
      lastEnc2 = now2;
      Act += delta2 * 3;
			Target += speed;
			if (mode == 1) 
			{
				Target = Actual;
			}

			/*获取本次误差、上次误差和上上次误差*/
			Error2 = Error1;			//获取上上次误差
			Error1 = Error0;			//获取上次误差
			Error0 = Target - Actual;	//获取本次误差，目标值减实际值，即为误差值
			
			E2 = E1;			//获取上上次误差
			E1 = E0;			//获取上次误差
			E0 = Actual - Act;
			/*PID计算*/
			/*使用增量式PID公式，计算得到输出值*/
			Out1 += Kp * (Error0 - Error1) + Ki * Error0 + Kd * (Error0 - 2 * Error1 + Error2);
			Out2 += Kp * (E0 - E1) + Ki * E0 + Kd * (E0 - 2 * E1 + E2);
			if ( E0 == 0.0f)
			{
				Out2 = 0;
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


