/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2016年6月20日, 上午8:14 
 */

#include "include.h"

// PIC18F46K80 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1L
#pragma config RETEN = OFF      // VREG Sleep Enable bit (Ultra low-power regulator is Disabled (Controlled by REGSLP bit))
#pragma config INTOSCSEL = LOW // LF-INTOSC Low-power Enable bit (LF-INTOSC in High-power mode during Sleep)
#pragma config SOSCSEL = DIG   // SOSC Power Selection and mode Configuration bits (High Power SOSC circuit selected)
#pragma config XINST = OFF       // Extended Instruction Set (Enabled)

// CONFIG1H
#pragma config FOSC = HS2
//#pragma config FOSC = INTIO2    // Oscillator (Internal RC oscillator)
#pragma config PLLCFG = ON     // PLL x4 Enable bit (Disabled)
#pragma config FCMEN = ON      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = OFF       // Internal External Oscillator Switch Over Mode (Disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power Up Timer (Disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Enabled in hardware, SBOREN disabled)
#pragma config BORV = 0         
#pragma config BORPWR = LOW 

// CONFIG2H
#pragma config WDTEN = ON  		// Watchdog Timer (WDT enabled in hardware; SWDTEN bit disabled)
#pragma config WDTPS = 10    // Watchdog Postscaler (1:1048576) 喂狗时间 4Ms * 10

// CONFIG3H
#pragma config CANMX = PORTB    // ECAN Mux bit (ECAN TX and RX pins are located on RB2 and RB3, respectively)
#pragma config MSSPMSK = MSK7   // MSSP address masking (7 Bit address masking mode)
#pragma config MCLRE = ON       // Master Clear Enable (MCLR Enabled, RE3 Disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset (Enabled)
#pragma config BBSIZ = BB2K     // Boot Block Size (2K word Boot Block size)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protect 00800-03FFF (Disabled)
#pragma config CP1 = OFF        // Code Protect 04000-07FFF (Disabled)
#pragma config CP2 = OFF        // Code Protect 08000-0BFFF (Disabled)
#pragma config CP3 = OFF        // Code Protect 0C000-0FFFF (Disabled)

// CONFIG5H
#pragma config CPB = OFF        // Code Protect Boot (Disabled)
#pragma config CPD = OFF        // Data EE Read Protect (Disabled)

// CONFIG6L
#pragma config WRT0 = OFF       // Table Write Protect 00800-03FFF (Disabled)
#pragma config WRT1 = OFF       // Table Write Protect 04000-07FFF (Disabled)
#pragma config WRT2 = OFF       // Table Write Protect 08000-0BFFF (Disabled)
#pragma config WRT3 = OFF       // Table Write Protect 0C000-0FFFF (Disabled)

// CONFIG6H
#pragma config WRTC = OFF       // Config. Write Protect (Disabled)
#pragma config WRTB = OFF       // Table Write Protect Boot (Disabled)
#pragma config WRTD = OFF       // Data EE Write Protect (Disabled)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protect 00800-03FFF (Disabled)
#pragma config EBTR1 = OFF      // Table Read Protect 04000-07FFF (Disabled)
#pragma config EBTR2 = OFF      // Table Read Protect 08000-0BFFF (Disabled)
#pragma config EBTR3 = OFF      // Table Read Protect 0C000-0FFFF (Disabled)

// CONFIG7H
#pragma config EBTRB = OFF      // Table Read Protect Boot (Disabled)

#define EnableWatchDog()        WDTCONbits.SWDTE = 0b1
#define DisableWatchDog()       WDTCONbits.SWDTE = 0b0

#define DEBUG

BOOL g_startUp = TRUE;

void System_Init(void);

