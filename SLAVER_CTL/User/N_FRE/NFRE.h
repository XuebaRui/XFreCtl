#ifndef NFRE_H_
#define NFRE_H_

#include "stm32f10x.h"

#define S0(a)  if(a) \
							GPIO_SetBits(GPIOB,GPIO_Pin_7); \
						else \
							GPIO_ResetBits(GPIOB,GPIO_Pin_7); 
#define S1(a)  if(a) \
							GPIO_SetBits(GPIOB,GPIO_Pin_6); \
						else \
							GPIO_ResetBits(GPIOB,GPIO_Pin_6); 
#define N0(a)  if(a) \
							GPIO_SetBits(GPIOB,GPIO_Pin_5); \
						else \
							GPIO_ResetBits(GPIOB,GPIO_Pin_5); 
#define N1(a)  if(a) \
							GPIO_SetBits(GPIOB,GPIO_Pin_4); \
						else \
							GPIO_ResetBits(GPIOB,GPIO_Pin_4); 
#define N2(a)  if(a) \
							GPIO_SetBits(GPIOB,GPIO_Pin_3); \
						else \
							GPIO_ResetBits(GPIOB,GPIO_Pin_3); 
#define N3(a)  if(a) \
							GPIO_SetBits(GPIOA,GPIO_Pin_15); \
						else \
							GPIO_ResetBits(GPIOA,GPIO_Pin_15); 

void NFRE_Init(void);
#endif //整数步进频综 4.7 -- 5.3 
/***** N5 N4 N3 N2 N1 N0 S1 S0**/
/* 真值表
**4.7  0  0  1  0  1  0  1  1          
**4.8  0  0  1  0  1  1  0  0 
**4.9	 0  0  1  0  1  1  0  1				
**5.0  0  0  1  0  1  1  1  0
**5.1  0  0  1  0  1  1  1  1
**5.2  0  0  1  1  0  0  0  0
**5.3  0  0  1  1  0  0  0  1
*/
						