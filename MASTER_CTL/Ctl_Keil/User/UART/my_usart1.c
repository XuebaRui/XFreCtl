#include "my_usart1.h"
#include "stdarg.h"

#define CMD_BUFFER_LEN 128


//串口初始化 控制LCD 以及GPRS
void my_usart_config(u32 usart1_baud,u32 usart2_baud)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure ;
	
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
 	

	/**********************************串口1初始化**********************************/	
		//TX GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//RX GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate =  usart1_baud ; //波特率
	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode =  USART_Mode_Rx | USART_Mode_Tx;  
	USART_InitStructure.USART_Parity= USART_Parity_No ; 	
	USART_InitStructure.USART_StopBits  = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1, &USART_InitStructure); 
	USART_Cmd(USART1, ENABLE);
	/*******************************************************************************/	
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//	/****测试串口****/
//	//TX GPIO
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	//RX GPIO
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	USART_InitStructure.USART_BaudRate =  115200 ; //波特率
//	USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None; 
//	USART_InitStructure.USART_Mode =  USART_Mode_Rx | USART_Mode_Tx;  
//	USART_InitStructure.USART_Parity= USART_Parity_No ; 	
//	USART_InitStructure.USART_StopBits  = USART_StopBits_1;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	
//	USART_Init(USART3, &USART_InitStructure); 
//	USART_Cmd(USART3, ENABLE);
}

void usart1Printf(char *fmt, ...)
{
    char buffer[CMD_BUFFER_LEN - 1];
    u8 i = 0;
    u8 len;

    va_list arg_ptr; //Define convert parameters variable
    va_start(arg_ptr, fmt); //Init variable
    len = vsnprintf(buffer, CMD_BUFFER_LEN+1, fmt, arg_ptr); //parameters list format to buffer
    
    while ((i < CMD_BUFFER_LEN) && (i < len) && (len > 0))
    {
        USART_SendData(USART1, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
    }
    va_end(arg_ptr);
}