//============================================================================
// Function    ：SysClk_Init
// Description ：Initialize the system clock Fosc.
// Parameters  ：none 
// Returns     ：none
//============================================================================
void SysClk_Init(void)
{   
    OSCCONbits.SCS 		= 0b00;     // 默认主振荡器(OSC或者HF-INTOSC)

    //OSCCONbits.IRCF 	= 0b111;    // 设置所选HF_INTOSC输出频率(16MHz)
    //OSCTUNEbits.TUN 	= 0b000000;		//快速RC振荡器(INTOSC)频率调节 暂时不调节，使用默认校验好的

    OSCCONbits.IDLEN 	= 0b0;      // 执行SLEEP指令后进入休眠模式
    OSCTUNEbits.PLLEN 	= 0b1;      //enable PLL  系统时钟运行在16MHz*4 
}


//============================================================================
// Function    ：GPIO_Init
// Description ：单片机部分IO管脚初始化，部分IO的初始化可能在相应模块初始化函
//               数中执行
// Parameters  ：none 
// Returns     ：none
//============================================================================ 
void GPIO_Init(void)
{
	TRISDbits.TRISD4 = 1;  // 设置keyrun脚为输入
	TRISDbits.TRISD5 = 1;  // No Used
	TRISDbits.TRISD6 = 1;  // 预留输入
	TRISDbits.TRISD7 = 1;  //充电器充电检测引脚
}

void TRIG_TEST(void)
{
	LATDbits.LATD0 ^= 0b1;   
}

static void ShutDown_1MsTick(void)
{
	TMR1H = 0x00;
	TMR1L = 0x00;
	
	PIR1bits.TMR1IF	= 0b0;	// 清除外设中断请求寄存器定时器1的中断标志位
	PIE1bits.TMR1IE = 0b0;	// 不允许定时器1的中断
}

/*
 * 
 */
void main(void) 
{
	static uint8_t taskList = 0;
	static uint8_t cnt = 0;
	
    System_Init();    
	
	//EnableWatchDog();
	TskLcdShow();

	g_startUp = FALSE;
	ShutDown_1MsTick();

    for(;;)
    {
    	g_SysTickMs += 2;
		// 查询优先级较高任务
		TskCurrentMgt();
		Soc_AhAcc();
		Soh_ChargeAhAcc();
		ClrWdt();
		DetectCharger();
		TskBatteryModeMgt();
		TskCanRecMsgToBuf();

		if(16 == cnt++)
		{
			cnt = 0;
			CAN_SendHeartToTxBuf();
			CAN_SendSTDBattInfoToTxBuf();
		}
		
        switch(taskList++)
        {
		case 0:
			TskAfeMgt();
			break;
		case 1:	
			TskSOCMgt();
			break;
		case 2:
            TskCanMgt();
			TskSohMgt();
			break;
		case 3:
			TskCellTempMgt();
			TskLcdShow();
			TskSciMgt();
			break;
		case 4:
            TskFaultStoreMgt(); 
			TaskLedMgt();
			LCD_DisplayDriver();
			taskList = 0;
			break;
		default:
			taskList = 0;
			break;	
        }
        while(!PIR4bits.TMR4IF);
        PIR4bits.TMR4IF = 0;       
        ClrWdt();
    }
}

void System_Init(void)
{
	ClrWdt();
	SysClk_Init();  // 系统时钟初始化

	ISR_Init();     // 中断处理程序初始化
	Timer_Init();
	ADC_IOInit();   // ADC初始化
	
	SPI_Init();      // SPI初始化
	Gpio_Init();
	Lcd_Init();
	ECAN_Init();
	Sci_Init();

	ClrWdt();

	TskBatteryPra_Init();	// 电池部分参数的初始化
	TskCan_Init();
	LTC6811_Initialize();
    TskAdc_Init();			// 模拟模块参数初始化
	TskCan_Init();
	Soc_Init();
	Soh_Init();
	FaultStoreInit();
	Tsk_SCIInit();
	ClrWdt();

	g_BatteryMode 			= IDLE;
	g_ProtectDelayCnt 		= 0xffff;
	g_SystemWarning.all 	= 0;    // clear system warning flags

	SystemSelftest();  		// 系统自检
	Soc_PowerOnAdjust();	// SOC 上电校准
	
}

