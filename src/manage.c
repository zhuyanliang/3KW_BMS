
#include "include.h" 

/*
 * 管理电压，电流，警报...
 * 管理功能函数模块
 * 独立于模块的全局变量的定义
 *
 */

volatile BatteryPackTypedef g_BatteryParameter;
volatile BatteryModeTypedef g_BatteryMode = IDLE;			

uint16_t 					g_ProtectDelayCnt;	// 保护延时计数 
uint16_t 					g_PrechargeTimer = 0;
uint8_t 					g_EnterLowPoweModeFlg;
uint16_t                    g_BeforeChargeCnt = 0;

//不检测欠压二级警告
static BOOL DetectSecondWarning(void)
{
	if((g_SystemWarning.DOT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.COT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.DUT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.CUT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.COV == WARNING_SECOND_LEVEL)   // cell过压故障
			|| (g_SystemWarning.POV == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.COC == WARNING_SECOND_LEVEL)// 充电过流
			|| (g_SystemWarning.DOC == WARNING_SECOND_LEVEL)// 放电过流
			|| (g_SystemWarning.CER == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.VDIF == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.TDIF == WARNING_SECOND_LEVEL))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//----------------------------------------------------------------------------
// Function    : TskAfe_Init
// Description : 电池部分参数的初始化
// Parameters  : none
// Returns     : 
//----------------------------------------------------------------------------
void TskBatteryPra_Init(void)
{
	uint8_t i;
	uint8_t crc = 0;
	uint8_t *ptr;
	uint8_t buff[5];

	g_BatteryParameter.voltage = 0;
	g_BatteryParameter.current = 0;
	g_BatteryParameter.SOC = 0;
	g_BatteryParameter.CellVoltMax = 0;
	g_BatteryParameter.CellVoltMin = 0;
	g_BatteryParameter.CellVoltAvg = 0;
	g_BatteryParameter.CellTemp[0] = 0;
	g_BatteryParameter.CellTemp[1] = 0;
	g_BatteryParameter.CellTemp[2] = 0;
	g_BatteryParameter.CellTemp[3] = 0;
	g_BatteryParameter.CellTempMax = 0;
	g_BatteryParameter.CellTempMin = 0;
	g_BatteryParameter.AmbientTemp = 0;
	g_BatteryParameter.BalanceCellNum = 0;
	g_BatteryParameter.BalanceFlag = 0;

	// 读取充电单体过充警报等级
	EEPROM_ReadBlock(EEPROM_ADDR_COV_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_CellOVThr.cls_1;

		for (i = 0; i < 5; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_CellOVThr = CellOVThrDefault; 
	}

	
	// 读取单体放电低压报警等级
	EEPROM_ReadBlock(EEPROM_ADDR_CUV_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_CellUVThr.cls_1;

		for (i = 0; i < 5; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_CellUVThr = CellUVThrDefault; 
	}

	// 读取充电过温警报等级
	EEPROM_ReadBlock(EEPROM_ADDR_COT_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_PACKCOTThr.cls_1;

		for (i = 0; i < 5; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_PACKCOTThr = CellCOTThrDefault; 
	}

	//读取充电低温报警等级
	EEPROM_ReadBlock(EEPROM_ADDR_CUT_THRHOLD, buff, 5);
    crc = calculate_crc8(buff, 5);

    if (crc == buff[4])
    {
        ptr = (uint8_t *)&g_PACKCUTThr.cls_1;

        for (i = 0; i < 5; i++)
        {
            *ptr++ = buff[i];
        }
    }
    else
    {
        g_PACKCUTThr = CellCUTThrDefault; 
    }
    
	// 读取电池包放电高温
    EEPROM_ReadBlock(EEPROM_ADDR_DOT_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_PACKDOTThr.cls_1;

		for (i = 0; i < 4; i++)
		{
		    *ptr++ = buff[i];
		}
	}
	else
	{
		g_PACKDOTThr = CellDOTThrDefault; 
	}

	// 读取电池包放电低温	
    EEPROM_ReadBlock(EEPROM_ADDR_DUT_THRHOLD, buff, 5);
    crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_PACKDUTThr.cls_1;

		for (i = 0; i < 4; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_PACKDUTThr = CellDUTThrDefault; 
	}

	// 读取电池包充电过流报警等级
    EEPROM_ReadBlock(EEPROM_ADDR_COC_THRHOLD, buff, 5);
    crc = calculate_crc8(buff, 4);

    if (crc == buff[4])
    {
        ptr = (uint8_t *)&g_BattCOCThr.cls_1;

        for (i = 0; i < 4; i++)
        {
            *ptr++ = buff[i];
        }
    }
    else
    {
        g_BattCOCThr = BattCOCThrDefault; 
    }
	// 读取电池包放电过流报警等级
    EEPROM_ReadBlock(EEPROM_ADDR_DOC_THRHOLD, buff, 5);
    crc = calculate_crc8(buff, 4);

    if (crc == buff[4])
    {
        ptr = (uint8_t *)&g_BattDOCThr.cls_1;

        for (i = 0; i < 4; i++)
        {
            *ptr++ = buff[i];
        }
    }
    else
    {
        g_BattDOCThr = BattDOCThrDefault; 
    }

    // 读取电池包温差等级报警
	EEPROM_ReadBlock(EEPROM_ADDR_DLT_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_PACKDLTThr.cls_1;

		for (i = 0; i < 4; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_PACKDLTThr = CellDLTThrDefault; 
	}

	// 读取电池包过压等级
	EEPROM_ReadBlock(EEPROM_ADDR_POV_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_PackOVThr.cls_1;

		for (i = 0; i < 5; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_PackOVThr = PackOVThrDefault; 
	}
	
	// 读取电池包低压等级
	EEPROM_ReadBlock(EEPROM_ADDR_PUV_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_PackUVThr.cls_1;

		for (i = 0; i < 5; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_PackUVThr = PackUVThrDefault; 
	}
	
	// 读取单体非一致性等级
	EEPROM_ReadBlock(EEPROM_ADDR_DLV_THRHOLD, buff, 5);
	crc = calculate_crc8(buff, 4);

	if (crc == buff[4])
	{
		ptr = (uint8_t *)&g_CellIBMThr.cls_1;

		for (i = 0; i < 5; i++)
		{
			*ptr++ = buff[i];
		}
	}
	else
	{
		g_CellIBMThr = CellIBMThrDefault; 
	}

}


//============================================================================
// Function    : TskCurMgt
// Description : 电池包母线电流计算
// Parameters  : none
// Returns     : 
//============================================================================
void TskCurrentMgt(void)
{
	uint8_t i;
	int32_t temp = 0;

	ADC_Convert(CHANNEL_CURRENT); 
	
	while(ADCON0bits.GO);  //等待转换完成，大约需要15us

	g_AdcConvertValue.Current[g_AdcConvertValue.CurIndex++ & 0x07] = ADC_GetCvtRaw();
	
	
	g_AdcConvertValue.CurAvg = ADC_AverageCalculate(g_AdcConvertValue.Current);

	//根据ADC采样值，结合电流采集器的特性，获取真实的电流值
	temp = (int16_t)g_AdcConvertValue.CurAvg - g_CurrentOffset;
	g_BatteryParameter.current = (int16_t)(temp*125/256);

	//检查电池包电流是否超过限定值
	DetectPackOverCurrent();
}


//============================================================================
// Function    : 温度处理
// Description : 四个通道的温度值更新完成后才进行一次温度检测
// Parameters  : none
// Returns     : 
//============================================================================
void TskCellTempMgt(void)
{
	static int8_t  state = 0;

	switch(state++)
	{
	case 0:
		g_BatteryParameter.CellTemp[0] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[0][1]);
		break;
	case 1:
		g_BatteryParameter.CellTemp[1] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[0][2]); 
		break;
	case 2:
		g_BatteryParameter.CellTemp[2] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[0][3]);
		break;
	case 3:
		g_BatteryParameter.CellTemp[3] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[0][4]); 
		break;
	case 4:
		g_BatteryParameter.CellTemp[4] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[1][1]);
		break;
	case 5:
		g_BatteryParameter.CellTemp[5] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[1][2]); 
		break;
	case 6:
		g_BatteryParameter.CellTemp[6] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[1][3]);
		break;
	case 7:
		g_BatteryParameter.CellTemp[7] = ADCToTempVal(g_ArrayLtc6811Unit.temperature[1][4]); 
		break;
	case 8:
		// 检测温度值，并得到单体最大最小温度值
		DetectMaxMinCellTemp();
		DetectCellsOverTemp();
		DetectCellsUnderTemp();
		DetectCellTempDlt();
		state = 0;
		break;
	default:
		state = 0;
		break;
	}
}


