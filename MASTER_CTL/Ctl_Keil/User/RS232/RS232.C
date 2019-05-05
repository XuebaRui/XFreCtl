#include "rs232.h"


void RS232_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure ;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 
	//TX GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//RX GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RS232_Baud_Set(115200);
	/*******************************************************************************/
}

void RS232_Baud_Set(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	USART_Cmd(USART1, DISABLE);
	
	/********************************串口2初始化***********************************/
	USART_InitStructure.USART_BaudRate =  baud ; //波特率
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode =  USART_Mode_Rx | USART_Mode_Tx;  
	USART_InitStructure.USART_Parity= USART_Parity_No ; 	
	USART_InitStructure.USART_StopBits  = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure); 
	
	//串口1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1, ENABLE);
}
