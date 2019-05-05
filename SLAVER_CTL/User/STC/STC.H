#ifndef _stc_h
#define _stc_h

#include "stm32f10x.h"

#define Stc1_4(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_4); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_4); 
#define Stc1_3(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_3); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_3); 
										
#define Stc1_2(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_15); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_15); 
#define Stc1_1(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_12); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_12); 

#define Stc2_5(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_11); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_11); 
#define Stc2_4(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_10); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_10);  
#define Stc2_3(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_9); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_9); 
#define Stc2_2(a)   if(a) \
											GPIO_SetBits(GPIOA,GPIO_Pin_8); \
										else \
											GPIO_ResetBits(GPIOA,GPIO_Pin_8); 
#define Stc2_1(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_15); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_15); 

#define Stc3_5(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_9); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_9); 
#define Stc3_4(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_8); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_8); 
#define Stc3_3(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_7); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_7); 
#define Stc3_2(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_6); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_6); 
#define Stc3_1(a)   if(a) \
											GPIO_SetBits(GPIOB,GPIO_Pin_5); \
										else \
											GPIO_ResetBits(GPIOB,GPIO_Pin_5); 



void AGC_CTL(unsigned char stc1,unsigned char stc2);
void MGC_CTL(unsigned char stc1,unsigned char stc2,char stc3);
void STC_GPIO_Init(void);
void AGC_STEP(char stc3);

#endif