//============================================================================
// Function    : TskSOCMgt
// Description : 对电池SOC的管理，涉及到Ah,Soc,Accumulator的存储
// Parameters  : 
// Returns     : none
//============================================================================
void TskSOCMgt(void)
{
   Soc_CalculateAh();	
   Soc_Update();
   Soc_StoreSoc();
}

//============================================================================
// Function    : TaskFaultStore
// Description : 对错误信息进行管理
// Parameters  : none
// Returns     : none
//============================================================================
void TskFaultStoreMgt(void)
{
	static uint16_t state = 0;
		
	ADC_Convert(CHANNEL_12VD);  
	while(ADCON0bits.GO);  //等待转换完成，大约需要15us
	//检测到BMS板电压低于12V，停止存储数据
	if(ADC_GetCvtRaw() < 1840)// 2.25V/5V*4096 
		return;
	
	switch(state & 0x0001) // 电芯低压
	{
	case 0:
		if (g_SystemWarning.CUV == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.cuv < FAULT_REC_LIMIT)
			{
				g_FaultRecord.cuv++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_CUV, (uint8_t*)&g_FaultRecord.cuv, 2);
				Nvm_UpdateRecentFaultRec(FALT_CUV);
			}
			state |= 0x0001;
		}
		break;

	case 1:
		if (g_SystemWarning.CUV != WARNING_SECOND_LEVEL)
		{
			state &= ~0x0001;
		}
		break;

	default:
		break;
	}

	switch (state & 0x0002) // 电芯过压
	{
	case 0:
		if (g_SystemWarning.COV == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.cov < FAULT_REC_LIMIT)
			{
				g_FaultRecord.cov++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_COV, (uint8_t*)&g_FaultRecord.cov, 2);
				Nvm_UpdateRecentFaultRec(FALT_COV);
			}
			state |= 0x0002;
		}
		break;

	case 0x0002:
		if (g_SystemWarning.COV != WARNING_SECOND_LEVEL)
		{
			state &= ~0x0002;
		}
		break;
	default:
		break;
	}

	switch (state & 0x0004)	// 单体一致性
	{
	case 0:
		if (g_SystemWarning.VDIF == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.vdif < FAULT_REC_LIMIT)
			{
				g_FaultRecord.vdif++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_VDIF, (uint8_t*)&g_FaultRecord.vdif, 2);
				Nvm_UpdateRecentFaultRec(FALT_VDIF);	
			}
			state |= 0x0004;
		}
		break;

	case 0x0004:
		if (g_SystemWarning.VDIF != WARNING_SECOND_LEVEL)
		{
			state &= ~0x0004;
		}
		break;
	}

	switch (state & 0x0008) 
	{
	case 0:
		if (g_SystemWarning.CUT == WARNING_SECOND_LEVEL)// 电芯充电低温
		{
			if (g_FaultRecord.cut < FAULT_REC_LIMIT)
			{
				g_FaultRecord.cut++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_CUT, (uint8_t*)&g_FaultRecord.cut, 2);
				Nvm_UpdateRecentFaultRec(FALT_CUT);
			}
			state |= 0x0008;
		}
		if (g_SystemWarning.DUT == WARNING_SECOND_LEVEL)// 电芯放电高温
		{
			if (g_FaultRecord.dut < FAULT_REC_LIMIT)
			{
				g_FaultRecord.dut++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_DUT, (uint8_t*)&g_FaultRecord.dut, 2);
				Nvm_UpdateRecentFaultRec(FALT_DUT);
			}
			state |= 0x0008;
		}
		break;

	case 0x0008:
		if ((g_SystemWarning.CUT != WARNING_SECOND_LEVEL)
			&& (g_SystemWarning.DUT != WARNING_SECOND_LEVEL))
		{
			state &= ~0x0008;
		}
		break;
	}

	switch (state & 0x0010)
	{
	case 0:
		if (g_SystemWarning.COT == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.cot < FAULT_REC_LIMIT)
			{
				g_FaultRecord.cot++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_COT, (uint8_t*)&g_FaultRecord.cot, 2);
				Nvm_UpdateRecentFaultRec(FALT_COT);
			}
			state |= 0x0010;
		}
		if (g_SystemWarning.DOT == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.dot < FAULT_REC_LIMIT)
			{
				g_FaultRecord.dot++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_DOT, (uint8_t*)&g_FaultRecord.dot, 2);
				Nvm_UpdateRecentFaultRec(FALT_DOT);
			}
			state |= 0x0010;
		}
		break;

	case 0x0010:
		if ( (g_SystemWarning.COT != WARNING_SECOND_LEVEL)
			&& (g_SystemWarning.DOT != WARNING_SECOND_LEVEL))
		{
			state &= ~0x0010;
		}
		break;
	}

	switch (state & 0x0020)
	{
	case 0:
		if (g_SystemWarning.COC == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.coc < FAULT_REC_LIMIT)
			{
				g_FaultRecord.coc++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_COC, (uint8_t*)&g_FaultRecord.coc, 2);
				Nvm_UpdateRecentFaultRec(FALT_COC);
			}
			state |= 0x0020;
		}
		if (g_SystemWarning.DOC == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.doc < FAULT_REC_LIMIT)
			{
				g_FaultRecord.doc++; 
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_DOC, (uint8_t*)&g_FaultRecord.doc, 2);
				Nvm_UpdateRecentFaultRec(FALT_DOC);
			}
			state |= 0x0020;
		}
		break;

	case 0x0020:
		if ((g_SystemWarning.COC != WARNING_SECOND_LEVEL) 
			&&(g_SystemWarning.DOC != WARNING_SECOND_LEVEL))	
		{
			state &= ~0x0020;
		}
		break;
	}

	switch (state & 0x0040)
	{
	case 0:
		if (g_SystemWarning.TDIF == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.tdif < FAULT_REC_LIMIT)
			{
				g_FaultRecord.tdif++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_TDIF, (uint8_t*)&g_FaultRecord.tdif, 2);
				Nvm_UpdateRecentFaultRec(FALT_TDIF);
			}
			state |= 0x0040;
		}
		break;

	case 0x0040:
		if (g_SystemWarning.TDIF != WARNING_SECOND_LEVEL)	
		{
			state &= ~0x0040;
		}
		break;
	}

	switch (state & 0x0080)
	{
	case 0:
		if (g_SystemError.ltc_st)
		{
			if (g_FaultRecord.ltc_st < FAULT_REC_LIMIT)
			{
				g_FaultRecord.ltc_st++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_LTCST, (uint8_t*)&g_FaultRecord.ltc_st, 2);
				Nvm_UpdateRecentFaultRec(FALT_LTC);	
			}
			state |= 0x0080;
		}
		break;

	case 0x0080:
		if (!g_SystemError.ltc_st)
		{
			state &= ~0x0080;
		}
		break;
	}

	switch (state & 0x0100)
	{
	case 0:
		if(g_SystemError.ltc_com)
		{
			if(g_FaultRecord.ltc_com < FAULT_REC_LIMIT)
			{
				g_FaultRecord.ltc_com++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_LTCCOM, (uint8_t*)&g_FaultRecord.ltc_com, 2);
				Nvm_UpdateRecentFaultRec(FALT_LTC);
			}
			state |= 0x0100;
		}
		break;

	case 0x0100:
		if (!g_SystemError.ltc_com)
		{
			state &= ~0x0100;
		}
		break;
	}

	switch (state & 0x0200)
	{
	case 0:
		if (g_SystemWarning.POV == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.pov < FAULT_REC_LIMIT)
			{
				g_FaultRecord.pov++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_POV, (uint8_t*)&g_FaultRecord.pov, 2);
				Nvm_UpdateRecentFaultRec(FALT_POV);
			}
			state |= 0x0200;
		}
		if (g_SystemWarning.PUV == WARNING_SECOND_LEVEL)
		{
			if (g_FaultRecord.puv < FAULT_REC_LIMIT)
			{
				g_FaultRecord.puv++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_PUV, (uint8_t*)&g_FaultRecord.puv, 2);
				Nvm_UpdateRecentFaultRec(FALT_PUV);
			}
			state |= 0x0200;
		}
		break;

	case 0x0200:
		if ((g_SystemWarning.POV != WARNING_SECOND_LEVEL) 
			&&(g_SystemWarning.PUV != WARNING_SECOND_LEVEL))	
		{
			state &= ~0x0200;
		}
		break;
	}
	
}


