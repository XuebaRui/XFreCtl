#include "myflash.h"


/*
**函数名： FLASH_WriteMoreData 
**参数  ： startAddress 写地址 writeData 写数据   countToWrite 数量
**功能  ： FLASH 写
**日期	： 2019-02-26
**作者  ： 王瑞
*/
//void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
//{
//  if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE)))
//  {
//    return;
//  }
//  FLASH_Unlock();          //解锁FLASH
//  uint32_t offsetAddress=startAddress-FLASH_BASE;                //相对基地址的偏移
//  uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;             //计算扇区位置
//  
//  uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;   //扇区首地址

//  FLASH_ErasePage(sectorStartAddress);                           //擦除扇区
//  
//  for(dataIndex=0;dataIndex<countToWrite;dataIndex++)           //半字写入
//  {
//    FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
//  }
//  FLASH_Lock();//锁定FLASH
//}
/*
**函数名： FLASH_WriteMoreData 
**参数  ： startAddress 写地址 writeData 写数据   countToWrite 数量
**功能  ： FLASH 写
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void FLASH_WriteHalfWord(u32 startAddress, u16 data)
{
  if(startAddress<FLASH_BASE || (startAddress>=(FLASH_BASE+1024*FLASH_SIZE)))
  {
    return;
  }
  FLASH_Unlock();          //解锁FLASH

  FLASH_ProgramHalfWord(startAddress,data);

  FLASH_Lock();//锁定FLASH
}
/*
**函数名： FLASH_ReadHalfWord 
**参数  ： address 读地址 
**功能  ： FLASH 读半字
**日期	： 2019-02-26
**作者  ： 王瑞
*/
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}
/************************END OF FILE**********************/