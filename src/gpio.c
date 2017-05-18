#include "include.h"

/*
 * 简单IO口控制
 * Led Button
 *
 *
 */

static void Led_Init(void)
{
	TRISDbits.TRISD5 = 0b0;   // PCB板绿灯控制端口
	TRISEbits.TRISE2 = 0b0;   // PCB板红灯控制端口
	
    LedGreOn();
    LedRedOff();
}

void Gpio_Init(void)
{
	Led_Init();
	
	TRISAbits.TRISA1 = 0b1;   // 按钮检测引脚
	TRISAbits.TRISA2 = 0b1;   // 短路保护信号

	TRISBbits.TRISB4 = 0b0;		// output 2
	TRISBbits.TRISB5 = 0b0;		// output 1
	TRISCbits.TRISC0 = 0b0;		// 蜂鸣器
	TRISDbits.TRISD4 = 0b0;		// 过流复位控制管脚
	TRISDbits.TRISD6 = 0b0;   	// 使能电源控制引脚
	TRISDbits.TRISD7 = 0b0;		// 充电使能管脚
	TRISEbits.TRISE1 = 0b0;		// 放电使能管脚

	if(Button)
		PowerOn();
	BeepOff();
}

//============================================================================
// Function    : SysStateLedMgt
// Description : 该函数为系统状态LED灯指示，包括系统正常运行、报警、故障等。
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

