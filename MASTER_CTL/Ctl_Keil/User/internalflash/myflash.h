#ifndef __myflash_h_
#define __myflash_h_

#include "stm32f10x.h"
#include "mytypedef.h"
#define FLASH_SIZE 64          //512kflash

#if FLASH_SIZE<256
  #define SECTOR_SIZE           1024    //������С
#else 
  #define SECTOR_SIZE           2048    //������С
#endif
/*********************�˿ڶ���********************/

/*************************************************/
void FLASH_WriteHalfWord(u32 startAddress, u16 data);
uint16_t FLASH_ReadHalfWord(uint32_t address);
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
#endif
