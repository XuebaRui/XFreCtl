#ifndef _ctl_h__
#define _ctl_h__

#include "stm32f10x.h"

#define Ref_Set(a)   if(a) \
											GPIO_ResetBits(GPIOB,GPIO_Pin_6); \
										else \
											GPIO_SetBits(GPIOB,GPIO_Pin_6); 
										
#define Fre_PW_Set(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_9); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_9); 
#define LED_RED(a) if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_12); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_12); 
										
#define LED_GREEN(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_15); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_15); 										
										
unsigned char Check_LD(void);										
void Ctl_IO_Init(void);
void SoftReset(void);
double Get_Temprate(void);
u16 T_Get_Adc_Average(u8 ch,u8 times);
u16 T_Get_Temp(void);
u16 T_Get_Adc(u8 ch);
void T_Adc_Init(void)  ;
#endif 
										