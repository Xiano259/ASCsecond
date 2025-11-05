#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

void Encoder1_Init(void);
void Encoder2_Init(void);

uint16_t Encoder1_GetRaw(void);
uint16_t Encoder2_GetRaw(void);

int32_t CalcDelta16(uint16_t last, uint16_t now);

#endif