//============================================================================
// Function    : TskBlncMgt
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void AfeBlncMgt(void)
{
	static BOOL balFlg = FALSE;
	static uint16_t timeStamp = 0;
	uint8_t i, j;

	if(g_BatteryMode != CHARGE)
		return;
	if (!balFlg)
	{
		if (g_SysTickMs - timeStamp > 5000)
		{
			g_Ltc6811CfgReg[0].cfgr[4] &= 0x00;
			g_Ltc6811CfgReg[0].cfgr[5] &= 0xF0;

			g_Ltc6811CfgReg[1].cfgr[4] &= 0x00;
			g_Ltc6811CfgReg[1].cfgr[5] &= 0xF0;

			if (g_BatteryParameter.CellVoltMin 
				> CELL_BALANCE_OPEN_VOLT)
			{
				for(i=0; i<ModuleAmount; i++)
				{
					for(j=0;j<(CellsAmount/ModuleAmount);j++)
					{
						if(g_ArrayLtc6811Unit.cellVolt[i][j] - g_BatteryParameter.CellVoltMin 
							> CELL_BALANCE_THRESHOLD)
						{
							if(j<8)
								g_Ltc6811CfgReg[i].cfgr[4] |= ((uint8_t)0x01 << j);
							else
								g_Ltc6811CfgReg[i].cfgr[5] |= ((uint8_t)0x01 << (j-8));
						}
					}
				}
			}

			LTC6811_WriteCfgReg();

			balFlg = TRUE;
			timeStamp = g_SysTickMs;
		}
	}
	else
	{
		static uint8_t n = 0;
		if(n++ < 3)
		{
			g_Ltc6811CfgReg[0].cfgr[4] &= 0x00;
			g_Ltc6811CfgReg[0].cfgr[5] &= 0xF0;

			g_Ltc6811CfgReg[1].cfgr[4] &= 0x00;
			g_Ltc6811CfgReg[1].cfgr[5] &= 0xF0;

			LTC6811_WriteCfgReg();			
		}
		else
		{
			balFlg = FALSE;
			n = 0;
		}
	}
}


