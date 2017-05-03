
#include "include.h"

static int16_t cellVoltTest[ModuleAmount][12] = {0};
static uint8_t ltc6803TestBuf[18] = {0};


//============================================================================
// Function    ：SystemSelftest
// Description ：系统上电自检
// Parameters  ：none 
// Returns     ：none
//============================================================================ 
uint8_t SystemSelftest(void)
{

	// ltc6803自检
	//Ltc6803_Selftest();

	// 检测线开路检查
	//Ltc6803_OpenWireTest();
	// cell电压自检
	CellVoltSelftest();

	// 系统温度自检
	//CellTempSelftest();
	//AmbTempSelftest();

	// pack总压自检
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
// Description : ltc6803芯片自检
// Parameters  : none
// Returns     : none
//============================================================================
void Ltc6803_Selftest(void)
{
	
}



//============================================================================
// Function    : Ltc6803_OpenWireTest
// Description : ltc6803芯片检测线开路检查
// Parameters  : none
// Returns     : none
//============================================================================
void Ltc6803_OpenWireTest(void)
{

}



//============================================================================
// Function    : CellVoltSelftest
// Description : 电芯电压自检
// Parameters  : none
// Returns     : none
//============================================================================
void CellVoltSelftest(void)
{
	
}



//============================================================================
// Function    : CellTempSelftest
// Description : 上电自检单体温度
// Parameters  : none
// Returns     : none
//============================================================================
void CellTempSelftest(void)
{
	
}




//============================================================================
// Function    : CellTempSelftest
// Description : 上电自检电路板温度
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
		while(ADCON0bits.GO);  //等待转换完成，大约需要15us
		g_AdcConvertValue.AmbTempRaw[g_AdcConvertValue.AmbTempIndex++] = ADC_GetConvertVal();
	}

	// 采集满一组就计算平均值
	g_AdcConvertValue.AmbTempAvg = ADC_AverageCalculate(g_AdcConvertValue.AmbTempRaw); 
	g_AdcConvertValue.AmbTempIndex = 0;

	g_BatteryParameter.AmbientTemp = ADCToTempVal(g_AdcConvertValue.AmbTempAvg);

	if (g_BatteryParameter.AmbientTemp > 85)
	{
		g_SystemError.pcb_ot = 1;
	}
}








