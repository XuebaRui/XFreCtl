#include "delay.h"
/*
**函数名： GPIO_init
**参数  ： 无
**功能  ： HMC832控制IO初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void Delay_TIM4_Init(void)
{

    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//时钟

    /* TIM2*/

    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 47;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM4->CNT = 0;
    TIM_Cmd(TIM4, DISABLE);
}

/*
**函数名： GPIO_init
**参数  ： 无
**功能  ： HMC832控制IO初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void delay_us(u32 us)
{
    TIM4->CNT = 0;
    TIM_Cmd(TIM4,ENABLE);
    while(TIM4 ->CNT <= us);
    TIM_Cmd(TIM4,DISABLE);
}
/*
**函数名： GPIO_init
**参数  ： 无
**功能  ： HMC832控制IO初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void delay_ms(u8 ms)
{
    TIM4->CNT = 0;
    TIM_Cmd(TIM4,ENABLE);
    while(TIM4 ->CNT <= ms*1000);
    TIM_Cmd(TIM4,DISABLE);
}
/*
**函数名： GPIO_init
**参数  ： 无
**功能  ： HMC832控制IO初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void delay_s(u8 s)
{
    int i = 0;
    for( i = 0; i<=s*20; i++)
    {
        delay_ms(50);
    }
}
//void upload_tim_Init(void)
//{

//    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
//		NVIC_InitTypeDef					NVIC_InitStructure;
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//时钟

//    /* TIM3*/

//    TIM_TimeBaseStructure.TIM_Period = 24999;
//    TIM_TimeBaseStructure.TIM_Prescaler = 9599;
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//    TIM4->CNT = 0;
//	
//		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
//		
//		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
//    TIM_Cmd(TIM3, ENABLE);
//}
/************************END OF FILE**********************/