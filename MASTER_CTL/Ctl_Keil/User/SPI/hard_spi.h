#ifndef __hard_spi_h_
#define __hard_spi_h_

#include "stm32f10x.h"
#include "string.h"

#include "stdio.h"
#define SlaverDevice1_SendByte Spi1_SendByte
#define SlaverDevice2_SendByte Spi2_SendByte
void Spi1_Init(void);
void Spi2_Init(void);
u8 SPI1_SendByte(u8 data);
u8 SPI2_SendByte(u8 data);
void SPI_Send(u8 fre,u8 ref,u8 rec_data[3]);
#endif
