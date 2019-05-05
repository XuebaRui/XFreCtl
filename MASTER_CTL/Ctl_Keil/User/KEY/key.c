#include "key.h"


/*
**�������� Key_IOInit 
**����  �� ��
**����  �� ��ʼ������IO�� ���� 
**����	�� 2019-02-26
**����  �� ����
*/
void Key_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOA, ENABLE); 

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

/************************END OF FILE**********************/