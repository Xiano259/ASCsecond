#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"

// 系统时钟定义
#define SYSTEM_CLOCK_FREQ 72000000

// 函数声明
void SystemClock_Config(void);
void Error_Handler(void);

#endif
