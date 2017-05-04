/* 
 * File:   lcd.h
 * Author: Administrator
 *
 * Created on 2016年12月1日, 上午8:55
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "include.h"

#define LcdBack 		LATDbits.LATD0
#define LatchOutput 	LATDbits.LATD1	// 74HC595锁存控制管脚
#define LCDBP 			LATDbits.LATD2	// LCD common管脚，该管脚需要加50Hz方波

#define LCD_BackLightOn()	LcdBack 	= 0b1
#define LCD_BackLightOff()	LcdBack 	= 0b0
#define LcdOutEn()			LatchOutput = 0b1
#define LcdOutDisEn()		LatchOutput = 0b0
#define LcdBpEn()			LCDBP 		= 0b1
#define LcdBpDisEn()		LCDBP 		= 0b0


void Lcd_Init(void);    
void LCD_DisplayDriver(void);
void TskLcdShow(void);

    
extern uint8_t g_LcdBuf[2];
extern uint8_t g_LcdCharTable[19];


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

