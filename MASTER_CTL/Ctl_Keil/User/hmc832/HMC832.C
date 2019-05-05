#include "HMC832.h"
#include "func.h"
/********************HMC832A*******************/
/*
**函数名： HMC832A_GPIOInit
**参数  ： 无
**功能  ： HMC832控制IO初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
static void HMC832A_GPIOInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 											   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
		SENA(0);
		SENA(1);
		SENA(0);
		SCLKA(0);
		SDIA(0);
}
/*
**函数名： HMC832A_Write
**参数  ： int data 写hmc832的数据   char addr hmc832地址
**功能  ： HMC832 写指令
**日期	： 2019-02-26
**作者  ： 王瑞
*/
static void HMC832A_Write(unsigned long int data, unsigned char addr)	
{
	char i = 0 ;
	
	data = ((addr<<24) | data)<<1;
	delay_us(10);
	SENA(0);
	delay_us(10);
	SENA(1);
	SDIA(0);
	SCLKA(0);
	delay_us(4);
	for(i = 0;i < 32; i++)
	{
		delay_us(2);
		SCLKA(0); //时钟低
		delay_us(2);
		if((( data >> (31 - i) ) & 0x01) > 0)
		{
			SDIA(1);
		}
		else
		{
			SDIA(0);
		}	
		delay_us(2);
		SCLKA(1);
		delay_us(2);
	}
	SCLKA(0);
	SENA(0);
	delay_us(2);
}
/*
**函数名： HMC832A_Init
**参数  ： 无
**功能  ： HMC832 芯片初始化  鉴相频率 VCO 设置 初始化频点等
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void HMC832A_Init(void)
{
	HMC832A_GPIOInit();
	HMC832A_Write(0x0001,0x01); 
	HMC832A_Write(0x0002,0x02);  	//r = 2  PD = 100M /R  =50M   N = 2520 /50 = 50
	HMC832A_Write(0x0110,0x05); 		//输出VCO分频比为1
	HMC832A_Write(0x4B98,0x05); 
	HMC832A_Write(0x7D8B,0x05); 
	HMC832A_Write(0x0000,0x05); 
	HMC832A_Write(0x200b4a,0x06); 	// 小数模式 
	HMC832A_Write(0x8cd,0x07);  
	HMC832A_Write(0xc1beff,0x08);  
	HMC832A_Write(0x1f7efd,0x09);  
	HMC832A_Write(0x2006,0x0a);  
	HMC832A_Write(0x81,0x0f); 
	HMC832A_Write(0x27,0x03); 			// 整数部分
	HMC832A_Write(0x1eb851,0x04); // 小数部分
	delay_us(50);
}
/*
**函数名： HMC832A_FreSet
**参数  ： fre 32位频率值对应xxx.xxx * 1000
**功能  ： HMC832 芯片初始化  鉴相频率 VCO 设置 初始化频点等
**日期	： 2019-02-26
**作者  ： 王瑞
*/
u8 HMC832A_FreSet(double fre)
{
	u8 retry = 0;
	u32 freN = (u32)(fre * 1000); //频率改成整数
	freN = (u32)(((freN - 1698000 + 3000.0)/25000.0) * 16777216);
	HMC832A_Write(freN ,0x04); 			// 小数部分
	while(FA_Check() != 11)
	{
		retry ++;
		if(retry >= 200)
			return 0;
	}
	return 1;
}
/********************HMC832B*******************/
/*
**函数名： HMC832B_GPIOInit
**参数  ： 无
**功能  ： HMC832控制IO初始化
**日期	： 2019-02-26
**作者  ： 王瑞
*/
static void HMC832B_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 											   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	SENB(0);
	SENB(1);
	SENB(0);
	SCLKB(0);
	SDIB(0);
}
/*
**函数名： HMC832B_Write
**参数  ： int data 写hmc832的数据   char addr hmc832地址
**功能  ： HMC832 写指令
**日期	： 2019-02-26
**作者  ： 王瑞
*/
static void HMC832B_Write(unsigned long int data, unsigned char addr)	
{
	char i = 0 ;
	
	data = ((addr<<24) | data)<<1;
	delay_us(10);
	SENB(0);
	delay_us(10);
	SENB(1);
	SDIB(0);
	SCLKB(0);
	delay_us(4);
	for(i = 0;i < 32; i++)
	{
		delay_us(2);
		SCLKB(0); //时钟低
		delay_us(2);
		if((( data >> (31 - i) ) & 0x01) > 0)
		{
			SDIB(1);
		}
		else
		{
			SDIB(0);
		}	
		delay_us(2);
		SCLKB(1);
		delay_us(2);
	}
	SCLKB(0);
	SENB(0);
	delay_us(2);
}
/*
**函数名： HMC832B_Init
**参数  ： 无
**功能  ： HMC832 芯片初始化  鉴相频率 VCO 设置 初始化频点等
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void HMC832B_Init(void)
{
	HMC832B_GPIOInit();
	HMC832B_Write(0x0001,0x01); 
	HMC832B_Write(0x0002,0x02);  	//r = 2  PD = 100M /R  =50M   N = 2520 /50 = 50
	HMC832B_Write(0x0110,0x05); 		//输出VCO分频比为1
	HMC832B_Write(0x4B98,0x05); 
	HMC832B_Write(0x7D8B,0x05); 
	HMC832B_Write(0x0000,0x05); 
	HMC832B_Write(0x200b4a,0x06); 	// 小数模式 
	HMC832B_Write(0x8cd,0x07);  
	HMC832B_Write(0xc1beff,0x08);  
	HMC832B_Write(0x1f7efd,0x09);  
	HMC832B_Write(0x2006,0x0a);  
	HMC832B_Write(0x81,0x0f); 
	HMC832B_Write(0x27,0x03); 			// 整数部分
	HMC832B_Write(0x1eb851,0x04); // 小数部分
	delay_us(50);
}
/*
**函数名： HMC832B_FreSet
**参数  ： fre 32位频率值对应xxx.xxx
**功能  ： HMC832 芯片初始化  鉴相频率 VCO 设置 初始化频点等
**日期	： 2019-02-26
**作者  ： 王瑞
*/
u8 HMC832B_FreSet(double fre)
{
	u8 retry = 0;
	u32 freN = (u32)(fre * 1000); //频率改成整数
	freN = (u32)(((freN - 1698000 + 3000.0)/25000.0) * 16777216);
	HMC832B_Write(freN ,0x04); 			// 小数部分
	while(FA_Check() != 11)
	{
		retry ++;
		if(retry >= 200)
			return 0;
	}
	return 1;
}
/************************END OF FILE**********************/