
/* 
 * File:   sci.c
 * Author: Yanliang.Zhu
 * GlobeTools串口模拟通信
 * Created on 2016年12月24日, 下午1:34
 */

#include "include.h"

volatile SciMsgRxBufTypedef g_sciRxBuf;
volatile SciTypedef g_sciTx;
volatile SciTypedef g_sciRx;


volatile uint8_t 	g_battSeriNum = 0x00;		// 电池串数
volatile uint8_t 	g_battPatallNum = 0x00;	// 电池并数
volatile int8_t     g_battTemVal = 0x00;		// 电池温度

volatile uint16_t 	g_year = 0x00;
volatile uint16_t 	g_month = 0x00;
volatile uint16_t 	g_date = 0x00;
volatile uint16_t 	g_cycle = 0x00;
volatile uint16_t 	g_historyHight = 0x00;
volatile uint16_t 	g_historyLow = 0x00;

volatile BOOL 		g_canSend = TRUE;
volatile uint8_t 	g_battState[4] = {0x00};

void Sci_Init(void)
{
    //IO初始化
    TRISDbits.TRISD3 = 0b0; // 发送引脚
	TRISBbits.TRISB0 = 0b1; // 接收引脚
    ANCON1bits.ANSEL10 = 0b0;

    TRISDbits.TRISD1 = 0b0;
	DisableSCI();
	SCI_TX_PORT = 0b0;
    Set_IdelMode();
}

void HistoryInfo_Init(void)
{
	g_year = 0x00;
	g_month = 0x00;
	g_date = 0x00;
	g_cycle = 0x00;
	g_historyHight = 0x00;
	g_historyLow = 0x00;
}

void Timer1_IrqEnable(void)
{
    PIR1bits.TMR1IF = 0b0;
    PIE1bits.TMR1IE = 0b1;
}

void Timer1_IrqDisable(void)
{
    PIR1bits.TMR1IF = 0b0;
    PIE1bits.TMR1IE = 0b0;
}

void SCI_SetClk(uint16_t val)
{
	Disable_Timer1();
    if(100 == val)
    {
        TMR1 = (uint16_t)64762;
        //TMR1H = 0xFC;
        //TMR1L = 0xFA;
    }
    else if(200 == val)
    {
        TMR1 = (uint16_t)63962;
        //TMR1H = 0xF9;
        //TMR1L = 0xDA;
    } 
    else if(400 == val)
    {
        TMR1 = (uint16_t)62340;// 400uS
    }
    else
    {
        TMR1 = (uint16_t)60740;// 600uS
    }
    Enable_Timer1();
}

void Set_IdelMode(void)
{
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b0; // 暂时禁止外部中断0
	
	Timer1_IrqDisable();	 // 禁止Timer1中断
    g_canSend = TRUE;
}

void Set_TxMode(void)
{
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b0; // 暂时禁止外部中断0
}

void Set_RxMod(void)
{
    Timer1_IrqDisable();
    SCI_TX_PORT = 0b0;
    
	INTCON2bits.INTEDG0 = 0b1;  // 设置上升沿触发中断
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b1;  // 允许外部中断
    
    g_canSend = FALSE;
}

void Sci_StartSend(void)
{
    SCI_TX_PORT = 0b0;
    g_canSend = FALSE;
    
    SCI_SetClk(600);
    Timer1_IrqEnable();
}

