#ifndef __LED_H
#define __LED_H

#include <stdint.h>

// ????
void LED_Init(void);
void LED_UpdateSpeed(uint8_t speed_level);
void LED_UpdateDirection(uint8_t direction);
void LED_Run(void);

// ??LED?????
uint8_t LED_GetDirection(void);
uint8_t LED_GetSpeedLevel(void);
float LED_GetSpeedLevelForDisplay(void);  // ??????

#endif
