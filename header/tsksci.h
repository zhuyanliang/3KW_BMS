/* 
 * File:   tsksci.h
 * Author: Administrator
 *
 * Created on 2017年5月9日, 下午1:16
 */

#ifndef TSKSCI_H
#define	TSKSCI_H

#ifdef	__cplusplus
extern "C" {
#endif

//============================================================================
/* 定义SCI通信功能码 */
#define  SCI_MSG_SHAKE_HAND             ((uint8_t)0x00)
#define  SCI_MSG_OVER_DISCHARGE         ((uint8_t)0x01)
#define  SCI_MSG_CHARGE_ERROR           ((uint8_t)0x02)
#define  SCI_MSG_TEMPE_VALUE            ((uint8_t)0x03)
#define  SCI_MSG_OVER_TEMP_CHARGE       ((uint8_t)0x04)
#define  SCI_MSG_SERIES_NUM             ((uint8_t)0x05)
#define  SCI_MSG_PACK_CAPACITY          ((uint8_t)0x06)
#define  SCI_MSG_WELL_STATE             ((uint8_t)0x07)
#define  SCI_MSG_CHARGE_END             ((uint8_t)0x08)
#define  SCI_MSG_OVER_TEMP_DISCHARGE    ((uint8_t)0x09)
#define  SCI_MSG_BAD_CELL_DETECTED      ((uint8_t)0x0A)
#define  SCI_MSG_RESET_CHARGER          ((uint8_t)0x1E)
#define  SCI_MSG_RESET_TOOLS            ((uint8_t)0x2E)

//============================================================================

typedef enum
{
   SHAKE_HAND = 0,
   COM_FOUND,
   SHAKE_HAND_FAIL
}SciStateTypedef;

/* 外接设备类型定义 */
typedef enum
{
   NONE = 0,
   TOOLS = 0x2D,
   CHARGER = 0x1B
}ConnectDeviceTypedef;


extern uint8_t  			g_SciState; 
extern uint32_t 			g_SciTimeStamp; 
extern ConnectDeviceTypedef g_ExternalDevice;


//============================================================================
/* Function Prototype */
void Tsk_SCIInit(void);
uint8_t SCI_RcvMsgCheck(void);
void SCI_ShakeHandHandle(void);
void SCI_SendMsgHandle(void);
void SCI_RcvMsgHandle(void);
void SCI_TxRxHandle(void);
void TskSciMgt(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TSKSCI_H */