//============================================================================
// Function    : TskAfeMgt 模拟前端 analog front end
// Description : 该函数实现LTC6811-1的启动转换、读取数据、数据计算、均衡等功能
//               实现和状态切换
// Parameters  : none
// Returns     : none
//============================================================================
void TskAfeMgt(void)
{
	static uint8_t s_cnt = 0;
    static uint32_t currentTime = 0x00;
	static uint8_t ComErrCnt = 5;
	static AfeStateTypedef AfeState = AFE_TEMP_CNVT;//AFE_VOLT_CNVT;
#if 0
	s_cnt++;
	if(24 == s_cnt)
	{
		g_Ltc6811CfgReg[0].cfgr[0] = 0x06;
	    g_Ltc6811CfgReg[1].cfgr[0] = 0x06;
	    LTC6811_WriteCfgReg();
	    return;
	}
	else if(25 == s_cnt)
	{
		s_cnt = 0;
	}
	else
	{
		return;
	}
#endif
	switch (AfeState)
	{
	case AFE_VOLT_CNVT:
		LTC6811_Adcv(MD_NORMAL,DCP_ENABLED,CELL_CH_ALL);  			//启动单体电压转换
		AfeState = AFE_READ_VOLT;  	//状态切换
		break;

	case AFE_READ_VOLT:
		if (!LTC6811_ReadCellVolt(0,g_ArrayLtc6811Unit.cellVolt))
		{
			AfeState = AFE_VOLT_DETECT;
			g_SystemError.ltc_com = 0;
			ComErrCnt = 5;
		}
		else
		{
			AfeState = AFE_VOLT_DETECT;

			/* mcu与ltc6803 spi通信错误检测 */
			if (ComErrCnt)
			{
				ComErrCnt--;
			}
			else 
			{
				g_SystemError.ltc_com = 1;
				g_FaultRecord.ltc_com ++;
				if (g_ProtectDelayCnt > RELAY_ACTION_DELAY_1S)
				{
					g_ProtectDelayCnt = RELAY_ACTION_DELAY_1S;
				}
			}
		}
		Cell_VoltConvert(g_ArrayLtc6811Unit.cellVolt);
		break;
	case AFE_VOLT_DETECT:
		DetectMaxMinAvgCellVolt();
		DetectCellsOverVolt();
		DetectCellsUnderVolt();
		DetectCellsVoltImba();
		DetectPackOv();
		DetectPackUv();    
		AfeState = AFE_TEMP_CNVT;
        currentTime = g_SysTickMs;
		break;
	case AFE_TEMP_CNVT:
		LTC6811_Adax(MD_NORMAL,AUX_CH_ALL);  	//启动温度转换
		if((g_SysTickMs - currentTime) > 10)
        { 				//状态切换
        	AfeState = AFE_READ_TEMP; 
            LTC6811_Adax(MD_NORMAL,AUX_CH_ALL);  	//启动温度转换
        }
		break;
	case AFE_READ_TEMP:
        if(!LTC6811_ReadAux(0,g_ArrayLtc6811Unit.temperature))
        {
			AfeState = AFE_STAT_CNVT;
			g_SystemError.ltc_com = 0;
			ComErrCnt = 5;
        }
        else
        {
			AfeState = AFE_STAT_CNVT;

			/* mcu与ltc6803 spi通信错误检测 */
			if (ComErrCnt)
			{
				ComErrCnt--;
			}
			else 
			{
				g_SystemError.ltc_com = 1;
				g_FaultRecord.ltc_com ++;
				if (g_ProtectDelayCnt > RELAY_ACTION_DELAY_1S)
				{
					g_ProtectDelayCnt = RELAY_ACTION_DELAY_1S;
				}
			}
        }
        GPIO_VoltConvert(g_ArrayLtc6811Unit.temperature);
		break;
	case AFE_STAT_CNVT:
		LTC6811_Adstat(MD_NORMAL,STAT_ALL);
		AfeState = AFE_READ_STAT;
        break;
	case AFE_READ_STAT:
		if(!LTC6811_ReadCellStat(0,g_ArrayLtc6811Unit.status))
		{
			g_BatteryParameter.voltage = (g_ArrayLtc6811Unit.status[0][0] + g_ArrayLtc6811Unit.status[1][0])/50; 
			AfeState = AFE_BALANCE;
		}
		else
		{
			AfeState = AFE_BALANCE;

			/* mcu与ltc6803 spi通信错误检测 */
			if (ComErrCnt)
			{
				ComErrCnt--;
			}
			else 
			{
				g_SystemError.ltc_com = 1;
				g_FaultRecord.ltc_com ++;
				if (g_ProtectDelayCnt > RELAY_ACTION_DELAY_1S)
				{
					g_ProtectDelayCnt = RELAY_ACTION_DELAY_1S;
				}
			}
		}
        break;
	case AFE_BALANCE:
		AfeBlncMgt();
		AfeState = AFE_VOLT_CNVT;
		break;

	default:
		AfeState = AFE_VOLT_CNVT;
		break;
	}
#if 0
	g_Ltc6811CfgReg[0].cfgr[0] = 0x04;
    g_Ltc6811CfgReg[1].cfgr[0] = 0x04;

	LTC6811_WriteCfgReg();
#endif
}

