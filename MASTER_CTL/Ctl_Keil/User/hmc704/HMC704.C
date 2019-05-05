#include "HMC704.h"
void HMC704_Write(unsigned long int data ,unsigned char channel)	
{
	char i = 0 ;
	data = data<<1;

	delay_us(10);
	if(channel == 0x01)
	{
		SEN_1(0);
		delay_us(10);
		SEN_1(1);
	}
	else if(channel == 0x02)
	{
		SEN_2(0);
		delay_us(10);
		SEN_2(1);	
	}
	else
	{
		SEN_1(0);
		SEN_2(0);
		delay_us(10);
		SEN_1(1);
		SEN_2(1);	
	}


	SCLK(0); //Ê±ÖÓµÍ
	SDI(0);
	for(i = 0;i < 32; i++)
	{
		if(((data>>(31-i)) & 0x01) > 0)
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
		SCLK(0);
	}
	SEN_1(0);
	SEN_2(0);
	delay_us(10);
}


void HMC704_OpenModeWrite(unsigned long int data ,unsigned char channel)	
{
	char i = 0 ;
	unsigned char reg = 0;
	reg = (data>>24) &0x1f;
	data = ((data<<8)|(reg<<3)) << 1;
	delay_us(10);
	
	if(channel == 0x01)
	{
		SEN_1(0);
	}
	else if(channel == 0x02)
	{
		SEN_2(0);
	}
	else
	{
		SEN_1(0);
		SEN_2(0);
	}
	SDI(0);
	SCLK(0); 
	delay_us(10);
	SCLK(1); 

	for(i = 0;i < 32; i++)
	{
		delay_us(2);
		SCLK(0);
		if(((data >> (31-i)) & 0x01) > 0)
		{
			SDI(1);
		}
		else
		{
			SDI(0);
		}
		delay_us(2);
		SCLK(1);
	}
	delay_us(2);
	if(channel == 0x01)
	{
		SEN_1(1);
	}
	else if(channel == 0x02)
	{
		SEN_2(1);
	}
	else
	{
		SEN_1(1);
		SEN_2(1);
	}
	delay_us(1);
	if(channel == 0x01)
	{
		SEN_1(0);
	}
	else if(channel == 0x02)
	{
		SEN_2(0);
	}
	else
	{
		SEN_1(0);
		SEN_2(0);
	}
	delay_us(1);
	SDI(0);
	SCLK(0); 
	delay_us(10);
}
void HMC704_GPIO_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); 											   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_8;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);													   
		GPIO_ResetBits(GPIOA, GPIO_Pin_11|GPIO_Pin_8);	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);													   
		GPIO_ResetBits(GPIOB, GPIO_Pin_7|GPIO_Pin_8);
}

void HMC704_Init(void)
{
		HMC704_Write(0x01000002,0x03);
		HMC704_Write(0x02000002,0x03);
		HMC704_Write(0x04000000,0x03);
		HMC704_Write(0x05000005,0x03);
		HMC704_Write(0x06200b4a,0x03);
		HMC704_Write(0x0700024D,0x03);
		HMC704_Write(0x0809BEFF,0x03);
		HMC704_Write(0x092f3fff,0x03);
		HMC704_Write(0x0a000801,0x03);
		HMC704_Write(0x0b078071,0x03);
		HMC704_Write(0x0c000000,0x03);
		HMC704_Write(0x0d000000,0x03);
		HMC704_Write(0x0e000000,0x03);
		HMC704_Write(0x0f0000c1,0x03);
		HMC704_Write(0x0300007D+6,0x03);
		HMC704_Write(0x04000000|(unsigned long int)(209.7152*20000),0x03);//500hz 167.77216
		delay_ms(1);
		HMC704_OpenModeWrite(0x01000002,0x03);
		HMC704_OpenModeWrite(0x02000002,0x03);

		HMC704_OpenModeWrite(0x04000000,0x03);
		HMC704_OpenModeWrite(0x05000005,0x03);
		HMC704_OpenModeWrite(0x06200b4a,0x03);
		HMC704_OpenModeWrite(0x0700024D,0x03);
		HMC704_OpenModeWrite(0x0809BEFF,0x03);
		HMC704_OpenModeWrite(0x092f3fff,0x03);
		HMC704_OpenModeWrite(0x0a000801,0x03);
		HMC704_OpenModeWrite(0x0b078071,0x03);
		HMC704_OpenModeWrite(0x0c000000,0x03);
		HMC704_OpenModeWrite(0x0d000000,0x03);
		HMC704_OpenModeWrite(0x0e000000,0x03);
		HMC704_OpenModeWrite(0x0f0000c1,0x03);
		HMC704_OpenModeWrite(0x0300007D+6,0x03);
		HMC704_OpenModeWrite(0x04000000|(unsigned long int)(209.7152*20000),0x03);//500hz 167.77216
}
void Fre_Set(u16 fre_h, u32 fre_l,u8 sel)
{
	uint64_t fre =  (((uint64_t)fre_h << 32 ) | fre_l)+20000;
	u16 N = (fre*500)/40000000;
	u32 frac =(u32)((((fre*500)%40000000)/40000000.0)*16777216);

	HMC704_Write(0x0300007D+6+N,sel);
	HMC704_Write(0x04000000+frac,sel);
	delay_ms(1);
	HMC704_Write(0x0300007D+6+N,sel);
	HMC704_Write(0x04000000+frac,sel);
}
