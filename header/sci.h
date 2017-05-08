/* 
 * File:   sci.h
 * Author: Yanliang.Zhu
 *
 * Created on 2016��12��24��, ����1:34
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

    
// �շ����ṹ���������
typedef struct
{
   uint8_t DR;  	// SCIͨ�ŷ��ͼĴ���
   uint8_t TBCNT;  	// ����λ������
   uint8_t RBCNT;  	// ����λ������

   union
   {
      uint8_t Byte;

      struct 
      {
         unsigned TxEF :1;      // ���ͻ������գ���1����Ч
         unsigned RxFF :1;      // ���ջ�������, ��1����Ч
         unsigned TxRx :1;      // "1"����ģʽ, "0"����ģʽ
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

