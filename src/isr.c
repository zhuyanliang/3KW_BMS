#include "common.h"
#include <pic18.h>

volatile uint32_t g_SysTickMs = 0;

void ISR_Init(void)
{
	INTCONbits.GIE_GIEH 	= 1;	//允许所有未屏蔽中断
	INTCONbits.PEIE_GIEL 	= 1;	//允许所有未屏蔽外设中断
	RCONbits.IPEN			= 1;	//允许中断优先级
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
