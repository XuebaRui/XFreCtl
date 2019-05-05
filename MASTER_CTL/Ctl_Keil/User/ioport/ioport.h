#ifndef __ioport_h_
#define __ioport_h_

#include "stm32f10x.h"
#include "mytypedef.h"

/*********************¶Ë¿Ú¶¨Òå********************/
//AGCA
#define AGC1V1_1(a) if (a)\
											GPIO_SetBits  (GPIOC,GPIO_Pin_4);\
										else		\
											GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define AGC1V1_2(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_0);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define AGC1V1_3(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_7);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_7)
#define AGC1V1_4(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_8);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_8)
#define AGC1V1_5(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_9);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_9)
#define AGC1V2_1(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_10);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_10)
#define AGC1V2_2(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_11);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_11)
#define AGC1V2_3(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_12);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_12)
#define AGC1V2_4(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_13);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_13)
#define AGC1V2_5(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_14);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_14)
#define AGC1V3_1(a) if (a)\
											GPIO_SetBits  (GPIOE,GPIO_Pin_15);\
										else		\
											GPIO_ResetBits(GPIOE,GPIO_Pin_15)
#define AGC1V3_2(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_10);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define AGC1V3_3(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_11);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_11)
#define AGC1V3_4(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_12);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define AGC1V3_5(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_13);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_13)
//AGCB										
#define AGC2V1_1(a) if (a)\
											GPIO_SetBits  (GPIOA,GPIO_Pin_7);\
										else		\
											GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define AGC2V1_2(a) if (a)\
											GPIO_SetBits  (GPIOC,GPIO_Pin_5);\
										else		\
											GPIO_ResetBits(GPIOC,GPIO_Pin_5)
#define AGC2V1_3(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_1);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define AGC2V1_4(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_14);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define AGC2V1_5(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_8);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_8)
#define AGC2V2_1(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_10);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_10)
#define AGC2V2_2(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_12);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_12)
#define AGC2V2_3(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_13);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_13)
#define AGC2V2_4(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_14);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_14)
#define AGC2V2_5(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_15);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_15)
#define AGC2V3_1(a) if (a)\
											GPIO_SetBits  (GPIOC,GPIO_Pin_6);\
										else		\
											GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define AGC2V3_2(a) if (a)\
											GPIO_SetBits  (GPIOB,GPIO_Pin_15);\
										else		\
											GPIO_ResetBits(GPIOB,GPIO_Pin_15)
#define AGC2V3_3(a) if (a)\
											GPIO_SetBits  (GPIOC,GPIO_Pin_7);\
										else		\
											GPIO_ResetBits(GPIOC,GPIO_Pin_7)
#define AGC2V3_4(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_9);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_9)
#define AGC2V3_5(a) if (a)\
											GPIO_SetBits  (GPIOD,GPIO_Pin_11);\
										else		\
											GPIO_ResetBits(GPIOD,GPIO_Pin_11)										
										
							
/*************************************************/
void IOPort_Init(void);					
#endif
