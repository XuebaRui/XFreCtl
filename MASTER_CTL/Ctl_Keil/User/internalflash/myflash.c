#include "myflash.h"


/*
**�������� FLASH_WriteMoreData 
**����  �� startAddress д��ַ writeData д����   countToWrite ����
**����  �� FLASH д
**����	�� 2019-02-26
**����  �� ����
*/
//void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
//{
//  if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE)))
//  {
//    return;
//  }
//  FLASH_Unlock();          //����FLASH
//  uint32_t offsetAddress=startAddress-FLASH_BASE;                //��Ի���ַ��ƫ��
//  uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;             //��������λ��
//  
//  uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;   //�����׵�ַ

//  FLASH_ErasePage(sectorStartAddress);                           //��������
//  
//  for(dataIndex=0;dataIndex<countToWrite;dataIndex++)           //����д��
//  {
//    FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
//  }
//  FLASH_Lock();//����FLASH
//}
/*
**�������� FLASH_WriteMoreData 
**����  �� startAddress д��ַ writeData д����   countToWrite ����
**����  �� FLASH д
**����	�� 2019-02-26
**����  �� ����
*/
void FLASH_WriteHalfWord(u32 startAddress, u16 data)
{
  if(startAddress<FLASH_BASE || (startAddress>=(FLASH_BASE+1024*FLASH_SIZE)))
  {
    return;
  }
  FLASH_Unlock();          //����FLASH

  FLASH_ProgramHalfWord(startAddress,data);

  FLASH_Lock();//����FLASH
}
/*
**�������� FLASH_ReadHalfWord 
**����  �� address ����ַ 
**����  �� FLASH ������
**����	�� 2019-02-26
**����  �� ����
*/
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}
/************************END OF FILE**********************/