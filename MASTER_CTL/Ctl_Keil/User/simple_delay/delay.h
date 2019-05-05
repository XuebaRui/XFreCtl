#ifndef __delay_h_
#define __delay_h_

#include "stm32f10x.h"
void delay_us( u32 us);
void delay_ms( u8 ms);
void Delay_TIM4_Init(void);
void delay_s(u8 s);
void upload_tim_Init(void);
#endif

