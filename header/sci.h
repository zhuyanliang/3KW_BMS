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

#define  MESSAGE_SEND_LEN                  3
#define  MESSAGE_RECEIVE_LEN               2

#define SCI_TX_PORT     LATDbits.LATD3
#define SCI_RX_PORT     LATBbits.LATB0
#define EnableSCI()		LATDbits.LATD1 = 0b1
#define DisableSCI()	LATDbits.LATD1 = 0b0

    
// �շ����ṹ���������
typedef struct
{
	uint16_t 		DR;  				// SCIͨ�ŷ��ͼĴ���
	uint8_t 		TBCNT;  			// ����λ������
	uint8_t 		RBCNT;  			// ����λ������

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

/* Valuable Definition */
extern SciTypedef g_SCI;
extern uint8_t g_SciSendBuf[MESSAGE_SEND_LEN];
extern uint8_t g_SciRcvBuf[MESSAGE_RECEIVE_LEN];
extern uint8_t g_SciByteCnt;
extern uint8_t g_SciRxBufFull;


void Sci_Init(void); 
void SCI_SetClk(uint16_t val);
void SCI_WriteByte(uint8_t data);
void SCI_TimerIrqEnable(void);
void SCI_TimerIrqDisable(void);
uint8_t SCI_ReadByte(void);
void SCI_SetIdelMode(void);
void SCI_SetTxMode(void);
void SCI_SetRxMode(void);
void SCI_StartSend(void);
void SCI_SendByte(void);
void SCI_RcvBits(void);
void SCI_SendMsg(void);
void SCI_RcvMsg(void);
void SCI_IsrHandle(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SCI_H */

