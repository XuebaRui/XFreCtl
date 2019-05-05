#include "adc.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}
static void BSP_adc_init(void)
{

	ADC_InitTypeDef ADC_InitStruct;
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; //独立模式
  ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; //连续转换
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; //数据右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //触发方式
	
	ADC_InitStruct.ADC_NbrOfChannel = 1;    //使用通道数
	ADC_InitStruct.ADC_ScanConvMode = DISABLE; //扫描模式
	ADC_Init(ADC1, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5); //55.5个采样周期+12.5个转换周期 转换时间为 (55.5+12.5)*(1/9mhz) 7.6us
	
	ADC_Cmd(ADC1,ENABLE);
	
		
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_GetVal(5);
}

void adc_init(void)
{
	
	ADC1_GPIO_Config();
  BSP_adc_init();

}

double ADC_GetVal(u8 ch )//12位精度电压
{
		double adc_val[5];
		int i = 0,j=0;
		double tmp = 0;
		ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_13Cycles5);
		for(j=0;j<5;j++)
		{
			for(i = 0;i<3;i++)
			{
				ADC1->CR2 |= ((uint32_t)0x00500000);
				while(!(ADC1->SR & ((uint8_t)0x02)));
				tmp += ADC1->DR;
			}
			adc_val[j] = tmp/3;
			tmp = 0;
		}
		for(i = 0;i<4;i++)
		{
			for(j=0;j<3;j++)
			{
				tmp = adc_val[j];
				adc_val[j]= adc_val[j+1];
				adc_val[j+1] = tmp;
			}
		}
		return	3.3/4096 * adc_val[2];
}

double ADC_GetOverSampleval(u8 resolution)//分辨率 12 13 14 15 16 过采样电压值
{
	u32 adc_val = 0;
	u16 i= 0;
	if(resolution>16 || resolution <12)
	__breakpoint(0);
	for(i = 0; i <= 255; i++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		adc_val += ADC_GetConversionValue(ADC1);
	}
	return (double)adc_val / (256 / 2^(resolution - 12));
}

double widget_filter(u8 width) //窗口滤波
{	
	unsigned int* widget_num;
	char i = 0;
	u16 _tmp = 0;
	u32 _tmp1 = 0; 
	
	widget_num = malloc(sizeof(double)*20);

	memset(widget_num,0,sizeof(double)*20);
	
	_tmp = ADC_GetVal(4); //将16位分辨率的进行串口滤波

	for(i = width - 1; i >= 1; i--)
	{
		widget_num[i] =widget_num[i-1];		
	}
	widget_num[0] = _tmp;
	for(i = 0 ;i <= width - 1; i++)
	{
		_tmp1 += widget_num[i];
	}
	
	return (double)_tmp1 / width;
}



