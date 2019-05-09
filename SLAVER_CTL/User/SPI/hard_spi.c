#include "hard_spi.h"


/*
**函数名： Spi1_Init 
**参数  ： 无
**功能  ： 主模式 全双工8位
**日期	： 2019-04-30
**作者  ： 王瑞
*/
void Spi1_Init(void) //主模式 全双工8位
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);


	//SCK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	//CS
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上啦
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	//spi中断 
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	SPI_I2S_ITConfig (SPI1,SPI_I2S_IT_RXNE, ENABLE);
  SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
	SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_TXE);
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE);
}
/*
**函数名： Spi1_SendByte 
**参数  ： 无
**功能  ： 发送一个字节
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void Spi1_SendByte(u8 byte)
{
	u8 retry= 0;  
	while((SPI1->SR&1<<1)==0)
	{
		retry++;
		if(retry>20)break; 
	}
	SPI1->DR = byte;
}
typedef struct 
{
	u32  cf;
	u8 cg;
	u8 att;
	u8 bw;
	u8 agc;
}Sys_Para; //系统参数
u8 Spi1_RecBuff[7] = {0}; //主机数据接收
u8 Spi1_Reclen = 0;
u8 Spi1_RecFinish = 0;
u8 rtn_buf[6] = {0};
u16 cmd_cnt = 40001;
void SPI1_IRQHandler(void)
{
	u8 tmp = 0;
	u8 retry = 0;
	if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE) != RESET)
	{
		if(Spi1_Reclen < 7 && !Spi1_RecFinish)
		{
			cmd_cnt = 0;
			Spi1_RecBuff[Spi1_Reclen++] = SPI1->DR;
			if(Spi1_RecBuff[0] == 0x5b ) //查询指令
			{
					if(Spi1_Reclen < 7)    
						SPI1->DR = rtn_buf[Spi1_Reclen - 1];
					else
					{
						SPI1->DR = 0 ;
					}
			}
			if(Spi1_Reclen == 7)
			{
				if(Spi1_RecBuff[6] == 0xa5 || Spi1_RecBuff[6] == 0xb5 )
				{
					Spi1_RecFinish = 1; //从机返回成功
					cmd_cnt = 40001;
				}
			}
		}
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
	}
}
