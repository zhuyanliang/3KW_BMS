#include "include.h"

int16_t  g_CurrentOffset = 0; //电流的校准偏移
AdcRawTypedef g_AdcConvertValue; 

// ADC值与温度的对照表 AD值为mV
#if 0
const ThermLookupTypedef ThermLookupTable[] = 
{
   { 2992, -40 }, {  2969, -35 }, { 2940, -30 }, { 2904, -25 }, { 2861, -20 }, 
   { 2809, -15 }, {  2748, -10 }, { 2677,  -5 }, { 2596,   0 }, { 2503,   5 },
   { 2401,  10 }, { 2290,   15 }, { 2170,  20 }, { 2045,  25 }, { 1916,  30 }, 
   { 1784,  35 }, { 1651,   40 }, { 1520,  45 }, { 1392,  50 }, { 1269,  55 },
   { 1153,  60 }, { 1045,   65 }, { 944,   70 }, { 852,   75 }, { 767,   80 },
   { 691,   85 }, { 621,    90 }, { 559,   95 }, { 502,  100 }, { 452,  105 },
   { 407,  110 }, { 366,   115 }, { 330,  120 }, { 297,  125 },
};
#endif 
const ThermLookupTypedef ThermLookupTable[] = 
{
   { 2860, -40 }, { 2817, -35 }, { 2764, -30 }, { 2700, -25 }, { 2666, -20 }, 
   { 2537, -15 }, { 2437, -10 }, { 2324,  -5 }, { 2202,   0 }, { 2069,   5 },
   { 1930,  10 }, { 1787,   15 }, { 1643,  20 }, { 1500,  25 }, { 1362,  30 }, 
   { 1230,  35 }, { 1104,   40 }, { 987,   45 }, { 880,  50  }, { 782,  55 },
   { 694,   60 },  { 615,   65 }, { 545,   70 }, { 483,   75 }, { 429,   80 },
   { 380,   85 }, { 338,    90 }, { 300,   95 }, { 268,  100 }, { 238,  105 },
};


//ADC与温度值对照元素个数 
#define THERM_TABLE_NUM      ((uint8_t)30)

//============================================================================
// Function    : ADC_IOInit
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
void ADC_IOInit(void)
{
	//1.配置对应的管脚为模拟端口
    ANCON0 = 0x28;  //配置AN3,AN5
    ANCON1 = 0x01;  //配置AN8

    //set ad input pin direct as input
    TRISAbits.TRISA3 = 1;   //AN3  PCB板温度采集
    TRISBbits.TRISB1 = 1;   //AN8  检测电压是否为12V
    TRISEbits.TRISE0 = 1;   //AN5  电流检测

    ADCON1bits.VCFG 	= 0b00;     // ADC的正端参考电压选择 AVdd 
    ADCON1bits.VNCFG 	= 0b0;   	// ADC的负端参考电压选择 AVss
    ADCON1bits.CHSN 	= 0b0;    	// 模拟反相通道选择位 通道00（AVss） 

    ADCON2bits.ADFM 	= 0b1;   	// 右对齐
    ADCON2bits.ADCS 	= 0b110; 	// AD clock : Fosc/64, Tad=1us 
    ADCON2bits.ACQT 	= 0b010;  	// 采样时间为4个AD clock

    ADCON0bits.ADON 	= 0b1;     	// 使能A/D 转换器
}

//============================================================================
// Function    : ADC_Convert
// Description : This function start convert on the selected channel
// Parameters  : channel
// Returns     : none
//============================================================================
void ADC_Convert(uint8_t channel)
{
   ADCON0bits.CHS = channel;	// 设置AD的转换通道
   ADCON0bits.GO = 1; 			//启动ADC转换周期
}

