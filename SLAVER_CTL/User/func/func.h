#ifndef func_H_
#define func_H_

#include "stm32f10x.h"

/***********************IO************************/
#define M1dB(a) if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_15);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_15)
#define M2dB(a) 	if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_14);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_14) 
#define M4dB(a) 		if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_13);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_13) 
#define M8dB(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_9);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_9) 
#define M16dB(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_8);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_8) 

#define A0_5dB(a) if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_12);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define A1dB(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_13);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_13) 
#define A2dB(a) 		if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_14);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_14) 
#define A4dB(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_15);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_15) 
#define A8dB(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_8);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_8) 
#define A16dB(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_9);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_9) 
#define A32dB(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_10);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_10) 
#define U1_A(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_2);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_2) 
#define U1_B(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_3);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_3) 
#define U2_A(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_12);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_12) 
#define U2_B(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_11);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_11) 								
void ATT_Ctl(u8 stc);															
void CG_Ctl(u8 stc);
void Fre_Ctl(u32 fre);
void IO_Init(void);
void BW_Ctl(u8 bw);
#endif 
				