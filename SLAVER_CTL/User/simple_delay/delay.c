#include "delay.h"
#include "adc.h"
void TIM4_Init(void)
{

    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʱ��

    /* TIM2*/

    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 47;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM4->CNT = 0;
      
  
    TIM_Cmd(TIM4, DISABLE);
}
void delay_us(u32 us)
{
    TIM4->CNT = 0;
    TIM_Cmd(TIM4,ENABLE);
    while(TIM4 ->CNT <= us);
    TIM_Cmd(TIM4,DISABLE);
}
void delay_ms(u8 ms)
{
    TIM4->CNT = 0;
    TIM_Cmd(TIM4,ENABLE);
    while(TIM4 ->CNT <= ms*1000);
    TIM_Cmd(TIM4,DISABLE);
}
void delay_s(u8 s)
{
    int i = 0;
    for( i = 0; i<=s*20; i++)
    {
        delay_ms(50);
    }
}

