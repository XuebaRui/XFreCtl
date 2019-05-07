#include "stm32f10x.h"
#include "uart.h"
#include "delay.h"
#include "stc.h"
#include "adc.h"
#include "math.h"
#include "hard_spi.h"
#include "hmc832.h"
#include "NFRE.h"
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

char MODE = MGC;
u16 mgc_stc=0;
void IWDG_Configuration(void);
int main(void)
{
  volatile double agc_val = 0;
	volatile double agc_basic = 1.229;
	int stc = 0;
	volatile RCC_ClocksTypeDef get_rcc_clock;
	RCC_GetClocksFreq(&get_rcc_clock);   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	GPIO_init();
	HMC832_Init(); //初始化
	NFRE_Init(); //4.7G
	
	while(1)
	{
		delay_ms(10);
	}
	//
	//IWDG_Configuration();
	//Spi1_Init(); //主从通信
	//STC_GPIO_Init();
	//adc_init();
	//TIM4_Init();
	//MGC_CTL(0,0,0);
//  while(1)
//	{
//		if(MODE == AGC)
//		{
//			agc_val = ADC_GetVal();
//			agc_val = agc_val + 0.003;	
//			if(fabs(agc_val - agc_basic) >=0.048)
//			{
//				if(agc_val>agc_basic)
//				{
//					if(agc_val - agc_basic >= 0.36)
//					{
//						stc = stc + (agc_val-agc_basic)/0.022;
//						if(stc >60)
//							stc = 60;					
//					}
//					else if(agc_val - agc_basic >= 0.24)
//					{
//						stc = stc + 10;
//						if(stc >60)
//							stc = 60;					
//					}
//					else if( agc_val - agc_basic >= 0.12)
//					{
//						stc = stc + 5;
//						if(stc >60)
//							stc = 60;					
//					}
//					else if(agc_val - agc_basic >= 0.048)
//					{
//							stc = stc + 2;
//							if(stc >60)
//							stc = 60;
//					}
//					else if(agc_val - agc_basic >= 0.024)
//					{
//						stc = stc + 2;
//						if(stc >60)
//							stc = 60;
//					}
//					else 
//						stc = stc;
//				}
//				else
//				{
//					if(agc_basic - agc_val >= 0.36)
//					{
//						stc = stc - (agc_basic-agc_val)/0.022;
//						if(stc <0)
//							stc = 0;					
//					}
//					else if(agc_basic - agc_val >= 0.24)
//					{
//						stc = stc - 10;
//						if(stc<0)
//						stc =0 ;
//					}
//					else if( agc_basic - agc_val >= 0.12)
//					{
//						stc = stc - 5;
//							if(stc<0)
//								stc =0 ;
//					}
//					else if(agc_basic - agc_val >= 0.048)
//					{
//							stc = stc - 2;
//							if(stc<0)
//								stc =0 ;
//					}
//					else if(agc_basic - agc_val >= 0.024)
//					{
//						stc = stc - 1;
//						if(stc<0)
//						stc =0 ;
//					}
//					else 
//						stc = stc;		
//				}
//			}
//			if(stc > 0)
//			{
//					if(stc < 62)
//					{
//						if(stc > 30)
//						{
//							if(stc % 2 == 0)
//							{
//								AGC_CTL(stc-30,30);
//							}
//							else
//							{
//								AGC_CTL(stc-31,31);
//							}
//						}
//						else 
//						{
//							AGC_CTL(0,stc);
//						}
//				}
//			}
//			else 
//			{
//				AGC_CTL(0,0);
//			}
//		}
//		else
//		{
//			 MGC_CTL((u8)(((mgc_stc>>6)&0X0F)<<1),(u8)((mgc_stc>>1)&0X1F),0);
//		}
//		IWDG->KR=0XAAAA;   //喂狗
//	}
}
void IWDG_Configuration(void) //26s 看门口
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
    IWDG_SetPrescaler(IWDG_Prescaler_256); 
    IWDG_SetReload(0xFFF); //0xfff*256/40k=26s
    IWDG_ReloadCounter();
    IWDG_Enable(); 
}
/*********************************************END OF FILE**********************/