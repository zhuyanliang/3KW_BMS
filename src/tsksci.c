#include "include.h"

static uint8_t  s_SciRcvData;
static uint8_t  s_SciErrCnt;
static uint16_t s_SciMsgTimer;

uint8_t  				g_SciState; 
uint32_t 				g_SciTimeStamp; 
ConnectDeviceTypedef 	g_ExternalDevice;


#define  SHANK_HAND_TIMEOUT           30000
#define  COM_DROP_TIMEOUT             100

void Tsk_SCIInit(void)
{  
   s_SciMsgTimer = COM_DROP_TIMEOUT;
   s_SciErrCnt = 0;
   g_SciRxBufFull = 0;

   g_SciState = SHAKE_HAND;
}

//============================================================================
// Function    : SciComDropCheck
// Description : ͨ�ŵ��߼��
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_ComDropCheck(void)
{
   if (g_SciRxBufFull)
   {
      s_SciMsgTimer = COM_DROP_TIMEOUT;
   }
   else
   {
      if (s_SciMsgTimer)
      {
         s_SciMsgTimer--;
      }
      else
      {
         g_SciState = SHAKE_HAND;
         g_ExternalDevice = NONE;
         s_SciMsgTimer = COM_DROP_TIMEOUT;
      }
   }
}


//============================================================================
// Function    : SciRcvMsgCheck
// Description : ������ϢУ��
// Parameters  : none
// Returns     : 1, ��ȷ�յ���Ϣ�� 0, δ�յ���Ϣ����ϢУ��δͨ����
//============================================================================
uint8_t SCI_RcvMsgCheck(void)
{ 
   uint8_t tmp; 

   if (g_SciRxBufFull)
   {
      g_SciRxBufFull = 0; 
      tmp = g_SciRcvBuf[0] + g_SciRcvBuf[1];  // ��Ϣ֡У�鴦��

      if (tmp == 0xFF)
      {
         s_SciRcvData = g_SciRcvBuf[0];
         s_SciErrCnt = 0;        
         return 1;
      }
      else
      {
         /* ͨ�Ź��ϼ�� */
         if (s_SciErrCnt > 30)
         {
            g_SystemWarning.CER = WARNING_SECOND_LEVEL;
         }
         else
         {
            s_SciErrCnt++;
         }
      }
   }

   return(0);
}

//============================================================================
// Function    : SciLoadMsgPackWell
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_LoadMsgPackWell(void)
{
	g_SciSendBuf[0] = SCI_MSG_WELL_STATE;
	g_SciSendBuf[1] = 0;
	g_SciSendBuf[2] = g_SciSendBuf[0] + g_SciSendBuf[1];
}

//============================================================================
// Function    : SciLoadMsgChargeEnd
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_LoadMsgChargeEnd(void)
{
	g_SciSendBuf[0] = SCI_MSG_CHARGE_END;
	g_SciSendBuf[1] = 0;
	g_SciSendBuf[2] = g_SciSendBuf[0] + g_SciSendBuf[1];
}


//============================================================================
// Function    : SciLoadMsgOverDischarge
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_LoadMsgOverDischarge(void)
{
	g_SciSendBuf[0] = SCI_MSG_OVER_DISCHARGE;
	g_SciSendBuf[1] = 0;
	g_SciSendBuf[2] = g_SciSendBuf[0] + g_SciSendBuf[1];
}


//============================================================================
// Function    : SciLoadMsgChargeOverTemp
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_LoadMsgChargeOverTemp(void)
{
	g_SciSendBuf[0] = SCI_MSG_OVER_TEMP_CHARGE;
	g_SciSendBuf[1] = 0;
	g_SciSendBuf[2] = g_SciSendBuf[0] + g_SciSendBuf[1];
}

//============================================================================
// Function    : SciLoadMsgDisChargeOverTemp
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_LoadMsgDisChargeOverTemp(void)
{
	g_SciSendBuf[0] = SCI_MSG_OVER_TEMP_DISCHARGE;
	g_SciSendBuf[1] = 0;
	g_SciSendBuf[2] = g_SciSendBuf[0] + g_SciSendBuf[1];
}