//----------------------------------------------------------------------------
// Function    : TskCanMgt
// Description : 对CAN总线发送接收缓冲区的管理
//				CAN数据首先进入发送接收缓冲区，然后进行处理 执行周期10Ms
// Parameters  : 
// Returns     : 
//----------------------------------------------------------------------------

void TskCanMgt(void)
{
	TskCanProcessRxMsg();           // 处理接收数据
	CAN_BroadcastBufUpdate();       // 将要发送的广播数据更新到发送缓冲区
	TskCanSendTxBufMsg();           // 发送发送缓冲区数据
}


//============================================================================
// Function    ：BatteryModeSwitch
// Description ：
// Parameters  ：none
// Returns     ：none
//============================================================================
void TskBatteryModeMgt(void)
{
	switch (g_BatteryMode)
	{
	case IDLE:  
		if ( DetectSecondWarning() 
			|| (g_SystemError.all & 0x87))    
		{
			g_BatteryMode = PROTECTION;
		}
		else
		{
			if(GetChargeState() && !Button) // 充电器接入
			{
				g_BatteryMode = CHARGE;
			}
            else if(!GetChargeState() && Button)
			{
				g_BatteryMode = DISCHARGE; 
			}
			else
			{
				g_BatteryMode = PROTECTION;
			}
		}
		break;

	case DISCHARGE:  //放电状态
		if (DetectSecondWarning() 
			|| (g_SystemWarning.CUV == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.PUV == WARNING_SECOND_LEVEL)
			|| (g_SystemError.all & 0x87))
		{
			g_BatteryMode = PROTECTION;
		}
		else
		{
			if(GetChargeState())
			{
				g_BatteryMode = IDLE;                    
			}  
		}
		break;
        
	case CHARGE:  //充电状态  
			if ( DetectSecondWarning() 
				|| (g_SystemError.all & 0x87))
			{
				g_BatteryMode = PROTECTION;
			}
			else
			{
				if(!GetChargeState())  // 检查充电插头是否拔掉
				{
					g_BatteryMode = IDLE;
				}
			}
			break;

	case PROTECTION:  //保护状态 
		if( DetectSecondWarning() 
			|| g_SystemError.all)
		{
			g_BatteryMode = PROTECTION;
		}
		else
		{
			if(GetChargeState())  // 检查充电插头是否拔掉
			{
				if(DetectPackChargeFinish())
					g_BatteryMode = PROTECTION;
				else	
					g_BatteryMode = IDLE;
			}
			else
			{
				if(g_BatteryParameter.SOC > 0)
					g_BatteryMode = IDLE;
			}
		}
		break;
	default:  
		break;
    }
}


