#include "include.h"

uint8_t g_LcdBuf[2] = {0};

uint8_t g_LcdCharTable[19]=
{
    0xFA,   // '0'
    0x60,   // '1'
    0xDC,   // '2'
    0xF4,   // '3' 
    0x66,   // '4' 
    0xB6,   // '5'
    0xBE,   // '6' 
    0xE0,   // '7' 
    0xFE,   // '8' 
    0xF6,   // '9' 
    0xEE,   // 'A' 
    0x3E,   // 'b' 
    0x9A,   // 'C' 
    0x7C,   // 'd' 
    0x9E,   // 'E' 
    0x8E,   // 'F'
    0x6E,   // "H"
    0x1A,   // "L"
    0x3C,   // "o"
};

void Lcd_Init(void)
{
    // LCD相关引脚初始化

	TRISDbits.TRISD0 = 0b0;		// Lcd背光控制引脚
	TRISDbits.TRISD1 = 0b0;		// Lcd控制芯片的输出使能引脚
	TRISDbits.TRISD2 = 0b0;		// Lcd_Bp
    
    LCD_BackLightOn();  		// 点亮背光灯

    LcdOutDisEn();  			// 74HC595锁存输出拉低，为产生上升沿做准备

    g_LcdBuf[0] = g_LcdCharTable[15];
    g_LcdBuf[1] = g_LcdCharTable[15];
    
    SPI_SendByte(g_LcdBuf[0]);
    SPI_SendByte(g_LcdBuf[1]);
    
    LcdOutEn(); 
}


void LCD_DisplayDriver(void)
{
    static uint8_t cnt = 0;
    uint8_t tmp[2];
    
    if (cnt & 0x01)
	{
		tmp[0] = g_LcdBuf[0] ^ 0xFF;
		tmp[1] = g_LcdBuf[1] ^ 0xFF;
	}
	else
	{
		tmp[0] = g_LcdBuf[0];
		tmp[1] = g_LcdBuf[1];
	}
    
	/* 刷新显示 */
	LatchOutput = 0;  // 74HC595锁存输出拉低，为产生上升沿做准备   
	SPI_SendByte(tmp[0]);
	SPI_SendByte(tmp[1]);
	LatchOutput = 1;  // 输出拉高，产生74HC595锁存上升沿信号
    LCDBP = cnt++;
}



void TskLcdShow(void)
{
	static uint8_t cnt;

    g_LcdBuf[0] = g_LcdCharTable[g_BatteryParameter.SOC/10];
    g_LcdBuf[1] = g_LcdCharTable[g_BatteryParameter.SOC%10];
	
	
}


