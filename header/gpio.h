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

#define Button		PORTAbits.RA1//LATAbits.LATA1
#define Beep		LATCbits.LATC0
#define OCReset 	LATDbits.LATD4
#define Power		LATDbits.LATD6
#define Charge		LATDbits.LATD7
#define Discharge	LATEbits.LATE1


#define BeepOn()		Beep = 0b1
#define BeepOff()		Beep = 0b0
#define OCResetHight	OCReset = 0b1
#define OCResetLow		OCReset = 0b0
#define PowerOn()		Power = 0b1
#define PowerOff()		Power = 0b0
#define ChargeEn()		Charge = 0b1
#define ChargeDisEn()	Charge = 0b0
#define DischargeEn()	Discharge = 0b1
#define DischargeDisEn() Discharge = 0b0

#define BeepTrig()		Beep = ~Beep 

void Gpio_Init(void);
void TaskLedMgt(void);
void TskBeepMgt(void);
void TskMOSFETMgt(void);


#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

