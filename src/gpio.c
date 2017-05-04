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




