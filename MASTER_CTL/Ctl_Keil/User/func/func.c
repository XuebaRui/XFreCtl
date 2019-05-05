#include "func.h"
#include "string.h"
#include "adc.h"
#include "myflash.h"
#include "delay.h"
#include "math.h"
/*
**函数名： CGA_Set
**参数  ： cg 0-60 db
**功能  ： 变频衰减控制
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void CGA_Set(u8 cg)
{
	u8 tmp_cg = cg;
	if(cg > 31)
	{
		tmp_cg = tmp_cg - 31;
		AGC1V1_1(0);
		AGC1V1_2(0);
		AGC1V1_3(0);
		AGC1V1_4(0);
		AGC1V1_5(0);
		AGC1V2_5(!(tmp_cg&0x01));
		AGC1V2_4(!(tmp_cg&0x02));
		AGC1V2_3(!(tmp_cg&0x04));
		AGC1V2_2(!(tmp_cg&0x08));
		AGC1V2_1(!(tmp_cg&0x10));
	}
	else
	{
		AGC1V1_5(!(cg&0x01));
		AGC1V1_4(!(cg&0x02));
		AGC1V1_3(!(cg&0x04));
		AGC1V1_2(!(cg&0x08));
		AGC1V1_1(!(cg&0x10));
		AGC1V2_1(1);
		AGC1V2_2(1);
		AGC1V2_3(1);
		AGC1V2_4(1);
		AGC1V2_5(1);
	}
}
/*
**函数名： CGB_Set
**参数  ： cg 0-60 db
**功能  ： 变频衰减控制
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void CGB_Set(u8 cg)
{
	u8 tmp_cg = cg;
	if(cg > 31)
	{
		tmp_cg = tmp_cg - 31;
		AGC2V1_1(0);
		AGC2V1_2(0);
		AGC2V1_3(0);
		AGC2V1_4(0);
		AGC2V1_5(0);
		AGC2V2_5(!(tmp_cg&0x01));
		AGC2V2_4(!(tmp_cg&0x02));
		AGC2V2_3(!(tmp_cg&0x04));
		AGC2V2_2(!(tmp_cg&0x08));
		AGC2V2_1(!(tmp_cg&0x10));
	}
	else
	{
		AGC2V1_5(!(cg&0x01));
		AGC2V1_4(!(cg&0x02));
		AGC2V1_3(!(cg&0x04));
		AGC2V1_2(!(cg&0x08));
		AGC2V1_1(!(cg&0x10));
		AGC2V2_1(1);
		AGC2V2_2(1);
		AGC2V2_3(1);
		AGC2V2_4(1);
		AGC2V2_5(1);
	}
}
/*
**函数名： ATTB_Set
**参数  ： att 0-30 db
**功能  ： 末级衰减控制
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void ATTA_Set(u8 att)
{
	AGC1V3_5(!(att&0x01));
	AGC1V3_4(!(att&0x02));
	AGC1V3_3(!(att&0x04));
	AGC1V3_2(!(att&0x08));
	AGC1V3_1(!(att&0x10));
}
/*
**函数名： ATTB_Set
**参数  ： cg 0-60 db
**功能  ： 末级衰减控制
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void ATTB_Set(u8 att)
{
	AGC2V3_5(!(att&0x01));
	AGC2V3_4(!(att&0x02));
	AGC2V3_3(!(att&0x04));
	AGC2V3_2(!(att&0x08));
	AGC2V3_1(!(att&0x10));
}
/*
**函数名： Load_SysPara
**参数  ： 无
**功能  ： 掉电保存功能初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void LowPW_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_InitStructure.EXTI_Line = EXTI_Line16;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	PWR_PVDLevelConfig(PWR_PVDLevel_2V9); //门限2.9v
	PWR_PVDCmd(ENABLE);
}

/*
**函数名： Load_SysPara
**参数  ： 无
**功能  ： 装载 FLASH 里的设置参数 如果没有则 设置为默认缺省状态
**日期	： 2019-02-26
**作者  ： 王瑞
*/
 
