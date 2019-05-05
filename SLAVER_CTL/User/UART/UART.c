#include "uart.h"

void uart_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure ;
	
	//打开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	
	//TX GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//RX GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate =  115200 ; //波特率
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode =  USART_Mode_Rx | USART_Mode_Tx;  
	USART_InitStructure.USART_Parity= USART_Parity_No ; 	
	USART_InitStructure.USART_StopBits  = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART2, &USART_InitStructure); 
	USART_Cmd(USART2, ENABLE);
	
}


///重定向c库函数printf到USART2
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART2, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
	
		return (ch);
}
