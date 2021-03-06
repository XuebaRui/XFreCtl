#include "HMC832.h"
//void HMC832_Write(unsigned long int data, unsigned char addr)	
//{
//	char i = 0 ;
//	addr = addr << 3;

//	SEN(1);
//	SCLK(1);
//	delay_us(10);
//	for(i = 0;i < 24; i++)
//	{
//		SCLK(0); //时钟低
//		delay_us(2);
//		if((( data >> (23 - i) ) & 0x01) > 0)
//		{
//			SDI(1);
//		}
//		else
//		{
//			SDI(0);
//		}	
//		delay_us(2);
//		SCLK(1);
//		delay_us(4);
//	}
//	SEN(0);
//	for(i = 0;i < 8; i++)
//	{
//		SCLK(0); //时钟低
//		delay_us(2);
//		if((( addr >> (7 - i) ) & 0x01) > 0)
//		{
//			SDI(1);
//		}
//		else
//		{
//			SDI(0);
//		}	
//		delay_us(2);
//		SCLK(1);
//		delay_us(4);
//	}
//	SEN(1);
//	delay_us(2);
//	SEN(0);
//}
void HMC832_Write(unsigned long int data, unsigned char addr)	
{
	char i = 0 ;
	
	data = ((addr<<24) | data)<<1;
	delay_us(10);
	SEN(0);
	delay_us(10);
	SEN(1);
	SDI(0);
	SCLK(0);
	delay_us(4);
	for(i = 0;i < 32; i++)
	{
		delay_us(2);
		SCLK(0); //时钟低
		delay_us(2);
		if((( data >> (31 - i) ) & 0x01) > 0)
		{
			SDI(1);
		}
		else
		{
			SDI(0);
		}	
		delay_us(2);
		SCLK(1);
		delay_us(2);
	}
	SCLK(0);
	SEN(0);
	delay_us(2);

}
//void HMC832_Write(unsigned long int WR_DAT,unsigned char ADD)
//{ 
//	 unsigned int m=0,j=0;
//	 unsigned long int temp1=0x800000;
//	 unsigned char temp2=0x20;
//	// SEN=0; 
//	 delay_us(1);
//	 delay_us(1);
//	 SEN(1); 
//	 SDI(0); 
//	 SCLK(0);
//	 delay_us(4);
//	 SCLK(1);
//	 for(m=0;m<6;m++)
//	 {
//	   SCLK(1);
//		 delay_us(2);
//	     if(( ADD&temp2)==0x0)
//		   SDI(0);
//		 else 
//		  SDI(1);
//		  delay_us(1);
//		  SCLK(0);
//		  delay_us(1);    
//		  temp2=temp2>>1;
// 	  }
//		delay_us(10); 
//	 for(j=0;j<24;j++)
//	   { 
//	   	SCLK(1);
//		  delay_us(1);
//	    if((WR_DAT&temp1)==0X0)
//	   	SDI(0);
//	  	else 
//		  SDI(1);
//		  delay_us(1);
//		  SCLK(0) ;
//		  delay_us(1);
//		  temp1=temp1>>1;
//   		}
//		 delay_us(10); 
//	 SCLK(1);
//	 delay_us(2);
//	 SCLK(0); 
//	 delay_us(2);
//	 SCLK(1);
//	 delay_us(2);
//	 SCLK(0);
//	 delay_us(2);
//	 SEN(0);
//	 delay_us(10);
//}

void GPIO_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 											   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		SEN(0);
		SEN(1);
		SEN(0);
		SCLK(0);
		SDI(0);
}

void HMC832_Init(void)
{
//	  // 2520MHZ
//		HMC832_Write(0x0002,0x01); 
//		HMC832_Write(0x0001,0x02);  //r = 2  PD = 100M /R  =50M   N = 2520 /50 = 50
//		HMC832_Write(0x0010,0x05); //输出VCO分频比为1
//		HMC832_Write(0x4d98,0x05); //
//		HMC832_Write(0x7d8b,0x05); //
//		HMC832_Write(0x0000,0x05); //
//		HMC832_Write(0x200b4a,0x06); //  xiaoshu moshi 
//		HMC832_Write(0x08cd,0x07); //  
//		HMC832_Write(0xc1beff,0x08); // 
//		HMC832_Write(0x1f7efd,0x09); // 
//		HMC832_Write(0x2006,0x0a); // 
//		HMC832_Write(0x0081,0x0f); //
//		HMC832_Write(0x0032,0x03); // 整数部分
//		HMC832_Write(0x00000,0x04); // 小数部分
	
		HMC832_Write(0x0002,0x01); 
				delay_ms(10);
		HMC832_Write(0x0002,0x02);  //r = 2  PD = 100M /R  =50M   N = 2520 /50 = 50
				delay_ms(10);
		HMC832_Write(0x1628,0x05); //输出VCO分频比为1
				delay_ms(10);
		HMC832_Write(0x60a0,0x05); //
				delay_ms(10);
		HMC832_Write(0xe090,0x05); //
				delay_ms(10);
		HMC832_Write(0x0000,0x05); //
				delay_ms(10);
		HMC832_Write(0x2003ca,0x06); //  xiaoshu moshi 
				delay_ms(10);
		HMC832_Write(0xccd,0x07); //  
				delay_ms(10);
		HMC832_Write(0xc1beff,0x08); // 
				delay_ms(10);
		HMC832_Write(0x3ffff,0x09); // 
				delay_ms(10);
		HMC832_Write(0x2006,0x0a); // 
				delay_ms(10);
		HMC832_Write(0x89,0x0f); //
				delay_ms(10);
		HMC832_Write(0x30,0x03); // 整数部分
				delay_ms(10);
		//HMC832_Write(0x00000,0x04); // 小数部分
		delay_ms(10);
}
