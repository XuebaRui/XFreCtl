#ifndef __adc_H
#define __adc_H

#include "stm32f10x.h"

#define  RF_2     5
#define  RF_1     4

void adc_init(void);
double ADC_GetVal(u8 ch);
double ADC_GetVal_OverSample(u8 resolution);//·Ö±æÂÊ 12 13 14 15 16
double widget_filter(u8 width);

#endif
