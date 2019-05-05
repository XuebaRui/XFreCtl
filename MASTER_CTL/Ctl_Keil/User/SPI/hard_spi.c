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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);


	//SCK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上啦
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	//spi中断
//	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE, ENABLE);
//   SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_RXNE);
//	 SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_TXE);
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
/*
**函数名： Slaver_SendCmd(u8 buffer[]) 
**参数  ： 无
**功能  ： 发送一个字节
**日期	： 2019-02-26
**作者  ： 王瑞
*/
void SPI_Send(u8 fre,u8 ref,u8 rec_data[3])
{	
	u8 retry= 0;
	while((SPI1->SR&1<<1)==0)
	{
		retry++;
		if(retry>20)break; 
	}
	retry = 0;
	SPI1->DR = 0X5A;
	while((SPI1->SR&1<<0)==0)
	{
	 retry++;
				if(retry>20)break;		
	}	
	SPI1->DR;
	retry = 0;
	while((SPI1->SR&1<<1)==0)
	{
			 retry++;
				if(retry>20)break; 
	}
	retry = 0;
	//Delay_us(10);  
	SPI1->DR = fre;
	while((SPI1->SR&1<<0)==0)
	{
	 retry++;
				if(retry>20)break;		
	}
	retry = 0;
	//Delay_us(10);
	rec_data[0] = SPI1->DR;
	while((SPI1->SR&1<<1)==0)
	{
			 retry++;
				if(retry>20)break; 
	}
	retry = 0;
	
	SPI1->DR = ref;
	while((SPI1->SR&1<<0)==0)
	{
	 retry++;
				if(retry>20)break;		
	}
	retry = 0;
	//Delay_us(10);
	rec_data[1] = SPI1->DR;
	while((SPI1->SR&1<<1)==0)
	{
	
			 retry++;
				if(retry>20)break; 
	}
	retry = 0;
	SPI1->DR = 0Xf5;
	while((SPI1->SR&1<<0)==0)
	{
	 retry++;
		if(retry>20)break;		
	}
	retry = 0;
	//Delay_us(10);
	rec_data[2] = SPI1->DR;
}

/* SPI接收 中断
** 帧结构 
**         头  数据      尾
*/
void SPI1_IRQHandler(void)
{
  u8 tmp = 0;
	if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE) != RESET) // cs ↓
	{
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
	}
}