BOOL Sci_IsRxBufFull(void)
{
	if((g_sciRxBuf.RxBuf_Wptr+1)%SCI_BUF_DEEP == g_sciRxBuf.RxBuf_Rptr)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Sci_IsRxBufEmpty(void)
{
	if (g_sciRxBuf.RxBuf_Rptr == g_sciRxBuf.RxBuf_Wptr)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Sci_SendByte(void)
{
	static uint8_t txBitsCnt = 0x00;
	static BOOL flag = FALSE;

	if(FALSE == flag)
	{
        SCI_TX_PORT = 0b1;
		if(0x01 == (g_sciTx.Data.allData&0x01))
		{
			SCI_SetClk(200);
		}
		else
		{
			SCI_SetClk(100);
		}
		flag = TRUE;
	}
	else
	{
        SCI_TX_PORT = 0b0;
		SCI_SetClk(100);
		flag = 0;
        txBitsCnt ++;
        g_sciTx.Data.allData >>= 1;
	}

	if(txBitsCnt>=16)
	{
        txBitsCnt = 0;
        flag = FALSE;
		Set_RxMod();
	}
}

//协议与实际的接收电平发生反转
/*
 _______|----|_|-|--|....400uS高是开始
 */
void Sci_RcvBits(void)
{
    uint16_t sciTimeCnt = 0;
    static uint8_t rxBitsCnt = 0x00;
	static BOOL b_getHeader = FALSE;
    
    if(0b1 == INTCON2bits.INTEDG0)
    {
        TMR1 = 0x0;
        INTCON2bits.INTEDG0 = 0b0;  // 设置下一次由下降沿触发中断
    }
    else
    {
        sciTimeCnt = TMR1;
        
        INTCON2bits.INTEDG0 = 1;  // 设置下一次由上升沿触发中断
        
        if((sciTimeCnt > (uint16_t)2800) && (sciTimeCnt < (uint16_t)3600))
        {
            rxBitsCnt = 0;
            g_sciRx.Data.allData = 0;
			b_getHeader = TRUE;
            return;
        }
		if(!b_getHeader) //只有头部来触发读取数据操作
			return;
        
        if((sciTimeCnt > (uint16_t)1200) && (sciTimeCnt < (uint16_t)2000)) //150uS - 250uS
        {
            g_sciRx.Data.allData |= ((uint32_t)0x01<<rxBitsCnt);
        }
        
        if(++rxBitsCnt > 23) 
        {
            rxBitsCnt = 0;
			Set_IdelMode();// 接收完一帧数据后进入空闲模式
			b_getHeader = FALSE; // 头部标志位复位
            g_canSend = TRUE;
        }
    }
}


// 数组array存在val则返回val的小标，不存在返回-1
int8_t ScanValIsIn(uint8_t val,uint8_t array[],uint8_t len)
{
	int8_t i = 0;
	int8_t pos = -1;
	
	for(i=0;i<len;i++)
	{
		if(val == array[i])
		{
			pos = i;
			break;
		}
	}

	return pos;
}

// 在数组array内找到空位置，找不到返回-1
int8_t FindPos(uint8_t array[],uint8_t len)
{
	int8_t pos = -1;
	for(int8_t i=0;i<len;i++)
	{
		if(array[i] == 0x00)
		{
			pos = i;
			break;
		}
	}
	
	return pos;
}

void TskHandleSciBuf(void)
{
	static uint32_t lastSendData = 0x00;
	if(g_canSend)
    {
    #if 0
		if((g_sciRx.Data.cmd + g_sciRx.Data.data) != g_sciRx.Data.crc) // 接收的数据有误
		{
			Set_RxMod();//继续接收
			return;
		}
	#endif

		if(SERINUM == g_sciRx.Data.cmd)
			g_battSeriNum = g_sciRx.Data.data;
		else if(PALNUM == g_sciRx.Data.cmd)
			g_battPatallNum = g_sciRx.Data.data;
		else if(BATTEMP == g_sciRx.Data.cmd)
			g_battTemVal = g_sciRx.Data.data;
		else if(YEAR == g_sciRx.Data.cmd)
			g_year = g_sciRx.Data.data;
		else if(MONTH == g_sciRx.Data.cmd)
			g_month = g_sciRx.Data.data;
		else if(DATE == g_sciRx.Data.cmd)
			g_date = g_sciRx.Data.data;
		else if(CYCLE == g_sciRx.Data.cmd)
		{
			g_cycle = g_sciRx.Data.crc;
			g_cycle <<= 8;
			g_cycle += g_sciRx.Data.data;
		}
		else if(MAX == g_sciRx.Data.cmd)
		{
			static uint16_t vMax = 0; 
			vMax = g_sciRx.Data.crc;
			vMax <<= 8;
			vMax += g_sciRx.Data.data;
			if(g_historyHight < vMax)
			{
				g_historyHight = vMax;
			}
		}
		else if(MIN == g_sciRx.Data.cmd)
		{
			static uint16_t vMin1 = 0; 
			static uint16_t vMin2 = 0xFFFF;
			static uint32_t s_lastTick;

			if((g_SysTickMs - s_lastTick) > 2000)
			{
				vMin2 = 0xFFFF;
			}
			
			vMin1 = g_sciRx.Data.crc;
			vMin1 <<= 8;
			vMin1 += g_sciRx.Data.data;
			
			if(vMin2 > vMin1)
			{
				vMin2 = vMin1;	
			}
			g_historyLow = vMin2;
			s_lastTick = g_SysTickMs;
		}
		else
        {
            if((0x00 != g_sciRx.Data.cmd))
            {
            	//将g_sciRx.Data.cmd放入电池状态数组
            	if((int8_t)-1 == ScanValIsIn(g_sciRx.Data.cmd,g_battState,4))
            	{
					int8_t pos = FindPos(g_battState,4);
					if(pos != (int8_t)-1)
						g_battState[pos] = g_sciRx.Data.cmd;
            	}
            }
        }

        g_sciTx.Data.allData = lastSendData;
        if((CMD_SCI == g_testCmd) || (CMD_RESET == g_testCmd)) // 使电池包复位
        {
            g_sciTx.Data.allData = 0xE11E;
            g_battRestOk = TRUE;
        }
        // 电池包充电
        else if((CMD_SeriNum == g_testCmd) || (CMD_PatalNum == g_testCmd) 
        		 || (CMD_BattTemp == g_testCmd))
        {
            g_sciTx.Data.allData = 0xE41B;
        }
        else if(CMD_EnDischg == g_testCmd)
        {
			g_sciTx.Data.allData = 0xD22D;
        }
        else if((CMD_HistoryCycle == g_testCmd) || (CMD_HistoryDate == g_testCmd)
        		|| (CMD_HistoryHigh == g_testCmd)  || (CMD_HistoryLow == g_testCmd) 
        		|| (CMD_HistoryMonth == g_testCmd) || (CMD_HistoryYear == g_testCmd))
        {
			g_sciTx.Data.allData = 0x02FD;
        }
        
        if(0x00 != g_sciTx.Data.allData)
    	{
        	Set_TxMode();
        	Sci_StartSend();
        }
        
        lastSendData = g_sciTx.Data.allData;
        // 一个测试周期结束
		if((CMD_RESET == g_testCmd) || (CMD_DetectApp == g_testCmd))
		{
			lastSendData = 0x00;
		}
    }
}




