#include "ctl.h"
#include "delay.h"


void Ctl_IO_Init(void) //
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); 
	//LED	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//参考 点频电源 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//LD_IO 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);
	Fre_PW_Set(0);						//频率源电源:打开
}

void T_Adc_Init(void)  
{
    ADC_InitTypeDef ADC_InitStructure; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1    , ENABLE );  
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    ADC_DeInit(ADC1);  
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure); 
    ADC_TempSensorVrefintCmd(ENABLE); 
    ADC_Cmd(ADC1, ENABLE);  
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1)); 
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));    
}
u16 T_Get_Adc(u8 ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );     
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);     
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
    return ADC_GetConversionValue(ADC1);  
}

u16 T_Get_Temp(void)
{
    u16 temp_val=0;
    u8 t;
    for(t=0;t<10;t++)
    {
        temp_val+=T_Get_Adc(ADC_Channel_16);      //TampSensor
    }
    return temp_val/10;
}


u16 T_Get_Adc_Average(u8 ch,u8 times)
{
    u32 temp_val=0;
    u8 t;
    for(t=0;t<times;t++)
    {
        temp_val+=T_Get_Adc(ch);
    }
    return temp_val/times;
}      


double Get_Temprate(void)    
{
    u32 adcx;
    double temperate;
    adcx=T_Get_Adc_Average(ADC_Channel_16,20); 
    temperate=(float)adcx*(3.3/4096);       
    temperate=(1.43-temperate)/0.0043+25;                     
    return temperate;
}
unsigned char Check_LD(void)//0锁定
{
	//全0为锁定
	unsigned char ld1=0,ld2=0,ld3=0,ld4=0;
	ld1 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
	ld2 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
	ld3 = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10);
	ld4 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
	return ld1|(ld2<<1)|(ld3<<2)|(ld4<<3);
}
void SoftReset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}
