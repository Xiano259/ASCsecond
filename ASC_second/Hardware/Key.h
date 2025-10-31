#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "delay.h"

typedef enum {
    KEY = 1,
    KEY_NONE = 0,	
} Key_TypeDef;

void Key_Init(void);
Key_TypeDef Key_GetState(void);
void Key_Tick(void);

#endif
