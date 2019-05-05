#ifndef __func_h_
#define __func_h_

#include "stm32f10x.h"
#include "mytypedef.h"
#include "myflash.h"
#include "ioport.h"
#include "stdlib.h"
/*********************端口定义********************/
#define BACK_BASEADDR 0x0800FF00//低电量保存完成标志位存储地址
#define AGCCALVAL1_ADDR   BACK_BASEADDR + 12
#define AGCCALVAL2_ADDR   BACK_BASEADDR + 20
#define DEVICEADDR_ADDR   BACK_BASEADDR + 28
/*************************************************/
void CGA_Set(u8 cg);
void CGB_Set(u8 cg);
void ATTA_Set(u8 att);
void ATTB_Set(u8 att);
u8 FA_Check(void);
void LowPW_Init(void);
Sys_Para Load_SysPara(void);
void Save_SysPara(Sys_Para sPara,double adc_val1,double adc_val2);
double Cal_AGCVal(u8 ch );  //ch = 5 /4    5---adc2   4 -- adc1  
u8 AGC_Ctl(double agc_basic , double adc_valdb , u8 ch);
#endif
