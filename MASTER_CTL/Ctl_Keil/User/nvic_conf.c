#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "BSP_ZN200.h"
#include "mytypedef.h"
#include "myflash.h"
#include "func.h"
/**********************���ڽ��ձ���**************************/
u8 clear;
u8 Rec_Buffer[64] = {0}; //���ջ���
u8 Rec_Len = 0;          //���ճ���
u8 **Parse_Que = {0}; 
u8 Que_Len = 0;					 //�10�������
/*********************�ж����ȼ�����************************/
void BSP_NVIC_Configuration(void)
{
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
/*********************�жϷ�����************************/
//����2
void USART1_IRQHandler(void)   //��̫������
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
//����1
/****
void USART1_IRQHandler(void)   //rs-232����
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


void TIM2_IRQHandler(void) //nms�󴮿�û���������򴮿ڽ������
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
**�������� FLASH_WriteMoreData 
**����  �� startAddress д��ַ writeData д����   countToWrite ����
**����  �� FLASH д
**����	�� 2019-02-26
**����  �� ����
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
**�������� SPI���� �ж�
**����  �� 
**����  �� FLASH д
**����	�� 2019-05-07
**����  �� ����
*/
extern u8 s_ack1;  //�����ж���Ӧ  �ӻ�1��Ӧ��־
extern u8 s_ack2;  //�����ж���Ӧ  �ӻ�2��Ӧ��־
u8 Spi1_RecBuff[6] = {0}; //�ӻ�1�������
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
					Spi1_RecFinish = 1; //�ӻ����سɹ�
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
		s_ack1 = 1; //�ӻ�����
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
	}
}
u8 Spi2_Reclen = 0;
u8 Spi2_RecFinish = 0;
u8 Spi2_RecBuff[6] = {0};//�ӻ�2��������
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
					Spi2_RecFinish = 1; //�ӻ����سɹ�
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
		s_ack2 = 1; //�ӻ�����
		SPI_I2S_ClearITPendingBit(SPI2,SPI_I2S_IT_RXNE);
	}
}
/************************END OF FILE**********************/