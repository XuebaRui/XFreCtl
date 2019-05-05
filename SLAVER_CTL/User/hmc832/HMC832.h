#ifndef __HMC832_h_
#define __HMC832_h_

#include "stm32f10x.h"
#include "delay.h"
/***********************IO************************/
#define SDO(a) if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_0);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define SEN(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_11);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_11) 
#define SCLK(a) 		if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_10);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_10) 
#define SDI(a) 	if (a)\
									GPIO_SetBits  (GPIOB,GPIO_Pin_1);\
								else		\
									GPIO_ResetBits(GPIOB,GPIO_Pin_1) 
/*************************************************/

void GPIO_init(void);
void HMC832_Write(unsigned long int data, unsigned char addr)	;
void HMC832_Init(void);
								
#endif
