#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"

extern Key_TypeDef key;
#define KEY_TIME_LONG			1000
#define KEY_TIME_REPEAT			100

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

Key_TypeDef Key_GetNum(void)
{
	Key_TypeDef Temp;
	if (key)
	{
		Temp = key;
		key = KEY_NONE;
		return Temp;
	}
	  return KEY_NONE;
}

Key_TypeDef Key_GetState(void)
{
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		return KEY;
	}
  else{
		return KEY_NONE;
  }
}
 
void Key_Tick(void)
{
    static uint16_t Count;
    static Key_TypeDef CurrState, PrevState;
	
    Count++;
    if (Count >= 1000) 
    {
        Count = 0;
        
        PrevState = CurrState;
        CurrState = Key_GetState();
        
        if (CurrState != KEY_NONE && PrevState == KEY_NONE)
        {
            key = CurrState;
        }
    }
	}

