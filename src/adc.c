#include "include.h"

int16_t  g_CurrentOffset = 0; //������У׼ƫ��
AdcRawTypedef g_AdcConvertValue; 

// ADCֵ���¶ȵĶ��ձ� ADֵΪmV
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

//ADC���¶�ֵ����Ԫ�ظ��� 
#define THERM_TABLE_NUM      ((uint8_t)34)

//============================================================================
// Function    : ADC_IOInit
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
void ADC_IOInit(void)
{
	//1.���ö�Ӧ�Ĺܽ�Ϊģ��˿�
    ANCON0 = 0x28;  //����AN3,AN5
    ANCON1 = 0x01;  //����AN8

    //set ad input pin direct as input
    TRISAbits.TRISA3 = 1;   //AN3  PCB���¶Ȳɼ�
    TRISBbits.TRISB1 = 1;   //AN8  ����ѹ�Ƿ�Ϊ12V
    TRISEbits.TRISE0 = 1;   //AN5  �������

    ADCON1bits.VCFG 	= 0b00;     // ADC�����˲ο���ѹѡ�� AVdd 
    ADCON1bits.VNCFG 	= 0b0;   	// ADC�ĸ��˲ο���ѹѡ�� AVss
    ADCON1bits.CHSN 	= 0b0;    	// ģ�ⷴ��ͨ��ѡ��λ ͨ��00��AVss�� 

    ADCON2bits.ADFM 	= 0b1;   	// �Ҷ���
    ADCON2bits.ADCS 	= 0b110; 	// AD clock : Fosc/64, Tad=1us 
    ADCON2bits.ACQT 	= 0b010;  	// ����ʱ��Ϊ4��AD clock

    ADCON0bits.ADON 	= 0b1;     	// ʹ��A/D ת����
}

//============================================================================
// Function    : ADC_Convert
// Description : This function start convert on the selected channel
// Parameters  : channel
// Returns     : none
//============================================================================
void ADC_Convert(uint8_t channel)
{
   ADCON0bits.CHS = channel;	// ����AD��ת��ͨ��
   ADCON0bits.GO = 1; 			//����ADCת������
}

//============================================================================
// Function    : ADC_GetCvtRaw
// Description : �ú�����ȡADCת���Ĵ�����ֵ
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
// Description : �ú�����ADת��ֵ�ӼĴ�����ȡ������ɵ�ѹ����λmv
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
	//������12λ���ο���ѹ5V  ת���ɵ�ѹֵ
	temp = ((uint32_t)adcRaw*5000) >> 12;

	return((uint16_t)temp);
}

//============================================================================
// Function    : ADC_AverageCalculate
// Description : �ú������һ��������ݵ�ƽ��ֵ����ÿ�����ݸ���Ϊ�̶�8����
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
// Description : �ϵ�У׼�������������
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
			while(ADCON0bits.GO);  //�ȴ�ת����ɣ���Լ��Ҫ15us
			g_AdcConvertValue.Current[g_AdcConvertValue.CurIndex++ & 0x07] = ADC_GetCvtRaw();
		}

		g_AdcConvertValue.CurAvg = ADC_AverageCalculate(g_AdcConvertValue.Current);
		g_CurrentOffset += g_AdcConvertValue.CurAvg;
	}
	g_CurrentOffset = g_CurrentOffset/8;
}


//============================================================================
// Function    : ADC_CellTempCal
// Description : ����ADC��ֵ����ȡ�¶�ֵ
// Parameters  : DataAvg, ADC value 0-4096 1mv/LSB
// Returns     : Temp, temperature 1��/LSB
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
// Description : ADC������صĲ�����ʼ��
// Parameters  : none
// Returns     : 
//============================================================================
void TskAdc_Init(void)
{
	g_AdcConvertValue.AmbTempIndex = 0;
	g_AdcConvertValue.CurIndex = 0;
}



