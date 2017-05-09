#include "common.h"
#include <pic18.h>

volatile uint32_t g_SysTickMs = 0;

void ISR_Init(void)
{
	INTCONbits.GIE_GIEH 	= 1;	//��������δ�����ж�
	INTCONbits.PEIE_GIEL 	= 1;	//��������δ���������ж�
	RCONbits.IPEN			= 1;	//�����ж����ȼ�
}

//interrupt
void interrupt ISR_High_Handler(void) 
{
    SCI_IsrHandle();
}

void DelayMs(uint16_t nTime)
{ 
   uint32_t curTime = g_SysTickMs;

   while((g_SysTickMs - curTime) <= nTime);
}
