#ifndef __hard_spi_h_
#define __hard_spi_h_

#include "stm32f10x.h"
#include "string.h"

#include "stdio.h"
void Spi1_Init(void);
void SPI_Send(u8 fre,u8 ref,u8 rec_data[3]);
#endif