//============================================================================
// Function    : SciLoadMsgTempValue
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
static void SCI_LoadMsgTempValue(void)
{
	uint8_t i,j;
	uint16_t adVal;
	
	g_SciSendBuf[0] = SCI_MSG_TEMPE_VALUE; 

	i = (g_BatteryParameter.MaxTempChnIdx-1)/4;
	if(g_BatteryParameter.MaxTempChnIdx > 4)
	{
		j = (g_BatteryParameter.MaxTempChnIdx - 3);
	}
	else
	{
		j = g_BatteryParameter.MaxTempChnIdx;
	}
	
	adVal = g_ArrayLtc6811Unit.temperature[i][j];
	adVal >>= 4;
	g_SciSendBuf[1] = (uint8_t)adVal;  // ���cell�¶�ֵ ԭʼ��8λADֵ
	
	if ((g_BatteryParameter.CellTempMin < PACK_CHARGE_LOW_TEMP)
		|| (g_BatteryParameter.CellTempMax > PACK_CHARGE_HIGH_TEMP))
	{
		g_SciSendBuf[1] = 80;
	}
	else
	{
		g_SciSendBuf[1] = 100;
	}

   	g_SciSendBuf[2] = g_SciSendBuf[0] + g_SciSendBuf[1];
}


//============================================================================
// Function    : SCI_ShakeHandHandle
// Description : �ϵ�����빤�߻��߳��������ͨ�ŵ������źŷ��͡�
// Parameters  : none
// Returns     : 
//============================================================================
void SCI_ShakeHandHandle(void)
{
   static uint8_t state = 0;
   static uint8_t tmp[3]={0};

   switch (state)
   {
      case 0:
           if (g_SysTickMs - g_SciTimeStamp >= 200) 
           {
              g_SciTimeStamp = g_SysTickMs;
              g_SciSendBuf[0] = tmp[0];
              g_SciSendBuf[1] = tmp[1];
              g_SciSendBuf[2] = tmp[2];
  
              SCI_SetTxMode();
              SCI_StartSend();
              state = 1;
           }              
      break;
 
      case 1:
           if (g_SysTickMs - g_SciTimeStamp >= 50) 
           {
              if (SCI_RcvMsgCheck())
              {
                 if ((s_SciRcvData == SCI_MSG_RESET_CHARGER)
                  || (s_SciRcvData == SCI_MSG_RESET_TOOLS))
                 {
                    g_SciTimeStamp = g_SysTickMs;
                    tmp[0] = 0;
                    tmp[1] = 0;
                    tmp[2] = 0;
                    state = 0;
                    g_ExternalDevice = NONE;
                    return;
                 }

                 switch (g_SciSendBuf[0])
                 {
                    case SCI_MSG_SHAKE_HAND:
                         if ((s_SciRcvData == 0x2D)
                          || (s_SciRcvData == 0x1B))
                         {
                            tmp[0] = SCI_MSG_SERIES_NUM;
                            tmp[1] = CELL_SERIES_NUM;
                            tmp[2] = tmp[0] + tmp[1];

                            g_ExternalDevice = s_SciRcvData;
                         }
                    break;

                    case SCI_MSG_SERIES_NUM:
                         if (g_ExternalDevice == s_SciRcvData)
                         {
                            tmp[0] = SCI_MSG_PACK_CAPACITY;
                            tmp[1] = BATTERY_CAPACITY_TOTAL;
                            tmp[2] = tmp[0] + tmp[1];
                         }
                    break;

                    case SCI_MSG_PACK_CAPACITY:
                         if (g_ExternalDevice == s_SciRcvData)
                         {
                            g_SciState = COM_FOUND;
                            state = 2;
                            return;
                         }
                    default:
                    break;
                 }
              }

              state = 0;
           }
      break;
 
      default:
           g_SciTimeStamp = g_SysTickMs;
           tmp[0] = 0;
           tmp[1] = 0;
           tmp[2] = 0;
           state = 0;
      break;
   }
}

//============================================================================
// Function    : SciSendMsgHandle
// Description : 
// Parameters  : none
// Returns     : none
//============================================================================
void SCI_SendMsgHandle(void)
{
   static uint8_t state = 0;

   switch (g_ExternalDevice)
   {
      /* ����豸�ǳ���� */
      case CHARGER:
           if (g_BatteryMode == PROTECTION)
           {
              SCI_LoadMsgChargeOverTemp();
           }
           else
           {
              if (g_BatteryMode == CHARGE_END)
              {
                 SCI_LoadMsgChargeEnd();
              }
              else
              {
                 if (g_BatteryParameter.CellVoltMin < CELL_UV_FAULT)
                 {
                    SCI_LoadMsgOverDischarge();
                 }
                 else
                 {
                    if (state++ & 0x01)
                    {
                       SCI_LoadMsgPackWell();
                    }
                    else
                    {
                       SCI_LoadMsgTempValue();
                    }
                 }
              }
           }
      break;

      /* ����豸�ǹ��� */
      case TOOLS:
           if (g_BatteryMode == PROTECTION)
           {
              SCI_LoadMsgDisChargeOverTemp();
           }
           else
           {
              if (g_BatteryParameter.SOC)
              {
                 SCI_LoadMsgPackWell();
              }
              else
              {
                 SCI_LoadMsgOverDischarge();
              }
           }
      break;

      default:
      break;
   }
}

