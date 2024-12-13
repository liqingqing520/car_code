#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"


#define Sample_Num 50
#define Channel_Num 3

//extern __IO uint16_t ADC_ConvertedValue[Sample_Num][Channel_Num];
//extern u16 AD_After_Filter[Channel_Num];

void Gray_Init(void);
u16 Gray_GetVolt(u16 advalue);

#endif  /* __ADC_H */
