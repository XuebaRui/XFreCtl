#ifndef __RS232_H__
#define __RS232_H__

#include "stm32f10x.h"

void RS232_Init(void);

void RS232_Baud_Set(u32 baud);

#endif