//============================================================================
// Function    : SciRcvMsgHandle
// Description : ���ղ����������򹤾ߵ�Ӧ����Ϣ��
// Parameters  : none
// Returns     : none
//============================================================================
void SCI_RcvMsgHandle(void)
{    
   if (SCI_RcvMsgCheck())
   {
      switch (g_ExternalDevice)
      {
         /* ������ͨ��Ӧ�� */
         case CHARGER:
              switch (s_SciRcvData)
              {
                 case SCI_MSG_RESET_CHARGER:  // �����Ҫ����������
                      if (g_BatteryParameter.SOC != 100)
                      {
                         g_SciState = SHAKE_HAND;
                         g_ExternalDevice = NONE;
                         s_SciMsgTimer = COM_DROP_TIMEOUT;
                      }
                 break;

                 case 0x1F:

                 break;

                 case 0x1B:  // �����������Ӧ�ź�
                 default:
                 break;
              }
         break;

         /* �빤��ͨ��Ӧ�� */
         case TOOLS:
              switch (s_SciRcvData)
              {
                 case SCI_MSG_RESET_TOOLS:  // ����Ҫ����������
                      if (g_BatteryParameter.SOC != 0)
                      {
                         g_SciState = SHAKE_HAND;
                         g_ExternalDevice = NONE;
                         s_SciMsgTimer = COM_DROP_TIMEOUT;
                      }
                 break;

                 case 0x2D:

                 default:
                 break;
              }
         default:
         break;
      }
   }
}


//============================================================================
// Function    : SCI_TxRxHandle
// Description : �������ֺ�������豸ͨ�ź�����
// Parameters  : none
// Returns     : none
//============================================================================
void SCI_TxRxHandle(void)
{
   static uint8_t state = 0;

   switch (state)
   {
      case 0:
           if (g_SysTickMs - g_SciTimeStamp >= 200) 
           {
              g_SciTimeStamp = g_SysTickMs;
              SCI_SetTxMode();
              SCI_SendMsgHandle();  
              SCI_StartSend();

              state++;
           }              
      break;

      case 1:
           if (g_SysTickMs - g_SciTimeStamp > 20) 
           {
              SCI_RcvMsgHandle();  // ���ݽ��յ�����Ϣ�͵�ǰpack״̬
                                  // ȷ����һ���ڴ����͵����ݡ�
              state--;
           }
      break;
 
      default:
           g_SciTimeStamp = g_SysTickMs;
           state = 0;
      break;
   }
}


//============================================================================
// Function    : TskSciMgt
// Description : SCIͨ��task����ϵͳ��������ʱ������ⲿ�����豸���ǳ�������
//               ��ͨ�ţ���ֹ��������Ҫ��ͨ�Ź��ϱ�־��
// Parameters  : none
// Returns     : none
//============================================================================
void TskSciMgt(void)
{
   if ((g_ExternalDevice == CHARGER)
    || ((g_BatteryMode != PROTECTION)
     && !((g_ExternalDevice == TOOLS)
       && (g_BatteryParameter.SOC == 0))))
   {
      switch (g_SciState)
      {
         /* ���ⲿ�豸�������֣�ȷ������豸��
            ���߻��ǳ����,�����ǷǱ���˾��Ʒ */
         case SHAKE_HAND:
              SCI_ShakeHandHandle();
         break;

         /* �������ֺ����ⲿ�豸ͨ�� */
         case COM_FOUND:
              SCI_ComDropCheck();
              SCI_TxRxHandle();
         break;

         default:
              g_SciState = COM_FOUND;
         break;
      }
   }
   else if (g_BatteryMode == PROTECTION)
   {
      /* Ϊ�ɻָ����ϻظ��������������׼�� */
      g_SciState = SHAKE_HAND;
      g_ExternalDevice = NONE;
      s_SciMsgTimer = COM_DROP_TIMEOUT;
   }
   else if ((g_ExternalDevice == TOOLS)
         && (g_BatteryParameter.SOC == 0))
   {
      Nop();
   }
}


