#ifndef __key_h_
#define __key_h_

#include "stm32f10x.h"
#include "mytypedef.h"

/*********************¶Ë¿Ú¶¨Òå********************/

#define 	 Key_Left	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10)
#define   Key_Up  	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
#define   Key_Right	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)
#define		Key_Ok		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define		Key_Down		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9)

/*************************************************/

#endif
										
