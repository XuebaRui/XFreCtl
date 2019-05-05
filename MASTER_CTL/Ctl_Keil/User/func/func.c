#include "func.h"
#include "string.h"
#include "adc.h"
#include "myflash.h"
#include "delay.h"
#include "math.h"

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
u8 Sys_ParaInit(void)
{
	Sys_Para para;
	
	return Success;
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
/************************END OF FILE**********************/