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

static void Lcd_Init(void)
{
	TRISDbits.TRISD0 = 0b0;		// Lcd�����������
	TRISDbits.TRISD1 = 0b0;		// Lcd����оƬ�����ʹ������
	TRISDbits.TRISD2 = 0b0;		// Lcd_Bp
	
}

void Gpio_Init(void)
{
	Led_Init();
	Lcd_Init();
	
	TRISAbits.TRISA1 = 0b1;   // ��ť�������
	TRISAbits.TRISA2 = 0b1;   // ��·�����ź�

	TRISBbits.TRISB4 = 0b0;		// output 2
	TRISBbits.TRISB5 = 0b0;		// output 1
	TRISCbits.TRISC0 = 0b0;		// ������
	TRISDbits.TRISD4 = 0b0;		// ������λ���ƹܽ�
	TRISDbits.TRISD6 = 0b0;   	// ʹ�ܵ�Դ��������
	TRISDbits.TRISD7 = 0b0;		// ���ʹ�ܹܽ�
	TRISEbits.TRISE1 = 0b0;		// �ŵ�ʹ�ܹܽ�	
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




