#include "main.h"
#include "main.h"
#include "menu_system.h"
#include "delay.h"
#include "Key.h"
#include "Timer.h"
#include "Serial.h"
#include "Motor.h"
#include "Encoder.h"
#include "OLED.h"     

// 变量定义
Key_TypeDef key = KEY_NONE;
uint16_t CNT = 0;
float Target = 0.0f, Actual = 0.0f, Out = 0.0f;
float Kp = 0.2f, Ki = 0.2f, Kd = 0.0f;
float Error0 = 0.0f, Error1 = 0.0f, Error2 = 0.0f;

int main(void)
{
    // 系统初始化
    Menu_Init();
    Key_Init();
    Timer_Init(); 
    Serial_Init();
    Motor_Init();
    Encoder_Init();  // 初始化编码器
    
    while (1) {
		}
}

void TIM2_IRQHandler(void)
{
    static uint16_t Count;
    
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Key_Tick();
        
        Count++;
        if (Count >= 40)
        {
            Count = 0;
            
            Actual = Encoder_Get();
            
            Error2 = Error1;
            Error1 = Error0;
            Error0 = Target - Actual;
            
            Out += Kp * (Error0 - Error1) + Ki * Error0
                  + Kd * (Error0 - 2 * Error1 + Error2);
            
            // 输出限幅
            if (Out > 100) { Out = 100; }
            if (Out < -100) { Out = -100; }
            
            // 执行控制
            Motor1_SetPWM((int32_t)Out);
            Motor2_SetPWM((int32_t)Out);
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
