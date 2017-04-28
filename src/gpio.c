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
	
	TRISAbits.TRISA1 = 0b0;   // 按钮检测引脚
	TRISDbits.TRISD6 = 0b0;   // 使能电源控制引脚
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




