#include "NFRE.h"

void NFRE_Init(void)
{
		//io≥ı ºªØ
		GPIO_InitTypeDef GPIO_InitStructure;
	
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE); 											   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15	;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|	GPIO_Pin_6| GPIO_Pin_5| GPIO_Pin_4 | GPIO_Pin_3;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		N3(1);
		N2(0);
		N1(1);
		N0(0);
		S1(1);	
  	S0(1);  // 4.7gHZ
}


