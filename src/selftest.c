
#include "include.h"

static int16_t cellVoltTest[ModuleAmount][12] = {0};
static uint8_t ltc6803TestBuf[18] = {0};


//============================================================================
// Function    ��SystemSelftest
// Description ��ϵͳ�ϵ��Լ�
// Parameters  ��none 
// Returns     ��none
//============================================================================ 
uint8_t SystemSelftest(void)
{

	// ltc6803�Լ�
	//Ltc6803_Selftest();

	// ����߿�·���
	//Ltc6803_OpenWireTest();
	// cell��ѹ�Լ�
	CellVoltSelftest();

	// ϵͳ�¶��Լ�
	//CellTempSelftest();
	//AmbTempSelftest();

	// pack��ѹ�Լ�
	if (g_SystemError.ltc_st || g_SystemError.ltc_com || g_SystemError.det_oc)
	{
		if (g_ProtectDelayCnt > RELAY_ACTION_DELAY_1S)
		{
			g_ProtectDelayCnt = RELAY_ACTION_DELAY_1S;
		}
	}   

	return 0;
}



//============================================================================
// Function    : Ltc6803_Selftest
// Description : ltc6803оƬ�Լ�
// Parameters  : none
// Returns     : none
//============================================================================
void Ltc6803_Selftest(void)
{
	
}



//============================================================================
// Function    : Ltc6803_OpenWireTest
// Description : ltc6803оƬ����߿�·���
// Parameters  : none
// Returns     : none
//============================================================================
void Ltc6803_OpenWireTest(void)
{

}



//============================================================================
// Function    : CellVoltSelftest
// Description : ��о��ѹ�Լ�
// Parameters  : none
// Returns     : none
//============================================================================
void CellVoltSelftest(void)
{
	
}



//============================================================================
// Function    : CellTempSelftest
// Description : �ϵ��Լ쵥���¶�
// Parameters  : none
// Returns     : none
//============================================================================
void CellTempSelftest(void)
{
	
}




//============================================================================
// Function    : CellTempSelftest
// Description : �ϵ��Լ��·���¶�
// Parameters  : none
// Returns     : none
//============================================================================
void AmbTempSelftest(void)
{
	uint8_t i;

	g_AdcConvertValue.AmbTempIndex = 0;

	for (i=0; i<8; i++)
	{
		ADC_Convert(CHANNEL_TBAVAL);  
		while(ADCON0bits.GO);  //�ȴ�ת����ɣ���Լ��Ҫ15us
		g_AdcConvertValue.AmbTempRaw[g_AdcConvertValue.AmbTempIndex++] = ADC_GetConvertVal();
	}

	// �ɼ���һ��ͼ���ƽ��ֵ
	g_AdcConvertValue.AmbTempAvg = ADC_AverageCalculate(g_AdcConvertValue.AmbTempRaw); 
	g_AdcConvertValue.AmbTempIndex = 0;

	g_BatteryParameter.AmbientTemp = ADCToTempVal(g_AdcConvertValue.AmbTempAvg);

	if (g_BatteryParameter.AmbientTemp > 85)
	{
		g_SystemError.pcb_ot = 1;
	}
}








