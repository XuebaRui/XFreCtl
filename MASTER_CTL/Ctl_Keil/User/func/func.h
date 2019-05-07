#ifndef __func_h_
#define __func_h_

#include "stm32f10x.h"
#include "mytypedef.h"
#include "myflash.h"
#include "ioport.h"
#include "stdlib.h"
/*********************�˿ڶ���********************/
#define BACK_BASEADDR 0x0800FF00//�͵���������ɱ�־λ�洢��ַ

#define DEVICEADDR_ADDR   BACK_BASEADDR + 28
#define Success	1
#define Fail  0
/*************************************************/
void LowPW_Init(void);
Sys_Para Load_SysPara(void);
u8 SlaverDevice_Ctl(Sys_Para para);
void Save_SysPara(Sys_Para sPara);
#endif
