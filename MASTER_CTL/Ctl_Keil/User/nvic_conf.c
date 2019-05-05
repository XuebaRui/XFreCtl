#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "BSP_ZN200.h"
#include "mytypedef.h"
#include "myflash.h"
#include "func.h"
/**********************串口接收变量**************************/
u8 clear;
u8 Rec_Buffer[64] = {0}; //接收缓冲
u8 Rec_Len = 0;          //接收长度
u8 **Parse_Que = {0}; 
u8 Que_Len = 0;					 //最长10条命令缓存
/*********************中断优先级配置************************/
void BSP_NVIC_Configuration(void)
{
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
/*********************中断服务函数************************/
//串口2
void USART1_IRQHandler(void)   //以太网接收
{
    if(USART_GetITStatus(USART1,USART_IT_ORE) != RESET)
    {
        clear = USART1->SR;
        clear = USART1->DR;
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
				if(Que_Len < 10)
				{
					Rec_Buffer[Rec_Len++] = USART1->DR;
					TIM2->CNT = 0;					
					TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
				}
				else
				{
					clear = USART2->DR;
				}
				USART1->SR = (uint16_t)~((uint16_t)0x01 << (uint16_t)(USART_IT_RXNE >> 0x08));
    }
}
//串口1
/****
void USART1_IRQHandler(void)   //rs-232接收
{
    if(USART_GetITStatus(USART1,USART_IT_ORE) != RESET)
    {
        clear = USART1->SR;
        clear = USART1->DR;
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
				if(cmd_rec_finsh == 0 && Zn200_CMD_reciving == 0)
				{	

					TIM4->CNT = 0;
					TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
					TIM_Cmd(TIM2,ENABLE);
					
					cmd_data[cmd_len] = USART1->DR;
					cmd_len  = cmd_len + 1;
					Rs232_CMD_reciving = 1;
					if(cmd_len == 12)
					{
						cmd_rec_finsh = 1;
						Rs232_CMD_reciving = 0;
						cmd_len  = 0;
						TIM4->CNT = 0;
						TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
						TIM_Cmd(TIM2,DISABLE);
					}
				}
				else
				{
					clear = USART1->DR;
				}
				USART1->SR = (uint16_t)~((uint16_t)0x01 << (uint16_t)(USART_IT_RXNE >> 0x08));
    }
}*/
/*
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!= RESET)
	{

		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}*/


void TIM2_IRQHandler(void) //nms后串口没有新数据则串口接收完成
{

	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!= RESET)
	{
		if(Rec_Len > 0)
		{
			strcpy(*(Parse_Que),Rec_Buffer);
			Rec_Len = 0;
			Que_Len ++;
			memset(Rec_Buffer,0,sizeof(Rec_Buffer));
		}

		TIM2->CNT = 0;					
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
		TIM_Cmd(TIM2,DISABLE);
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
/*
**函数名： FLASH_WriteMoreData 
**参数  ： startAddress 写地址 writeData 写数据   countToWrite 数量
**功能  ： FLASH 写
**日期	： 2019-02-26
**作者  ： 王瑞
*/
extern Sys_Para cur_SysPara;
void PVD_IRQHandler(void)
{

	//#if DeBug
	if (EXTI_GetITStatus(EXTI_Line16) != RESET)
	{
		if(PWR_GetFlagStatus(PWR_FLAG_PVDO))
		{
		//	Save_SysPara(cur_SysPara,AGC_BASIC1,AGC_BASIC2);
		}
		PWR_ClearFlag(PWR_FLAG_PVDO);
		EXTI_ClearITPendingBit(EXTI_Line16);
	}
	//#endif
}
/************************END OF FILE**********************/