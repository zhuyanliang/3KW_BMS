
/* 
 * File:   sci.c
 * Author: Yanliang.Zhu
 * GlobeTools����ģ��ͨ��
 * Created on 2016��12��24��, ����1:34
 */

#include "include.h"

SciTypedef g_SCI;
uint8_t g_SciSendBuf[MESSAGE_SEND_LEN];
uint8_t g_SciRcvBuf[MESSAGE_RECEIVE_LEN];
uint8_t g_SciByteCnt;
uint8_t g_SciRxBufFull;


void Sci_Init(void)
{
    //IO��ʼ��
    TRISDbits.TRISD3 = 0b0; // ��������
	TRISBbits.TRISB0 = 0b1; // ��������
    ANCON1bits.ANSEL10 = 0b0;

    TRISDbits.TRISD1 = 0b0;
	SCI_TX_PORT = 0b0;

	g_SCI.STAT.TxEF = 1;  // ���ͻ�������
	g_SCI.STAT.RxFF = 0;  // ���ջ�������
	g_SCI.TBCNT = 0;  // ���㷢��λ������
	g_SCI.RBCNT = 0;  // �������λ������
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
	INTCONbits.INT0IE = 0b0; // ��ʱ��ֹ�ⲿ�ж�0
	
	Timer1_IrqDisable();	 // ��ֹTimer1�ж�
}

void SCI_SetTxMode(void)
{
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b0; // ��ʱ��ֹ�ⲿ�ж�0
}

void SCI_SetRxMode(void)
{
    SCI_TimerIrqDisable();
    SCI_TX_PORT = 0b0;
    
	INTCON2bits.INTEDG0 = 0b1;  // ���������ش����ж�
	INTCONbits.INT0IF = 0b0;
	INTCONbits.INT0IE = 0b1;  // �����ⲿ�ж�
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

//Э����ʵ�ʵĽ��յ�ƽ������ת
/*
 _______|----|_|-|--|....400uS���ǿ�ʼ
 */
void SCI_RcvBits(void)
{
    uint16_t sciTimeCnt = 0;
    
    if(0b1 == INTCON2bits.INTEDG0)
    {
        TMR1 = 0x0;
        INTCON2bits.INTEDG0 = 0b0;  // ������һ�����½��ش����ж�
    }
    else // ����������� __|-
    {
        sciTimeCnt = TMR1;
        INTCON2bits.INTEDG0 = 1;  // ������һ���������ش����ж�
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
// Description : message���ͣ��ú�����Ҫ����Ϊ�����������е�������λ��ͨ�Ŷ˿�
//               �Ƴ�
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
			SCI_TimerIrqDisable();  // ����������ȫ��������ϣ��رն�ʱ���ж�
			SCI_SetRxMode();  		// ����ͨ�Ŷ˿�Ϊ����ģʽ
			g_SciByteCnt = 0;
		}
	}
	
}


//============================================================================
// Function    : sciRcvMsg
// Description : message���ͣ��ú�����Ҫ����Ϊ�����������е�������λ��ͨ�Ŷ˿�
//               �Ƴ�
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
         SCI_SetTxMode();  // ����ͨ�Ŷ˿�Ϊ����ģʽ
         g_SciByteCnt = 0;
      }
   }
}


//============================================================================
// Function    : sciIsrHandle
// Description : SCI�շ��жϺ������ú������ж��е��á�
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




