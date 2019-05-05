#ifndef __my_usart1_h_
#define	__my_usart1_h_
/*tx---PA2   Rx----PA3*/

#include "stm32f10x.h"
#include <stdio.h>

void BSP_NVIC_Configuration(void);
void usart1Printf(char *fmt, ...);
void usart2Printf(char *fmt, ...);
void my_usart_config(u32 usart1_baud,u32 usart2_baud);
void USART2_DMA_Config(void);
#endif
