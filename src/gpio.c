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
	
	TRISAbits.TRISA1 = 0b0;   // ��ť�������
	TRISDbits.TRISD6 = 0b0;   // ʹ�ܵ�Դ��������
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