Sys_Para Load_SysPara(void)
{
	Sys_Para Back_Para;
	u32 tmp = 0;
	
	memset(&Back_Para,0,sizeof(Back_Para));
	tmp = FLASH_ReadHalfWord(BACK_BASEADDR);
	tmp = tmp << 16;
	tmp |= FLASH_ReadHalfWord(BACK_BASEADDR + 2);
	Back_Para.cf = (double)(tmp/1000.0);
	tmp = FLASH_ReadHalfWord(BACK_BASEADDR + 4);
	Back_Para.att = tmp;
	Back_Para.cg = tmp >> 8;
	tmp = FLASH_ReadHalfWord(BACK_BASEADDR + 6);
	Back_Para.agc = tmp;
	tmp = FLASH_ReadHalfWord(DEVICEADDR_ADDR);
	Back_Para.addr = tmp;
	Back_Para.rem = LOCAL;
		//FLASH_WriteHalfWord(BACK_BASEADDR + 8,0xffff); //读完
	if(Back_Para.cg > 50)	//给默认初始值
	{
		Back_Para.agc = MGC;
		Back_Para.cg = 50;
		Back_Para.att = 0;
		Back_Para.rem = LOCAL;
		Back_Para.cf = 1698.000;
		Back_Para.addr = 0;
	}
	return Back_Para;
}
/*
**函数名： Load_SysPara
**参数  ： sPara  传入参数保存
**功能  ： 保存系统参数
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void Save_SysPara(Sys_Para sPara,double adc_val1,double adc_val2)
{
	u32 offsetAddress=BACK_BASEADDR-FLASH_BASE;               //相对基地址的偏移
  u32 sectorPosition=offsetAddress/SECTOR_SIZE;             //计算扇区位置
  u32 sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;   //扇区首地址
	u32 int_adcsum = 0;
	FLASH_Unlock(); 
  FLASH_ErasePage(sectorStartAddress);                           //擦除扇区
	FLASH_WriteHalfWord(BACK_BASEADDR, ((u32)(sPara.cf*1000) >> 16));
	FLASH_WriteHalfWord(BACK_BASEADDR + 2,(u16)(sPara.cf*1000));
	FLASH_WriteHalfWord(BACK_BASEADDR + 4,((u16)sPara.cg << 8) | sPara.att);	
	FLASH_WriteHalfWord(BACK_BASEADDR + 6,sPara.agc);
	
	FLASH_WriteHalfWord(DEVICEADDR_ADDR,sPara.addr);
	int_adcsum = ((u32)(adc_val1 * 10000 +0.5));
	FLASH_WriteHalfWord(AGCCALVAL1_ADDR,int_adcsum >> 16);  //存储 AGC1
	FLASH_WriteHalfWord(AGCCALVAL1_ADDR + 2,int_adcsum); 
	
	int_adcsum = ((u32)(adc_val2 * 10000 +0.5));
	FLASH_WriteHalfWord(AGCCALVAL2_ADDR,int_adcsum >> 16);  //存储 AGC2
	FLASH_WriteHalfWord(AGCCALVAL2_ADDR + 2,int_adcsum);
	FLASH_WriteHalfWord(BACK_BASEADDR + 30,0x915a); //写完
	//int_adcsum = FLASH_ReadHalfWord(BACK_BASEADDR + 30);
	FLASH_Lock(); 
}
/*
**函数名： FA_Check
**参数  ： 无
**功能  ： 返回本振工作状态
**日期	： 2019-02-26
**作者  ： 王瑞
*/
u8 FA_Check(void)
{
	return (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) * 10)+
					GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0);
}
/*
**函数名： Cal_AGCVal
**参数  ： CH  ADC 通道  
**功能  ： 返回0dbm时AGC_VAL值存入 FLASH   
**日期	： 2019-03-26
**作者  ： 王瑞
*/
double Cal_AGCVal(u8 ch)
{
	double Adc_CAlVal[256],temp = 0,adc_valsum = 0;
	u16 i = 0,j = 0;
	u32 int_adcsum = 0;
	u8 retry = 10;
	for(i = 0 ;i < 256 ; i++)             // 排序 平均
	{
		Adc_CAlVal[i] = ADC_GetVal(ch);
		delay_ms(20);
	}
	for(i = 0 ;i < 256 - 1 ; i++)
	{
		for(j = 0 ;j < 256 - 1 - i ; j++)
		{
			if(Adc_CAlVal[j] > Adc_CAlVal[j+1])
			{
				temp = Adc_CAlVal[j];
				Adc_CAlVal[j] = Adc_CAlVal[j + 1];
				Adc_CAlVal[j + 1] = temp;
			}
		}
	}
	for(i = 20; i < 235 ;i ++)
	{
		adc_valsum += Adc_CAlVal[i];
	}
	adc_valsum = adc_valsum / 215;
	int_adcsum = ((u32)(adc_valsum * 10000 +0.5));
	return int_adcsum / 10000.0;
}
/*
**函数名： AGC()
**参数  ： 0DBM时AGC的值   adc_valdb -- mv 每DB 校正得来或者手动输入  0.022v   ch -- 1 ch -- 2
**功能  ： 返回0dbm时AGC_VAL值存入 FLASH   
**日期	： 2019-03-27
**作者  ： 王瑞
*/
u8 AGC_Ctl(double agc_basic , double adc_valdb , u8 ch)
{
	double agc_bas = agc_basic;
	double agc_widget = 2 * adc_valdb + 0.25 * adc_valdb; //2db 门限
	double adc_mvdb = adc_valdb;   //22mv/dbm
	double adc_val = 0;					//当前ADC值
	signed char cur_stc = 0;             //当前衰减
	volatile double adc_diff = 0;
	adc_val = ADC_GetVal(ch);
	adc_diff = agc_basic - adc_val;
	if(fabs(agc_basic - adc_val) >= agc_widget) //2db 多的门
	{
		//AGC 方式1
		if(adc_val > agc_basic)   //功率越大 检波电压越小
		{		
			if(adc_diff >= 0.36) //0.36/0.022
			{
				cur_stc = (signed char)(adc_diff / adc_mvdb);
				if(cur_stc < -50)
					cur_stc = -50;
			}
			else if(adc_diff <= -0.24)
			{
				cur_stc = -10;
			}
			else if(adc_diff <= -0.12)
			{
				cur_stc = -5;		
			}
			else if(adc_diff <= -0.048)
			{
				cur_stc = -2;			
			}
			else if(adc_diff <= -0.024)
			{
				cur_stc = -1;		
			}
			else
			{
				cur_stc = cur_stc;
			}
		}
		else
		{
			if(adc_diff >= 0.36) //0.36/0.022
			{
				cur_stc = (signed char)(adc_diff / adc_mvdb);
				if(cur_stc > 50)
				{
					cur_stc = 50;
				}
			}
			else if(adc_diff >= 0.24)
			{
				cur_stc = 10;
			}
			else if(adc_diff >= 0.12)
			{
				cur_stc = 5;		
			}
			else if(adc_diff >= 0.048)
			{
				cur_stc = 2;		
			}
			else if(adc_diff >= 0.024)
			{
				cur_stc = 1;		
			}
			else
			{
				cur_stc = cur_stc;
			}
		}
//		//agc方式2 
//		if(adc_val > agc_basic)   //功率越大 检波电压越小
//		{		
//				cur_stc = cur_stc - (u16)(adc_diff / adc_mvdb);
//				if(cur_stc < 0)
//				{
//					cur_stc = 0;
//				}
//		}
//		else
//		{
//				cur_stc = cur_stc + (u16)(adc_diff / adc_mvdb);
//				if(cur_stc > 50)
//				{
//					cur_stc = 50;
//				}
//		}
	}
	return cur_stc;
}
	


/************************END OF FILE**********************/