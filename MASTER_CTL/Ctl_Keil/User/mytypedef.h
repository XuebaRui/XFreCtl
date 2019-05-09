#ifndef __mytypedef_h_
#define __mytypedef_h_

#include "stm32f10x.h"
#define DeBug						1
#define PowON_Pic       0
typedef struct 
{
	u8 rem;
	double  cf;
	u8 cg;
	u8 att;
	u8 bw;
	u8 agc;
	u8 addr;
}Sys_Para; //系统参数
#define	REMOTE  				0
#define	LOCAL   				1
#define OFF 						0
#define ON  						1
#define BW_180Mhz				1
#define BW_240Mhz				2
#define BW_500Mhz				3
#define AGC							1
#define MGC							0
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
#endif
/************************END OF FILE**********************/