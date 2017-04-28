/* 
 * File:   led.h
 * Author: Administrator
 *
 * Created on 2016年6月20日, 上午9:12
 */

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LedRed        	LATEbits.LATE2   
#define LedGre         	LATDbits.LATD5

#define LedRedOff()			LedRed = 0b1
#define LedGreOff()			LedGre = 0b1

#define LedRedOn()			LedRed = 0b0
#define LedGreOn()			LedGre = 0b0

#define LedGreTrig()        LedGre = ~LedGre

#define Button		LATAbits.LATA1
#define Power		LATDbits.LATD6
    

void Led_Init(void);
void TaskLedMgt(void);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