//============================================================================
// Function    : ADC_GetCvtRaw
// Description : 该函数读取ADC转换寄存器的值
// Parameters  : none
// Returns     : ADRESH:ADRESL
//============================================================================
uint16_t ADC_GetCvtRaw(void)
{
	uint16_t adcRaw = 0;  

	adcRaw = ADRESH;
	adcRaw <<= 8;
	adcRaw |= ADRESL;
	adcRaw &= 0x0FFF;
	
	return adcRaw;
}

//============================================================================
// Function    : ADC_GetConvertVal
// Description : 该函数将AD转换值从寄存器读取并计算成电压，单位mv
// Parameters  : none
// Returns     : ADRESH:ADRESL
//============================================================================
uint16_t ADC_GetConvertVal(void)
{
	uint16_t adcRaw;
	uint32_t temp;   

	//adcRaw = ADRESL + ADRESH * 256;
	/*adcRaw = ((uint16_t)ADRESH << 8) | ADRESL;
	adcRaw &= 0x0FFF;*/
	adcRaw = ADRESH;
	adcRaw <<= 8;
	adcRaw |= ADRESL;
	adcRaw &= 0x0FFF;
	//精度是12位，参考电压5V  转换成电压值
	temp = ((uint32_t)adcRaw*5000) >> 12;

	return((uint16_t)temp);
}

//============================================================================
// Function    : ADC_AverageCalculate
// Description : 该函数完成一组采样数据的平均值处理，每组数据个数为固定8个。
// Parameters  : none
// Returns     : none
//============================================================================
uint16_t ADC_AverageCalculate(uint16_t *TableEntry)
{
   uint8_t i;
   uint32_t temp = 0;

   for (i=0; i<8; i++)
   {
      temp += *(TableEntry+i);
   }

   return ((uint16_t)(temp>>3));
}

//============================================================================
// Function    : CurrentZeroOffsetAdjust
// Description : 上电校准电流传感器零点
// Parameters  : none
// Returns     : none
//============================================================================
void CurrentZeroOffsetAdjust(void)
{
	uint8_t i, j;
	DelayMs(500);

	for(j=0; j<8; j++)
	{
		for (i=0; i<8; i++)
		{
			ADC_Convert(CHANNEL_CURRENT);  
			while(ADCON0bits.GO);  //等待转换完成，大约需要15us
			g_AdcConvertValue.Current[g_AdcConvertValue.CurIndex++ & 0x07] = ADC_GetCvtRaw();
		}

		g_AdcConvertValue.CurAvg = ADC_AverageCalculate(g_AdcConvertValue.Current);
		g_CurrentOffset += g_AdcConvertValue.CurAvg;
	}
	g_CurrentOffset = g_CurrentOffset/8;
}


//============================================================================
// Function    : ADC_CellTempCal
// Description : 根据ADC的值查表获取温度值
// Parameters  : DataAvg, ADC value 0-4096 1mv/LSB
// Returns     : Temp, temperature 1℃/LSB
//============================================================================
int8_t ADCToTempVal(uint16_t dat)
{
	uint8_t i = 1;
	int32_t temp;

	if (dat >= ThermLookupTable[0].AdcValue)
	{
		return(-40);
	}
	else if (dat <= ThermLookupTable[THERM_TABLE_NUM-1].AdcValue)
	{ 
		return(125);
	}    
	else // the temperature is within the table 
	{
		for (i=1; i<(THERM_TABLE_NUM-1); i++)
		{
			if (ThermLookupTable[i].AdcValue < dat)
			{         
				temp = ThermLookupTable[i-1].TempValue + (ThermLookupTable[i-1].AdcValue - dat) * 5 
						/ (ThermLookupTable[i-1].AdcValue -  ThermLookupTable[i].AdcValue);
				return((int8_t)temp);
			}
		}
	}

	return(0);
}


//============================================================================
// Function    : TskAdc_Init
// Description : ADC任务相关的参数初始化
// Parameters  : none
// Returns     : 
//============================================================================
void TskAdc_Init(void)
{
	g_AdcConvertValue.AmbTempIndex = 0;
	g_AdcConvertValue.CurIndex = 0;
}



