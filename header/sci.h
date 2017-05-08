/* 
 * File:   sci.h
 * Author: Yanliang.Zhu
 *
 * Created on 2016年12月24日, 下午1:34
 */

#ifndef SCI_H
#define	SCI_H

#ifdef	__cplusplus
extern "C" {
#endif



#define SCI_TX_PORT     LATDbits.LATD3
#define SCI_RX_PORT     LATBbits.LATB0
#define EnableSCI()		LATDbits.LATD1 = 0b1
#define DisableSCI()	LATDbits.LATD1 = 0b0

    
// 收发器结构体参数定义
typedef struct
{
   uint8_t DR;  	// SCI通信发送寄存器
   uint8_t TBCNT;  	// 发送位计数器
   uint8_t RBCNT;  	// 接收位计数器

   union
   {
      uint8_t Byte;

      struct 
      {
         unsigned TxEF :1;      // 发送缓冲器空，“1”有效
         unsigned RxFF :1;      // 接收缓冲器满, “1”有效
         unsigned TxRx :1;      // "1"发送模式, "0"接收模式
         unsigned      :5; 
      };
   }STAT;

}SciTypedef;





void Sci_Init(void); 
void Sci_StartSend(void);
void Sci_SendByte(void);
void Timer1_IrqEnable(void);
void Timer1_IrqDisable(void);
void SCI_SetClk(uint16_t val);
void Set_TxMode(void);
void Set_RxMod(void);
void Set_IdelMode(void);
void Sci_RcvBits(void);
void TskHandleSciRxBuf(void);
void TskHandleSciBuf(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SCI_H */

