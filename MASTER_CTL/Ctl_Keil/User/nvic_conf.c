#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "BSP_ZN200.h"
#include "mytypedef.h"
#include "myflash.h"
#include "func.h"
/**********************´®¿Ú½ÓÊÕ±äÁ¿**************************/
u8 clear;
u8 Rec_Buffer[64] = {0}; //½ÓÊÕ»º³å
u8 Rec_Len = 0;          //½ÓÊÕ³¤¶È
u8 **Parse_Que = {0}; 
u8 Que_Len = 0;					 //×î³¤10ÌõÃüÁî»º´æ
/*********************ÖÐ¶ÏÓÅÏÈ¼¶ÅäÖÃ************************/
void BSP_NVIC_Configuration(void)
{
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
/*********************ÖÐ¶Ï·þÎñº¯Êý************************/
//´®¿Ú2
void USART1_IRQHandler(void)   //ÒÔÌ«Íø½ÓÊÕ
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
//´®¿Ú1
/****
void USART1_IRQHandler(void)   //rs-232½ÓÊÕ
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


void TIM2_IRQHandler(void) //nmsºó´®¿ÚÃ»ÓÐÐÂÊý¾ÝÔò´®¿Ú½ÓÊÕÍê³É
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
**º¯ÊýÃû£º FLASH_WriteMoreData 
**²ÎÊý  £º startAddress Ð´µØÖ· writeData Ð´Êý¾Ý   countToWrite ÊýÁ¿
**¹¦ÄÜ  £º FLASH Ð´
**ÈÕÆÚ	£º 2019-02-26
**×÷Õß  £º ÍõÈð
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

/*
**º¯ÊýÃû£º SPI½ÓÊÕ ÖÐ¶Ï
**²ÎÊý  £º 
**¹¦ÄÜ  £º FLASH Ð´
**ÈÕÆÚ	£º 2019-05-07
**×÷Õß  £º ÍõÈð
*/
extern u8 s_ack1;  //½ÓÊÕÖÐ¶ÏÏìÓ¦  ´Ó»ú1ÏìÓ¦±êÖ¾
extern u8 s_ack2;  //½ÓÊÕÖÐ¶ÏÏìÓ¦  ´Ó»ú2ÏìÓ¦±êÖ¾
u8 Spi1_RecBuff[6] = {0}; //´Ó»ú1·µ»ØÊý¾
u8 Spi1_Reclen = 0;
u8 Spi1_RecFinish = 0;
void SPI1_IRQHandler(void)
{
  u8 tmp = 0;
	if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE) != RESET) 
	{
		if(Spi1_Reclen <10 && !Spi1_RecFinish)
		{
			Spi1_RecBuff[Spi1_Reclen++] = SPI1->DR;
			if(Spi1_Reclen == 10)
			{
				if(Spi1_RecBuff[5] == 0xa5)
				{
					Spi1_RecFinish = 1; //´Ó»ú·µ»Ø³É¹¦
				}
				else
				{
					Spi1_RecFinish = 0;
					Spi1_Reclen = 0;
					memset(Spi1_RecBuff,0,sizeof(Spi1_RecBuff));
				}
			}
		}
		else
		{
			Spi1_RecFinish = 0;
			Spi1_Reclen = 0;
			memset(Spi1_RecBuff,0,sizeof(Spi1_RecBuff));
		}
		s_ack1 = 1; //´Ó»ú·µ»Ø
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
	}
}
u8 Spi2_Reclen = 0;
u8 Spi2_RecFinish = 0;
u8 Spi2_RecBuff[6] = {0};//´Ó»ú2·µ»ØÊý¾Ý
void SPI2_IRQHandler(void)
{
  u8 tmp = 0;
	if(SPI_I2S_GetITStatus(SPI2,SPI_I2S_IT_RXNE) != RESET) 
	{
		if(Spi2_Reclen <10 && !Spi2_RecFinish)
		{
			Spi2_RecBuff[Spi2_Reclen++] = SPI1->DR;
			if(Spi2_Reclen == 10)
			{
				if(Spi2_RecBuff[5] == 0xa5)
				{
					Spi2_RecFinish = 1; //´Ó»ú·µ»Ø³É¹¦
				}
				else
				{
					Spi2_RecFinish = 0;
					Spi2_Reclen = 0;
					memset(Spi2_RecBuff,0,sizeof(Spi2_RecBuff));
				}
			}
		}
		else
		{
			Spi2_RecFinish = 0;
			Spi2_Reclen = 0;
			memset(Spi2_RecBuff,0,sizeof(Spi2_RecBuff));
		}
		s_ack2 = 1; //´Ó»ú·µ»Ø
		SPI_I2S_ClearITPendingBit(SPI2,SPI_I2S_IT_RXNE);
	}
}
/************************END OF FILE**********************/