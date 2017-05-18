#include "include.h"

/*
 * ��IO�ڿ���
 * Led Button
 *
 *
 */

static void Led_Init(void)
{
	TRISDbits.TRISD5 = 0b0;   // PCB���̵ƿ��ƶ˿�
	TRISEbits.TRISE2 = 0b0;   // PCB���ƿ��ƶ˿�
	
    LedGreOn();
    LedRedOff();
}

void Gpio_Init(void)
{
	Led_Init();
	
	TRISAbits.TRISA1 = 0b1;   // ��ť�������
	TRISAbits.TRISA2 = 0b1;   // ��·�����ź�

	TRISBbits.TRISB4 = 0b0;		// output 2
	TRISBbits.TRISB5 = 0b0;		// output 1
	TRISCbits.TRISC0 = 0b0;		// ������
	TRISDbits.TRISD4 = 0b0;		// ������λ���ƹܽ�
	TRISDbits.TRISD6 = 0b0;   	// ʹ�ܵ�Դ��������
	TRISDbits.TRISD7 = 0b0;		// ���ʹ�ܹܽ�
	TRISEbits.TRISE1 = 0b0;		// �ŵ�ʹ�ܹܽ�

	if(Button)
		PowerOn();
	BeepOff();
}

//============================================================================
// Function    : SysStateLedMgt
// Description : �ú���Ϊϵͳ״̬LED��ָʾ������ϵͳ�������С����������ϵȡ�
// Parameters  : none
// Returns     : none
//============================================================================
void TaskLedMgt(void)
{
	static uint8_t timeStamp = 0;

	if(timeStamp++ >= 50)
	{
		timeStamp = 0;
        LedGreTrig();
	}
	
}

void TskBeepMgt(void)
{
	static uint8_t timeStamp = 0;

	if(g_BatteryMode != PROTECTION)
	{
		timeStamp = 0;
		BeepOff();
		return;
	}
	timeStamp++;
	if(timeStamp <= 50)
	{
		BeepOn();
	}
	else if(timeStamp <= 100)
	{
		BeepOff();
	}
	else
	{
		timeStamp = 0;
	}
}


void TskMOSFETMgt(void)
{
#if 0
	if((IDLE == g_BatteryMode) || (PROTECTION == g_BatteryMode))
	{
		ChargeDisEn();
		DischargeDisEn();
	}
	else if(DISCHARGE == g_BatteryMode)
	{
		DischargeEn();
	}
	else if(CHARGE == g_BatteryMode)
	{
		ChargeEn();
	}
#endif
	if(Button)
	{
		ChargeEn();
		DischargeEn();
	}
	else
	{
		ChargeDisEn();
		DischargeDisEn();
		PowerOff();
	}
}

