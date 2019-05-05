//////////////////////////////////////////////////////////////////////////////////	 
 //ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
   //********************************************************************************
 
#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f10x.h"
#include "stdlib.h"	    	

#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column		0x3F			// 256/4-1
#define Max_Row		0x3F			// 64-1
#define	Brightness	0xFF 
#define	Shift		0x1C
	  
/************OLED�˿ڶ���**************/ 
#define SCLK_PORT 					GPIOC
#define SCLK_PIN 						GPIO_Pin_14
#define SDIN_PORT 					GPIOE
#define SDIN_PIN 						GPIO_Pin_6
#define OLED_DC_PORT 				GPIOE
#define OLED_DC_PIN 				GPIO_Pin_3
#define OLED_RST_PORT 			GPIOC
#define OLED_RST_PIN 				GPIO_Pin_13
#define OLED_CS_PORT 				GPIOE
#define OLED_CS_PIN 				GPIO_Pin_2
/************�ֿ�˿ڶ���**************/ 
#define ZK_CS_PORT 					GPIOE
#define ZK_CS_PIN 					GPIO_Pin_5
#define ZK_OUT_PORT 				GPIOE
#define ZK_OUT_PIN 					GPIO_Pin_4

#define SCLK_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_14)//oled���ֿ⹫��spiʱ��
#define SCLK_Set() GPIO_SetBits(GPIOC,GPIO_Pin_14)
 
#define SDIN_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_6)//oled���ֿ⹫��spi����
#define SDIN_Set() GPIO_SetBits(GPIOE,GPIO_Pin_6)	

#define OLED_DC_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_3)//oled ��������
#define OLED_DC_Set() GPIO_SetBits(GPIOE,GPIO_Pin_3)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_13)//oled RES
#define OLED_RST_Set() GPIO_SetBits(GPIOC,GPIO_Pin_13)

#define OLED_CS_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_2)//oledƬѡ
#define OLED_CS_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_2)

//----------------------------------------------------------------------------
#define ZK_CS_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_5)//�ֿ�Ƭѡ
#define ZK_CS_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_5)					
	
#define ZK_OUT_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_4)//�ֿ�spi�������
#define ZK_OUT_Set() GPIO_SetBits(GPIOE,GPIO_Pin_4)	
				
#define ZK_OUT()    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡָ���ܽŵ�����
//--------------------------------------------------------------------------
		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Init(void);
void Set_Column_Address(unsigned char a, unsigned char b);
void Set_Row_Address(unsigned char a, unsigned char b);
void Set_Write_RAM(void);
void Set_Read_RAM(void);
void Fill_RAM(unsigned char Data);
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e);
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Grayscale(void);
void Set_Remap_Format(unsigned char d);
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Con_4_byte(unsigned char DATA);
void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c);
void Set_Display_Offset(unsigned char d);
void Set_Start_Line(unsigned char d);
void Set_Master_Current(unsigned char d);
void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c);
void Set_Gray_Scale_Table(void);
void Set_Linear_Gray_Scale_Table(void); 

//Ӳ�ֿ⺯��
void GBZK_ROMOUTSET(void);		//�ֿ�������� 
void GBZK_ROMOUTRESET(void);	 	//�ֿ��������
void GBZK_GPIO_Config(void);	 //�ֿ�оƬ��ʼ��
void ZK_command( unsigned char  data );
unsigned char  get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen );
void Display_GB2312_String(unsigned char zk_num,unsigned int x,unsigned int y, unsigned char  text[]);	// ��ʾ�����ַ���
void Display_GB2312( unsigned char zk_num,unsigned int x,unsigned int y );								 //	��ʾ����
void Display_GBasc_String(unsigned char zk_num,unsigned int x,unsigned int y , unsigned char  text[]);	  // ��ʾGB2312 ascii �ַ���
void Display_GBasc( unsigned char zk_num,unsigned int x,unsigned int y  );								  //  ��ʾGB2312 ascii 
void Display_Asc_String(unsigned char zk_num,unsigned int x,unsigned int y , unsigned char  text[]);		  //   ��ʾASCII �ַ���
void Display_Asc( unsigned char zk_num,unsigned int x,unsigned int y);									  //   ��ʾASCII
#endif  
	 



