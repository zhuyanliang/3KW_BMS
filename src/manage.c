
#include "include.h" 

/*
 * �����ѹ������������...
 * �����ܺ���ģ��
 * ������ģ���ȫ�ֱ����Ķ���
 *
 */

volatile BatteryPackTypedef g_BatteryParameter;
volatile BatteryModeTypedef g_BatteryMode = IDLE;			

uint16_t 					g_ProtectDelayCnt;	// ������ʱ���� 
uint16_t 					g_PrechargeTimer = 0;
uint8_t 					g_EnterLowPoweModeFlg;
uint16_t                    g_BeforeChargeCnt = 0;

//�����Ƿѹ��������
static BOOL DetectSecondWarning(void)
{
	if((g_SystemWarning.DOT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.COT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.DUT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.CUT == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.COV == WARNING_SECOND_LEVEL)   // cell��ѹ����
			|| (g_SystemWarning.POV == WARNING_SECOND_LEVEL)
			|| (g_SystemWarning.COC == WARNING_SECOND_LEVEL)// ������
			|| (g_SystemWarning.DOC == WARNING_SECOND_LEVEL)// �ŵ����
			|| (g_SystemWarning.ISO == WARNING_SECOND_LEVEL)
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
// Description : ��ز��ֲ����ĳ�ʼ��
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

	// ��ȡ��絥����侯���ȼ�
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

	
	// ��ȡ����ŵ��ѹ�����ȼ�
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

	// ��ȡ�����¾����ȼ�
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

	//��ȡ�����±����ȼ�
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
    
	// ��ȡ��ذ��ŵ����
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

	// ��ȡ��ذ��ŵ����	
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

	// ��ȡ��ذ������������ȼ�
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
	// ��ȡ��ذ��ŵ���������ȼ�
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

    // ��ȡ��ذ��²�ȼ�����
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

	// ��ȡ��ذ���ѹ�ȼ�
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
	
	// ��ȡ��ذ���ѹ�ȼ�
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
	
	// ��ȡ�����һ���Եȼ�
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
// Description : ��ذ�ĸ�ߵ�������
// Parameters  : none
// Returns     : 
//============================================================================
void TskCurrentMgt(void)
{
	uint8_t i;
	static uint8_t curr_flg = 0;

	ADC_Convert(CHANNEL_CURRENT); 
	
	while(ADCON0bits.GO);  //�ȴ�ת����ɣ���Լ��Ҫ15us
	//��һ������ʱ��һ���Զ�ȡ8������ֵ
	if (!curr_flg)
	{
		for ( i = 0; i < 8; i++ )
		{
			g_AdcConvertValue.Current[i] = ADC_GetCvtRaw();
		} 
		curr_flg = 1;    
	}
	else //�Ժ�ÿ�θ���һ����ѹֵ
	{ 
		g_AdcConvertValue.Current[g_AdcConvertValue.CurIndex++ & 0x07] = ADC_GetCvtRaw();
	}
	
	g_AdcConvertValue.CurAvg = ADC_AverageCalculate(g_AdcConvertValue.Current);

	//����ADC����ֵ����ϵ����ɼ��������ԣ���ȡ��ʵ�ĵ���ֵ
	g_BatteryParameter.current = 0x00;

	//����ذ������Ƿ񳬹��޶�ֵ
	DetectPackOverCurrent();
}


//============================================================================
// Function    : �¶ȴ���
// Description : �ĸ�ͨ�����¶�ֵ������ɺ�Ž���һ���¶ȼ��
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
		// ����¶�ֵ�����õ����������С�¶�ֵ
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
// Description : �Ե��SOC�Ĺ����漰��Ah,Soc,Accumulator�Ĵ洢
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
// Description : �Դ�����Ϣ���й���
// Parameters  : none
// Returns     : none
//============================================================================
void TskFaultStoreMgt(void)
{
	static uint16_t state = 0;
		
	ADC_Convert(CHANNEL_12VD);  
	while(ADCON0bits.GO);  //�ȴ�ת����ɣ���Լ��Ҫ15us
	//��⵽BMS���ѹ����12V��ֹͣ�洢����
	if(ADC_GetCvtRaw() < 1840)// 2.25V/5V*4096 
		return;
	
	switch(state & 0x0001) // ��о��ѹ
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

	switch (state & 0x0002) // ��о��ѹ
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

	switch (state & 0x0004)	// ����һ����
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
		if (g_SystemWarning.CUT == WARNING_SECOND_LEVEL)// ��о������
		{
			if (g_FaultRecord.cut < FAULT_REC_LIMIT)
			{
				g_FaultRecord.cut++;
				EEPROM_WriteBlock(EEPROM_ADDR_FAULT_CUT, (uint8_t*)&g_FaultRecord.cut, 2);
				Nvm_UpdateRecentFaultRec(FALT_CUT);
			}
			state |= 0x0008;
		}
		if (g_SystemWarning.DUT == WARNING_SECOND_LEVEL)// ��о�ŵ����
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
void TskBlncMgt(void)
{
	
}



//============================================================================
// Function    : TskAfeMgt ģ��ǰ�� analog front end
// Description : �ú���ʵ��LTC6811-1������ת������ȡ���ݡ����ݼ��㡢����ȹ���
//               ʵ�ֺ�״̬�л�
// Parameters  : none
// Returns     : none
//============================================================================
void TskAfeMgt(void)
{
	static uint8_t ComErrCnt = 5;
	static AfeStateTypedef AfeState = AFE_VOLT_CNVT;
        
	switch (AfeState)
	{
	case AFE_VOLT_CNVT:
		LTC6811_Adcv(MD_NORMAL,DCP_ENABLED,CELL_CH_ALL);  			//���������ѹת��
		AfeState = AFE_READ_VOLT;  	//״̬�л�
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
			AfeState = AFE_VOLT_CNVT;

			/* mcu��ltc6803 spiͨ�Ŵ����� */
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
	case AFE_VOLT_DETECT:
		DetectMaxMinAvgCellVolt();
		DetectCellsOverVolt();
		DetectCellsUnderVolt();
		DetectCellsVoltImba();
		DetectPackOv();
		DetectPackUv();    
		AfeState = AFE_TEMP_CNVT;//AFE_BALANCE;
		break;

	case AFE_TEMP_CNVT:
		LTC6811_Adax(MD_NORMAL,AUX_CH_ALL);  	//�����¶�ת��
		AfeState = AFE_READ_TEMP;  				//״̬�л�
		break;

	case AFE_READ_TEMP:
        LTC6811_ReadAux(0,g_ArrayLtc6811Unit.temperature);;
        AfeState = AFE_BALANCE;					// AFE_CAL_TEMP;
		break;
		
	case AFE_BALANCE:
		TskBlncMgt();
		AfeState = AFE_VOLT_CNVT;
		break;

	default:
		AfeState = AFE_VOLT_CNVT;
		break;
   }
}

//----------------------------------------------------------------------------
// Function    : TskCanMgt
// Description : ��CAN���߷��ͽ��ջ������Ĺ���
//				CAN�������Ƚ��뷢�ͽ��ջ�������Ȼ����д��� ִ������10Ms
// Parameters  : 
// Returns     : 
//----------------------------------------------------------------------------

void TskCanMgt(void)
{
	TskCanProcessRxMsg();           // �����������
	CAN_BroadcastBufUpdate();       // ��Ҫ���͵Ĺ㲥���ݸ��µ����ͻ�����
	TskCanSendTxBufMsg();           // ���ͷ��ͻ���������
}


//============================================================================
// Function    ��BatteryModeSwitch
// Description ��
// Parameters  ��none
// Returns     ��none
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
			if(GetChargeState()) // ���������
			{
				g_BatteryMode = BEFORECHARGE;
                g_BeforeChargeCnt = 0;
			}
            else
			{
				g_BatteryMode = PRECHARGE;
				g_PrechargeTimer =0;  
			}
		}
		break;

	case PRECHARGE:  //Ԥ���״̬
		if(++g_PrechargeTimer > PRE_CHARGE_TIME)
		{
			g_BatteryMode = DISCHARGE;
			g_PrechargeTimer = 0;
		}
		break;

	case DISCHARGE:  //�ŵ�״̬
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
        
    case BEFORECHARGE:
        if(++g_BeforeChargeCnt > BEFORE_CHARGE_TIME)
        {
            g_BatteryMode = CHARGE;
            g_BeforeChargeCnt = 0;
        }
        break;
	case CHARGE:  //���״̬  
			if ( DetectSecondWarning() 
				|| (g_SystemError.all & 0x87))
			{
				g_BatteryMode = PROTECTION;
			}
			else
			{
				if(!GetChargeState())  // ������ͷ�Ƿ�ε�
				{
					g_BatteryMode = IDLE;
				}
			}
			break;

	case PROTECTION:  //����״̬ 
		if( DetectSecondWarning() 
			|| g_SystemError.all)
		{
			g_BatteryMode = PROTECTION;
		}
		else
		{
			if(GetChargeState())  // ������ͷ�Ƿ�ε�
			{
				if(DetectPackChargeFinish())
					g_BatteryMode = PROTECTION;
				else	
					g_BatteryMode = BEFORECHARGE;
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

// ���ó������ѹ����
void TskSetCCsPra(void)
{
	// ��������׶�
	// 1.������
	if(CHARGE == g_BatteryMode)
	{
		if(g_BatteryParameter.CellVoltMin <= CELLPRECHARGEVOLT)
		{
			g_CCS_MaxVoltage = 835; // ��λ0.1V
			g_CCS_MaxCurrent = 60; // ��λ0.1A
		}
		// 2.�������
		if((g_BatteryParameter.CellVoltMin > CELLSTDCHARGEVOLT)
		&& (g_BatteryParameter.CellVoltMax < CELLCONSTCURRENTVOLT))
		{
			g_CCS_MaxVoltage = 835; // ��λ0.1V
			g_CCS_MaxCurrent = 150; // ��λ0.1A
		}
		// 3.��ѹ���
		if(g_BatteryParameter.CellVoltMax >= CELLCONSTCURRENTVOLT)
		{
			g_CCS_MaxVoltage = 835; // ��λ0.1V
			g_CCS_MaxCurrent = 60; // ��λ0.1A
		}
	}
}

// �����ͨ��Դ��ʮ����δ��������������CPU��������ģʽ
void TskCPUMode(void)
{
	static uint32_t time = 0;
	if(g_BatteryMode == DISCHARGE)
	{
		if(g_BatteryParameter.current < 20) // �ŵ����С��2A
		{
			time ++;
		}
		else
		{
			time = 0;
		}
		if(time > 180000U) // 10����
		{
			SLEEP();
		}
	}
}