void TskSohMgt(void)
{
	static uint8_t cnt = 0;
	static uint8_t state = 0;
	
	if(g_BatteryMode == CHARGE)
	{
		cnt++;
		if(cnt>= 50)
		{
			switch(state)
			{
			case 0:
				Soh_UpdateCycleTime();	
				state = 1;
				break;
			case 1:
				Soh_StoreChargedAh();
				state = 0;
				cnt = 0;
				break;
			default:
				state = 0;
				break;
			}
		}
	}	
}

// 设置充电器电压电流
void TskSetCCsPra(void)
{
	// 充电三个阶段
	// 1.涓流充电
	if(CHARGE == g_BatteryMode)
	{
		if(g_BatteryParameter.CellVoltMin <= CELLPRECHARGEVOLT)
		{
			g_CCS_MaxVoltage = 835; // 单位0.1V
			g_CCS_MaxCurrent = 60; // 单位0.1A
		}
		// 2.恒流充电
		if((g_BatteryParameter.CellVoltMin > CELLSTDCHARGEVOLT)
		&& (g_BatteryParameter.CellVoltMax < CELLCONSTCURRENTVOLT))
		{
			g_CCS_MaxVoltage = 835; // 单位0.1V
			g_CCS_MaxCurrent = 150; // 单位0.1A
		}
		// 3.恒压充电
		if(g_BatteryParameter.CellVoltMax >= CELLCONSTCURRENTVOLT)
		{
			g_CCS_MaxVoltage = 835; // 单位0.1V
			g_CCS_MaxCurrent = 60; // 单位0.1A
		}
	}
}

// 如果接通电源，十分钟未进行其他操作，CPU进入休眠模式
void TskCPUMode(void)
{
	static uint32_t time = 0;
	if(g_BatteryMode == DISCHARGE)
	{
		if(g_BatteryParameter.current < 20) // 放电电流小于2A
		{
			time ++;
		}
		else
		{
			time = 0;
		}
		if(time > 180000U) // 10分钟
		{
			SLEEP();
		}
	}
}



