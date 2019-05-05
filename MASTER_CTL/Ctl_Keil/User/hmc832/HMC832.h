#ifndef __HMC832_h_
#define __HMC832_h_

#include "stm32f10x.h"
#include "delay.h"

/*********************¶Ë¿Ú¶¨Òå********************/
//HMC832A
#define SDOA(a) if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_0);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define SENA(a) 	if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_3);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_3) 
#define SCLKA(a) 		if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_2);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_2) 
#define SDIA(a) 	if (a)\
									GPIO_SetBits  (GPIOC,GPIO_Pin_1);\
								else		\
									GPIO_ResetBits(GPIOC,GPIO_Pin_1) 
//HMC832B
#define SDOB(a) if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_1);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define SENB(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_3);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_3) 
#define SCLKB(a) 		if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_2);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_2) 
#define SDIB(a) 	if (a)\
									GPIO_SetBits  (GPIOA,GPIO_Pin_0);\
								else		\
									GPIO_ResetBits(GPIOA,GPIO_Pin_0) 
/*************************************************/
u8 HMC832A_FreSet(double fre);
void HMC832A_Init(void);
u8 HMC832B_FreSet(double fre);
void HMC832B_Init(void);
#endif
