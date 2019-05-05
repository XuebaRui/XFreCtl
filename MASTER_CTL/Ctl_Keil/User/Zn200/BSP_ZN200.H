#ifndef __BSP_ZN200_H_
#define	__BSP_ZN200_H_

#include "stm32f10x.h"

#define CFG(a)   if(a) \
											GPIO_SetBits(GPIOD,GPIO_Pin_7); \
										else \
											GPIO_ResetBits(GPIOD,GPIO_Pin_7); 
#define WDI(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_5); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_5); 
										
#define RST(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_3); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_3); 

#define Udp_SendStr  usart1Printf
void Usart_TIM2_Init(void);										
void ZN200_Init(void);
#endif
