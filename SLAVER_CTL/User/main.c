#include "stm32f10x.h"
#include "uart.h"
#include "delay.h"
#include "stc.h"
#include "adc.h"
#include "math.h"
#include "hard_spi.h"
#include "hmc832.h"
#include "NFRE.h"
#include "func.h"
#include "string.h"
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
extern u8 Spi1_RecBuff[7]; //主机数据接收
extern u8 Spi1_Reclen;
extern u8 Spi1_RecFinish;
extern u8 rtn_buf[6];
extern u16 cmd_cnt;
typedef struct 
{
	u32  cf;
	u8 cg;
	u8 att;
	u8 bw;
	u8 agc;
}Sys_Para; //系统参数

void IWDG_Configuration(void);
int main(void)
{
	Sys_Para cur_para,old_para;
	u8 pow_on = 0;
  volatile double agc_val = 0;
	volatile double agc_basic = 1.229;
	volatile RCC_ClocksTypeDef get_rcc_clock;
	RCC_GetClocksFreq(&get_rcc_clock);  	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	memset(&cur_para,0,sizeof(cur_para));
	memset(&old_para,0,sizeof(old_para));
	TIM4_Init();    //延时初始化
	HMC832_Init();  //初始化
	NFRE_Init();    //4.7G
	IO_Init();      // 控制IO初始化
	Spi1_Init();    //从机SPI 初始化
	//while(!Spi1_RecFinish); //等待主机一次初始化
	IWDG_Configuration();  //开看门狗
	while(1)
	{
		if(Spi1_RecFinish)
		{
			if(Spi1_RecBuff[0] == 0x5a && Spi1_RecBuff[6] == 0xa5)
			{
				cur_para.cf = (Spi1_RecBuff[1]<<16 | Spi1_RecBuff[2] << 8 | Spi1_RecBuff[3]) >> 3;
				cur_para.cg = ((Spi1_RecBuff[3]&0x07) << 3) | (Spi1_RecBuff[4] >> 5);
				cur_para.att = Spi1_RecBuff[4] & 0x1f;
				cur_para.bw = Spi1_RecBuff[5] >> 6;
				cur_para.agc = (Spi1_RecBuff[5] >> 5) & 0x01;
//				rtn_buf[0] = ((old_para.cf << 3)>>16) & 0xff;
//				rtn_buf[1] = ((old_para.cf << 3)>>8) & 0xff;
//				rtn_buf[2] = ((old_para.cf << 3) | (old_para.cg >> 3))& 0xff;
//				rtn_buf[3] = (old_para.cg << 5)|(old_para.att);
//				rtn_buf[4] = (old_para.bw << 6 | old_para.agc << 5)& 0xff;
//				rtn_buf[5] = 0xb5;  //目前状态返回数组
				if(cur_para.cf != old_para.cf || pow_on == 0)
				{
					Fre_Ctl(cur_para.cf);   //设置频率
					old_para.cf = cur_para.cf;
				}			
				if(cur_para.cg != old_para.cg || pow_on == 0)
				{
					CG_Ctl(50 - cur_para.cg);
					old_para.cg = cur_para.cg;
				}	
				if(cur_para.att != old_para.att || pow_on == 0)
				{
					ATT_Ctl(cur_para.att);
					old_para.att = cur_para.att;
				}
				if(cur_para.bw != old_para.bw || pow_on == 0)
				{
					BW_Ctl(cur_para.bw);
					old_para.bw = cur_para.bw;
				}
				if(cur_para.agc != old_para.agc || pow_on == 0)
				{				
					old_para.agc = cur_para.agc;
				}
			}
			rtn_buf[0] = ((old_para.cf << 3)>>16) & 0xff;
			rtn_buf[1] = ((old_para.cf << 3)>>8) & 0xff;
			rtn_buf[2] = ((old_para.cf << 3) | (old_para.cg >> 3))& 0xff;
			rtn_buf[3] = (old_para.cg << 5)|(old_para.att);
			rtn_buf[4] = (old_para.bw << 6 | old_para.agc << 5)& 0xff;
			rtn_buf[5] = 0xb5;  //目前状态返回数组
			memset(&cur_para,0,sizeof(cur_para));
			memset(Spi1_RecBuff,0,sizeof(Spi1_RecBuff));
			Spi1_RecFinish = 0 ;
			Spi1_Reclen = 0;
			pow_on = 1;
		}
		if(cmd_cnt <= 40000)	
		{
			if(cmd_cnt == 40000)
			{
				Spi1_RecFinish = 0;
				Spi1_Reclen = 0;
				memset(Spi1_RecBuff,0,sizeof(Spi1_RecBuff));
			}
			cmd_cnt ++;
		}
		IWDG->KR=0XAAAA;  												      //喂狗
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
    IWDG_SetReload(0x190); //0x190*256/40k=2.56s
    IWDG_ReloadCounter();
    IWDG_Enable(); 
}
/*********************************************END OF FILE**********************/