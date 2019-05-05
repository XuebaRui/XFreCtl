#include "HMC832.h"
#include "func.h"
/********************HMC832A*******************/
/*
**�������� HMC832A_GPIOInit
**����  �� ��
**����  �� HMC832����IO��ʼ��
**����	�� 2019-02-26
**����  �� ����
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
**�������� HMC832A_Write
**����  �� int data дhmc832������   char addr hmc832��ַ
**����  �� HMC832 дָ��
**����	�� 2019-02-26
**����  �� ����
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
		SCLKA(0); //ʱ�ӵ�
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
**�������� HMC832A_Init
**����  �� ��
**����  �� HMC832 оƬ��ʼ��  ����Ƶ�� VCO ���� ��ʼ��Ƶ���
**����	�� 2019-02-26
**����  �� ����
*/
void HMC832A_Init(void)
{
	HMC832A_GPIOInit();
	HMC832A_Write(0x0001,0x01); 
	HMC832A_Write(0x0002,0x02);  	//r = 2  PD = 100M /R  =50M   N = 2520 /50 = 50
	HMC832A_Write(0x0110,0x05); 		//���VCO��Ƶ��Ϊ1
	HMC832A_Write(0x4B98,0x05); 
	HMC832A_Write(0x7D8B,0x05); 
	HMC832A_Write(0x0000,0x05); 
	HMC832A_Write(0x200b4a,0x06); 	// С��ģʽ 
	HMC832A_Write(0x8cd,0x07);  
	HMC832A_Write(0xc1beff,0x08);  
	HMC832A_Write(0x1f7efd,0x09);  
	HMC832A_Write(0x2006,0x0a);  
	HMC832A_Write(0x81,0x0f); 
	HMC832A_Write(0x27,0x03); 			// ��������
	HMC832A_Write(0x1eb851,0x04); // С������
	delay_us(50);
}
/*
**�������� HMC832A_FreSet
**����  �� fre 32λƵ��ֵ��Ӧxxx.xxx * 1000
**����  �� HMC832 оƬ��ʼ��  ����Ƶ�� VCO ���� ��ʼ��Ƶ���
**����	�� 2019-02-26
**����  �� ����
*/
u8 HMC832A_FreSet(double fre)
{
	u8 retry = 0;
	u32 freN = (u32)(fre * 1000); //Ƶ�ʸĳ�����
	freN = (u32)(((freN - 1698000 + 3000.0)/25000.0) * 16777216);
	HMC832A_Write(freN ,0x04); 			// С������
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
**�������� HMC832B_GPIOInit
**����  �� ��
**����  �� HMC832����IO��ʼ��
**����	�� 2019-02-26
**����  �� ����
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
**�������� HMC832B_Write
**����  �� int data дhmc832������   char addr hmc832��ַ
**����  �� HMC832 дָ��
**����	�� 2019-02-26
**����  �� ����
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
		SCLKB(0); //ʱ�ӵ�
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
**�������� HMC832B_Init
**����  �� ��
**����  �� HMC832 оƬ��ʼ��  ����Ƶ�� VCO ���� ��ʼ��Ƶ���
**����	�� 2019-02-26
**����  �� ����
*/
void HMC832B_Init(void)
{
	HMC832B_GPIOInit();
	HMC832B_Write(0x0001,0x01); 
	HMC832B_Write(0x0002,0x02);  	//r = 2  PD = 100M /R  =50M   N = 2520 /50 = 50
	HMC832B_Write(0x0110,0x05); 		//���VCO��Ƶ��Ϊ1
	HMC832B_Write(0x4B98,0x05); 
	HMC832B_Write(0x7D8B,0x05); 
	HMC832B_Write(0x0000,0x05); 
	HMC832B_Write(0x200b4a,0x06); 	// С��ģʽ 
	HMC832B_Write(0x8cd,0x07);  
	HMC832B_Write(0xc1beff,0x08);  
	HMC832B_Write(0x1f7efd,0x09);  
	HMC832B_Write(0x2006,0x0a);  
	HMC832B_Write(0x81,0x0f); 
	HMC832B_Write(0x27,0x03); 			// ��������
	HMC832B_Write(0x1eb851,0x04); // С������
	delay_us(50);
}
/*
**�������� HMC832B_FreSet
**����  �� fre 32λƵ��ֵ��Ӧxxx.xxx
**����  �� HMC832 оƬ��ʼ��  ����Ƶ�� VCO ���� ��ʼ��Ƶ���
**����	�� 2019-02-26
**����  �� ����
*/
u8 HMC832B_FreSet(double fre)
{
	u8 retry = 0;
	u32 freN = (u32)(fre * 1000); //Ƶ�ʸĳ�����
	freN = (u32)(((freN - 1698000 + 3000.0)/25000.0) * 16777216);
	HMC832B_Write(freN ,0x04); 			// С������
	while(FA_Check() != 11)
	{
		retry ++;
		if(retry >= 200)
			return 0;
	}
	return 1;
}
/************************END OF FILE**********************/