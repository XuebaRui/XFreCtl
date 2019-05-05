#ifndef __HMC704_h_
#define __HMC704_h_

#include "stm32f10x.h"
#include "delay.h"
/***********************IO************************/
#define SCLK(a) if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_11);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define SDI(a) 		if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_8);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_8) 
#define SEN_1(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_8);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_8) 
#define SEN_2(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_7);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_7) 
/*************************************************/

void HMC704_OpenModeWrite(unsigned long int data ,unsigned char channel);
void HMC704_Write(unsigned long int data ,unsigned char channel)	;
void HMC704_Init(void);
void HMC704_GPIO_init(void);
void Fre_Set(u16 fre_h, u32 fre_l,u8 sel);
#endif
