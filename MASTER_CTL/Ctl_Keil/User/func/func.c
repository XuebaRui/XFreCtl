#include "func.h"
#include "string.h"
#include "adc.h"
#include "myflash.h"
#include "delay.h"
#include "math.h"
#include "hard_spi.h"

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
**函数名： slaver_init
**参数  ： 无
**功能  ： 按照 PARA 的参数设置 两个从机场设备
**日期	： 2019-02-26
**作者  ： 王瑞
*/
u8 s_ack1 = 0;  //接收中断响应   
u8 s_ack2 = 0;  //接收中断响应
u8 SlaverDevice_Ctl(Sys_Para para)
{
	u8 s_buff[7] = {0},rtn_buff[6] = {0};
	u8 s_cnt = 0;
	u8 retry = 0;
	u32 tmp = 0;
	u8 i = 0;
	s_buff[0] = 0x5a; 
	tmp = (u32)(para.cf * 1000 + 0.5);
	tmp = tmp - 7750000;
	s_buff[1] = ((tmp << 3)>>16) & 0xff;
	s_buff[2] = ((tmp << 3)>>8) & 0xff;
	s_buff[3] = ((tmp << 3) | (para.cg >> 3))& 0xff;
	s_buff[4] = (para.cg << 5)|(para.att);
	s_buff[5] = (para.bw << 6 | para.agc << 5)& 0xff;
	s_buff[6] = 0xa5; 
	for(i = 0; i < 7 ;i ++)
	{
		SPI1_SendByte(s_buff[i]);
	}
	s_buff[0] = 0x5b; 
	s_buff[6] = 0xb5; 
	delay_ms(50);
	SPI1_SendByte(s_buff[0]);
	delay_us(2);
	for(i = 1; i < 7 ;i ++)
	{
		rtn_buff[i-1] = SPI1_SendByte(s_buff[i]);
		delay_us(2);
	}
 	return Success;  //成功
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
	tmp = FLASH_ReadHalfWord(BACK_BASEADDR + 8);
	Back_Para.bw = tmp;
	tmp = FLASH_ReadHalfWord(DEVICEADDR_ADDR);
	Back_Para.addr = tmp;
	Back_Para.rem = LOCAL;
	//FLASH_WriteHalfWord(BACK_BASEADDR + 8,0xffff); //读完
	if(Back_Para.cg > 50 || Back_Para.att > 50 || Back_Para.cf < 7750.000 || Back_Para.cf > 9000.000
					||	Back_Para.bw > 3 || Back_Para.agc > 1)	//给默认初始值
	{
		Back_Para.agc = MGC;
		Back_Para.cg = 50;
		Back_Para.att = 0;
		Back_Para.rem = LOCAL;
		Back_Para.cf = 7750.000;
		Back_Para.bw = BW_180Mhz;
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

void Save_SysPara(Sys_Para sPara)
{
	u32 offsetAddress=BACK_BASEADDR-FLASH_BASE;               //相对基地址的偏移
  u32 sectorPosition=offsetAddress/SECTOR_SIZE;             //计算扇区位置
  u32 sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;   //扇区首地址
	u32 int_adcsum = 0;
	FLASH_Unlock(); 
  FLASH_ErasePage(sectorStartAddress);                           //擦除扇区
	FLASH_WriteHalfWord(BACK_BASEADDR, ((u32)(sPara.cf*1000 + 0.5 ) >> 16));
	FLASH_WriteHalfWord(BACK_BASEADDR + 2,(u16)(sPara.cf*1000 + 0.5));
	FLASH_WriteHalfWord(BACK_BASEADDR + 4,((u16)sPara.cg << 8) | sPara.att);	
	FLASH_WriteHalfWord(BACK_BASEADDR + 6,sPara.agc);
	FLASH_WriteHalfWord(BACK_BASEADDR + 8,sPara.bw);
	FLASH_WriteHalfWord(DEVICEADDR_ADDR,sPara.addr);
	FLASH_Lock(); 
}
/************************END OF FILE**********************/