#include "func.h"
#include "nfre.h"
#include "hmc832.h"
/***** N5 N4 N3 N2 N1 N0 S1 S0**/
/* 真值表
**4.7  0  0  1  0  1  0  1  1          
**4.8  0  0  1  0  1  1  0  0 
**4.9	 0  0  1  0  1  1  0  1				
**5.0  0  0  1  0  1  1  1  0
**5.1  0  0  1  0  1  1  1  1
**5.2  0  0  1  1  0  0  0  0
**5.3  0  0  1  1  0  0  0  1
*/

void IO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //禁用JTAG 使能SWD
	//MGC IO口初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//att io 口初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//带宽选择IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void Fre_Ctl(u32 fre)
{
	fre = fre + 7750000;
	if(fre <= 8000000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8000000 - fre);
	}
	else if (fre <= 8200000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8200000 - fre);
	}
	else if (fre <= 8400000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8400000 - fre);
	}
	else if (fre <= 8600000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8600000 - fre);
	}
	else if (fre <= 8800000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8800000 - fre);
	}
	else if (fre <= 9000000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(9000000 - fre);
	}
}
void ATT_Ctl(u8 stc)
{
	M1dB((stc & 0x01) ? 0 : 1);
	M2dB((stc & 0x02) ? 0 : 1);
	M4dB((stc & 0x04) ? 0 : 1);
	M8dB((stc & 0x08) ? 0 : 1);
	M16dB((stc & 0x10) ? 0 : 1);
}
void CG_Ctl(u8 stc)
{
	A0_5dB(0);
	A1dB((stc & 0x01) ? 0 : 1);
	A2dB((stc & 0x02) ? 0 : 1);
	A4dB((stc & 0x04) ? 0 : 1);
	A8dB((stc & 0x08) ? 0 : 1);
	A16dB((stc & 0x10) ? 0 : 1);
	A32dB((stc & 0x20) ? 0 : 1);
}
void BW_Ctl(u8 bw) // 0  1  2 
{
	if(bw == 0)// 180mhz
	{
		U1_A(0);
		U1_B(0);
		U2_A(0);
		U2_B(0);
	}
	else if(bw == 1)// 240mhz
	{
		U1_A(1);
		U1_B(0);
		U2_A(1);
		U2_B(0);
	}
	else if(bw == 2)// 500mhz
	{
		U1_A(0);
		U1_B(1);
		U2_A(0);
		U2_B(1);
	}
	else
		;
}