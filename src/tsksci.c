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
// Description : 通信掉线检测
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
// Description : 接收消息校验
// Parameters  : none
// Returns     : 1, 正确收到消息； 0, 未收到消息或消息校验未通过。
//============================================================================
uint8_t SCI_RcvMsgCheck(void)
{ 
   uint8_t tmp; 

   if (g_SciRxBufFull)
   {
      g_SciRxBufFull = 0; 
      tmp = g_SciRcvBuf[0] + g_SciRcvBuf[1];  // 消息帧校验处理

      if (tmp == 0xFF)
      {
         s_SciRcvData = g_SciRcvBuf[0];
         s_SciErrCnt = 0;        
         return 1;
      }
      else
      {
         /* 通信故障检测 */
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
	g_SciSendBuf[1] = (uint8_t)adVal;  // 最大cell温度值 原始的8位AD值
	
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
// Description : 上电初期与工具或者充电器建立通信的握手信号发送。
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
      /* 外接设备是充电器 */
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

      /* 外接设备是工具 */
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
// Description : 接收并处理充电器或工具的应答消息。
// Parameters  : none
// Returns     : none
//============================================================================
void SCI_RcvMsgHandle(void)
{    
   if (SCI_RcvMsgCheck())
   {
      switch (g_ExternalDevice)
      {
         /* 与充电器通信应答 */
         case CHARGER:
              switch (s_SciRcvData)
              {
                 case SCI_MSG_RESET_CHARGER:  // 充电器要求重新握手
                      if (g_BatteryParameter.SOC != 100)
                      {
                         g_SciState = SHAKE_HAND;
                         g_ExternalDevice = NONE;
                         s_SciMsgTimer = COM_DROP_TIMEOUT;
                      }
                 break;

                 case 0x1F:

                 break;

                 case 0x1B:  // 充电器正常响应信号
                 default:
                 break;
              }
         break;

         /* 与工具通信应答 */
         case TOOLS:
              switch (s_SciRcvData)
              {
                 case SCI_MSG_RESET_TOOLS:  // 工具要求重新握手
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
// Description : 建立握手后与外接设备通信函数。
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
              SCI_RcvMsgHandle();  // 根据接收到的信息和当前pack状态
                                  // 确定下一周期待发送的数据。
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
// Description : SCI通信task，当系统产生故障时，如果外部连接设备不是充电器则关
//               闭通信，防止产生不必要的通信故障标志。
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
         /* 与外部设备建立握手，确定外接设备是
            工具还是充电器,或者是非本公司产品 */
         case SHAKE_HAND:
              SCI_ShakeHandHandle();
         break;

         /* 建立握手后与外部设备通信 */
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
      /* 为可恢复故障回复后的重新握手做准备 */
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


