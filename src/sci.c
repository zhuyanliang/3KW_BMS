
/* 
 * File:   sci.c
 * Author: Yanliang.Zhu
 * GlobeTools串口模拟通信
 * Created on 2016年12月24日, 下午1:34
 */

#include "include.h"

SciTypedef g_SCI;
uint8_t g_SciSendBuf[MESSAGE_SEND_LEN];
uint8_t g_SciRcvBuf[MESSAGE_RECEIVE_LEN];
uint8_t g_SciByteCnt;
uint8_t g_SciRxBufFull;


void Sci_Init(void)
{
    //IO初始化
    TRISDbits.TRISD3 = 0b0; // 发送引脚
	TRISBbits.TRISB0 = 0b1; // 接收引脚
    ANCON1bits.ANSEL10 = 0b0;

    TRISDbits.TRISD1 = 0b0;
	SCI_TX_PORT = 0b0;

	g_SCI.STAT.TxEF = 1;  // 发送缓冲器空
	g_SCI.STAT.RxFF = 0;  // 接收缓冲器空
	g_SCI.TBCNT = 0;  // 清零发送位计数器
	g_SCI.RBCNT = 0;  // 清零接收位计数器
	g_SciByteCnt = 0;

	SCI_SetTxMode();
    
}

void SCI_TimerIrqEnable(void)
{
    PIR1bits.TMR1IF = 0b0;
    PIE1bits.TMR1IE = 0b1;
}

void SCI_TimerIrqDisable(void)
{
    PIR1bits.TMR1IF = 0b0;
    PIE1bits.TMR1IE = 0b0;
}

void SCI_SetClk(uint16_t val)
{
	SCI_TimerIrqDisable();
    if(100 == val)
    {
        TMR1 = (uint16_t)64762;
        //TMR1H = 0xFC;
        //TMR1L = 0xFA;
    }
    else 
    {
        TMR1 = (uint16_t)63962;
        //TMR1H = 0xF9;
        //TMR1L = 0xDA;
    } 
    SCI_TimerIrqEnable();
}

void SCI_WriteByte(uint8_t data)
{
	g_SCI.DR = data;
	g_SCI.STAT.TxEF = 0;
	g_SCI.TBCNT = 8;
}

uint8_t SCI_ReadByte(void)
{
	return g_SCI.DR;
}

void SCI_SetIdelMode(void)
{
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b0; // 暂时禁止外部中断0
	
	Timer1_IrqDisable();	 // 禁止Timer1中断
}

void SCI_SetTxMode(void)
{
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b0; // 暂时禁止外部中断0
}

void SCI_SetRxMode(void)
{
    SCI_TimerIrqDisable();
    SCI_TX_PORT = 0b0;
    
	INTCON2bits.INTEDG0 = 0b1;  // 设置上升沿触发中断
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b1;  // 允许外部中断
}

void SCI_StartSend(void)
{
	g_SCI.DR = 0x01;
	g_SCI.STAT.TxEF = 0;
	g_SCI.TBCNT = 1;
	
    SCI_TX_PORT = 0b1;
    SCI_SetClk(200);
    SCI_TimerIrqEnable();
}

void SCI_SendByte(void)
{
	static BOOL flag = FALSE;

	if(g_SCI.STAT.TxEF)
		return;
	if(FALSE == flag)
	{
		if(g_SCI.DR&0x01)
		{
			SCI_SetClk(200);
		}
		else
		{
			SCI_SetClk(100);
		}
		SCI_TX_PORT = 1;

		g_SCI.DR >>= 1;
		g_SCI.TBCNT--;
		flag = TRUE;
	}
	else
	{
        SCI_TX_PORT = 0b0;
		SCI_SetClk(100);
		flag = FALSE;
        if(0 == g_SCI.TBCNT)
        {
			g_SCI.STAT.TxEF = 1;
        }
	}
}

//协议与实际的接收电平发生反转
/*
 _______|----|_|-|--|....400uS高是开始
 */
void SCI_RcvBits(void)
{
    uint16_t sciTimeCnt = 0;
    
    if(0b1 == INTCON2bits.INTEDG0)
    {
        TMR1 = 0x0;
        INTCON2bits.INTEDG0 = 0b0;  // 设置下一次由下降沿触发中断
    }
    else // 如果是上升沿 __|-
    {
        sciTimeCnt = TMR1;
        INTCON2bits.INTEDG0 = 1;  // 设置下一次由上升沿触发中断
        g_SCI.DR >>= 1;
        
        if(sciTimeCnt > (uint16_t)1200) //150uS - 250uS
        {
            g_SCI.DR |= 0x80;
        }

        if(++g_SCI.RBCNT > 7)
        {
			g_SCI.STAT.RxFF = 0b1;
			g_SCI.RBCNT = 0;
        }
    }
}

//============================================================================
// Function    : sciSendMsg
// Description : message发送，该函数主要功能为：将缓冲区中的数据逐位从通信端口
//               移出
// Parameters  : none
// Returns     : none
//============================================================================

void SCI_SendMsg(void)
{
	SCI_SendByte();
	if(g_SCI.STAT.TxEF)
	{
		if(g_SciByteCnt < MESSAGE_SEND_LEN)
		{
			SCI_WriteByte(g_SciSendBuf[g_SciByteCnt++]);
		}
		else
		{
			SCI_TimerIrqDisable();  // 缓冲区数据全部发送完毕，关闭定时器中断
			SCI_SetRxMode();  		// 设置通信端口为接收模式
			g_SciByteCnt = 0;
		}
	}
	
}


//============================================================================
// Function    : sciRcvMsg
// Description : message发送，该函数主要功能为：将缓冲区中的数据逐位从通信端口
//               移出
// Parameters  : none
// Returns     : none
//============================================================================
void SCI_RcvMsg(void)
{
   SCI_RcvBits();

   if (g_SCI.STAT.RxFF)
   {      
      g_SciRcvBuf[g_SciByteCnt++] = SCI_ReadByte();
      g_SCI.STAT.RxFF = 0;

      if (g_SciByteCnt >= MESSAGE_RECEIVE_LEN)
      {
         g_SciRxBufFull = 1;
         SCI_SetTxMode();  // 设置通信端口为发送模式
         g_SciByteCnt = 0;
      }
   }
}


//============================================================================
// Function    : sciIsrHandle
// Description : SCI收发中断函数，该函数在中断中调用。
// Parameters  : none
// Returns     : none
//============================================================================
void SCI_IsrHandle(void)
{
	if (g_SCI.STAT.TxRx)
	{
		if (PIR1bits.TMR1IF)
		{
			PIR1bits.TMR1IF = 0;
			SCI_SendMsg();
		}
	}
	else
	{
		if (INTCONbits.INT0IF)
		{
			INTCONbits.INT0IF = 0;
			SCI_RcvMsg();
		}
	}
}




