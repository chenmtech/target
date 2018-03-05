/**************************************************************************************************
* CMTechTempHumid.c: Ӧ����Դ�ļ�
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "osal_snv.h"

#include "OnBoard.h"
//#include "hal_adc.h"
#include "hal_key.h"

#include "gatt.h"

#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "Service_TempHumid.h"

#if defined ( PLUS_BROADCASTER )
  #include "peripheralBroadcaster.h"
#else
  #include "peripheral.h"
#endif

#include "gapbondmgr.h"

#include "App_GAPConfig.h"

#include "App_GATTConfig.h"

#include "App_TempHumid.h"

#include "CMTechTempHumid.h"

#if defined FEATURE_OAD
  #include "oad.h"
  #include "oad_target.h"
#endif

#include "App_DataProcessor.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */


#define INVALID_CONNHANDLE                    0xFFFF

#if defined ( PLUS_BROADCASTER )
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif


// ����ģʽ������ģʽ�ʹ���ģʽ
#define MODE_ACTIVE         0             // ��ʾ����ģʽ
#define MODE_STANDBY        1             // ��ʾ����ģʽ


// ��ʾ�ϴ�����¶�ֵ�ĳ���ʱ�䣬Ĭ��3��
#define THERMO_LASTMAXTEMP_SHOWTIME           3000

// ��ʾԤ���¶�ֵ�ĳ���ʱ�䣬20��
#define THERMO_SHOW_PRETEMP                   20000

// Ĭ�ϵĴ������ڣ�ÿ1�����ݴ���һ��
#define DEFAULT_TRANSMIT_PERIOD         1

/* Ative delay: 125 cycles ~1 msec */
#define ST_HAL_DELAY(n) st( { volatile uint32 i; for (i=0; i<(n); i++) { }; } )


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 CMTechThermometer_TaskID;   // Task ID for internal task/event processing

static gaprole_States_t gapProfileState = GAPROLE_INIT;


// ��ǰ����ģʽ����ʼ��Ϊ����ģʽ
static uint8 curMode = MODE_STANDBY;

// �Ƿ�ʼAD�ɼ�����ʼ��Ϊֹͣ�ɼ�
static bool thermoADEnabled = FALSE;

// ���ݲɼ����ڣ��̶�Ϊ����ļ�ʱ��λ1��
static uint16 ADPeriod = THERMOMETER_TIME_UNIT;

// �������ݴ������ڣ���ADPeriod�ı�������ʾ��Ĭ��Ϊ1��
static uint8 transNumOfADPeriod = DEFAULT_TRANSMIT_PERIOD;

// ���ڼ�¼�����������Ƿ���Ҫͨ��������������
static uint8 haveSendNum = 0;

// ��������������
static uint8 bzTimes = 0;



/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void CMTechThermometer_ProcessOSALMsg( osal_event_hdr_t *pMsg );

static void CMTechThermometer_HandleKeys( uint8 shift, uint8 keys );

static void peripheralStateNotificationCB( gaprole_States_t newState );

static void thermometerServiceCB( uint8 paramID );

// ��ʼ��Ϊ����ģʽ
static void initAsStandbyMode();

// �Ӵ���ģʽת��������ģʽ
static void switchFromStandbyToActive( void );

// �ӿ���ģʽת��������ģʽ
static void switchFromActiveToStandby( void );

// ��ȡ����������
static void readAndProcessThermoData();

// ��ʼ��IO�ܽ�
static void initIOPin();

// �÷�������ָ������
static void turnOnTone(uint8 times);


/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t CMTechThermometer_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  NULL                            // When a valid RSSI is read from controller (not used by application)
};

// GAP Bond Manager Callbacks
static gapBondCBs_t CMTechThermometer_BondMgrCBs =
{
  NULL,                     // Passcode callback (not used by application)
  NULL                      // Pairing / Bonding state Callback (not used by application)
};

static thermometerServiceCBs_t CMTechThermometer_ServCBs =
{
  thermometerServiceCB    // Charactersitic value change callback
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEPeripheral_Init
 *
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void CMTechTempHumid_Init( uint8 task_id )
{
  CMTechThermometer_TaskID = task_id;

  // GAP ����
  //���ù㲥����
  GAPConfig_SetAdvParam(800, THERMOMETER_SERV_UUID);
  // ��ʼ�����㲥
  GAPConfig_EnableAdv(FALSE);

  //�������Ӳ���
  GAPConfig_SetConnParam(200, 200, 5, 10000, 1);

  //����GGS�������豸��
  GAPConfig_SetGGSParam("Thermometer");

  //���ð󶨲���
  GAPConfig_SetBondingParam(0, GAPBOND_PAIRING_MODE_WAIT_FOR_REQ);

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  DevInfo_AddService();                           // Device Information Service
  
#if defined FEATURE_OAD
  VOID OADTarget_AddService();                    // OAD Profile
#endif
  
  GATTConfig_SetThermoService(&CMTechThermometer_ServCBs);

  RegisterForKeys( CMTechThermometer_TaskID );

  //�������ʼ��GPIO
  //��һ�����йܽţ�reset���״̬�������������
  //�ڶ������ڲ��õ�IO�����鲻���ӵ��ⲿ��·������Ϊ��������
  //���������ڻ��õ���IO����Ҫ���ݾ����ⲿ��·�������������Ч���ã���ֹ�ĵ�
  {
    // For keyfob board set GPIO pins into a power-optimized state
    // Note that there is still some leakage current from the buzzer,
    // accelerometer, LEDs, and buttons on the PCB.
    
    // Register for all key events - This app will handle all key events
    
    initIOPin();
  }

  Thermo_Init();
  
  // ��ʼ��Ϊ����ģʽ
  initAsStandbyMode();  
 
  HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );  

  // Setup a delayed profile startup
  osal_set_event( CMTechThermometer_TaskID, TH_START_DEVICE_EVT );
}


// ��ʼ��IO�ܽ�
static void initIOPin()
{
  // ȫ����ΪGPIO
  P0SEL = 0; // Configure Port 0 as GPIO
  P1SEL = 0; // Configure Port 1 as GPIO
  P2SEL = 0; // Configure Port 2 as GPIO
//
//  // ����P0.0��P0.1����Ϊbutton����ߵ�ƽ������ȫ����Ϊ����͵�ƽ
  P0DIR = 0xFC; // Port 0 pins P0.0 and P0.1 as input (buttons),
//                // all others (P0.1-P0.7) as output
  P1DIR = 0xFF; // All port 1 pins (P1.0-P1.7) as output
  P2DIR = 0x1F; // All port 1 pins (P2.0-P2.4) as output
//  
  P0 = 0x03; // All pins on port 0 to low except for P0.0 and P0.1(buttons)
  P1 = 0;   // All pins on port 1 to low
  P2 = 0;   // All pins on port 2 to low   
  
  // I2C��SDA, SCL����ΪGPIO, ����͵�ƽ�����򹦺ĺܴ�
  I2CWC = 0x83;
  I2CIO = 0x00;
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 CMTechTempHumid_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( CMTechThermometer_TaskID )) != NULL )
    {
      CMTechThermometer_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & TH_START_DEVICE_EVT )
  {    
    // Start the Device
    VOID GAPRole_StartDevice( &CMTechThermometer_PeripheralCBs );

    // Start Bond Manager
    VOID GAPBondMgr_Register( &CMTechThermometer_BondMgrCBs );
    
    switchFromStandbyToActive();

    return ( events ^ TH_START_DEVICE_EVT );
  }
  
  if ( events & TH_PERIODIC_EVT )
  {
    if(thermoADEnabled)
    {
      readAndProcessThermoData();
      osal_start_timerEx( CMTechThermometer_TaskID, TH_PERIODIC_EVT, ADPeriod );
    }
    else
    {
      // ֹͣAD�ɼ�
      //Thermo_TurnOff_AD();
    }

    return (events ^ TH_PERIODIC_EVT);
  }

  
  // �궨
  if ( events & TH_CALIBRATION_EVT )
  {
    Thermo_DoCalibration();

    return (events ^ TH_CALIBRATION_EVT);
  }  
  
  // �л�����ģʽ
  if ( events & TH_SWITCH_MODE_EVT )
  {
    if(curMode == MODE_ACTIVE)
    {
      switchFromActiveToStandby();
    }
    else
    {
      switchFromStandbyToActive();
    }    

    return (events ^ TH_SWITCH_MODE_EVT);
  }  
  
  // Ԥ���¶��¼�
  if ( events & TH_DP_PRECAST_EVT )
  {
    turnOnTone(1);
    
    Thermo_SetPreTemp(getPrecastTemp());
    Thermo_SetShowPreTemp(TRUE);
    
    osal_start_timerEx(CMTechThermometer_TaskID, TH_STOP_SHOW_PRETEMP_EVT, THERMO_SHOW_PRETEMP);
  
    return (events ^ TH_DP_PRECAST_EVT);
  }  
  
  // ֹͣ��ʾԤ���¶��¼�
  if ( events & TH_STOP_SHOW_PRETEMP_EVT )
  {
    Thermo_SetShowPreTemp(FALSE);
  
    return (events ^ TH_STOP_SHOW_PRETEMP_EVT);
  }  
  
  
  // �����ȶ��¼�
  if ( events & TH_DP_STABLE_EVT )
  {
    turnOnTone(3);
    
    notifyTempStable();
  
    return (events ^ TH_DP_STABLE_EVT);
  }    
  
  if ( events & TH_TONE_ON_EVT )
  {
    Thermo_ToneOn();
    
    osal_start_timerEx(CMTechThermometer_TaskID, TH_TONE_OFF_EVT, 500 );
  
    return (events ^ TH_TONE_ON_EVT);
  }   
  
  if ( events & TH_TONE_OFF_EVT )
  {
    Thermo_ToneOff();
    
    if(--bzTimes > 0)
    {
      osal_start_timerEx(CMTechThermometer_TaskID, TH_TONE_ON_EVT, 500 );
    }
  
    return (events ^ TH_TONE_OFF_EVT);
  }    
  
  // Discard unknown events
  return 0;
}




/*********************************************************************
 * @fn      simpleBLEPeripheral_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void CMTechThermometer_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case KEY_CHANGE:
      CMTechThermometer_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;

    default:
      // do nothing
      break;
  }
}



#if defined( CC2540_MINIDK )
/*********************************************************************
 * @fn      CMTechThermometer_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void CMTechThermometer_HandleKeys( uint8 shift, uint8 keys )
{
  VOID shift;  // Intentionally unreferenced parameter

  if ( keys & HAL_KEY_SW_1 )
  {
    osal_set_event( CMTechThermometer_TaskID, TH_SWITCH_MODE_EVT);

  } 
}
#endif // #if defined( CC2540_MINIDK )



static void peripheralStateNotificationCB( gaprole_States_t newState )
{
  switch ( newState )
  {
    case GAPROLE_STARTED:
      {
        uint8 ownAddress[B_ADDR_LEN];
        uint8 systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
      }
      break;

    case GAPROLE_ADVERTISING:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Advertising",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_CONNECTED:


      break;

    case GAPROLE_WAITING:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Disconnected",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
          
        // �Ͽ�����ʱ��ֹͣAD�ɼ�
        //Thermo_TurnOff_AD();
      }
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Timed Out",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_ERROR:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Error",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    default:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

  }
  
  gapProfileState = newState;

}


static void thermometerServiceCB( uint8 paramID )
{
  uint8 newValue;

  switch (paramID)
  {
    case THERMOMETER_CONF:
      Thermometer_GetParameter( THERMOMETER_CONF, &newValue );
      
      if ( newValue == THERMOMETER_CONF_STANDBY)  // ֹͣ�ɼ����������ģʽ
      {
        switchFromActiveToStandby();
      }
      
      else if ( newValue == THERMOMETER_CONF_CALIBRATION) // ���б궨
      {
        osal_set_event( CMTechThermometer_TaskID, TH_CALIBRATION_EVT);
      }
      
      else if ( newValue == THERMOMETER_CONF_LCDON) // ��LCD
      {
        Thermo_TurnOn_LCD();
      }   
      
      else if ( newValue == THERMOMETER_CONF_LCDOFF) // ��LCD
      {
        Thermo_TurnOff_LCD();
      }  
      
      else // ʣ�µľ���������������
      { 
        Thermo_SetValueType(newValue);
      }
      
      break;

    case THERMOMETER_PERI:
      Thermometer_GetParameter( THERMOMETER_PERI, &newValue );
      transNumOfADPeriod = newValue;
      break;

    default:
      // Should not get here
      break;
  }
}


// ��ʼ��Ϊ����ģʽ
static void initAsStandbyMode()
{
  curMode = MODE_STANDBY;
  
  // ��������Ϊ1��
  ADPeriod = THERMOMETER_TIME_UNIT;
  
  // ��������Ϊ�������ڵ�1��
  transNumOfADPeriod = DEFAULT_TRANSMIT_PERIOD;  
  
  // ��ʼ����������
  // �¶�Ϊ0
  uint8 thermoData[THERMOMETER_DATA_LEN] = { 0, 0 };
  Thermometer_SetParameter( THERMOMETER_DATA, THERMOMETER_DATA_LEN, thermoData );
  
  // ֹͣ�ɼ�
  uint8 thermoCfg = THERMOMETER_CONF_STANDBY;
  Thermometer_SetParameter( THERMOMETER_CONF, sizeof(uint8), &thermoCfg );  
  
  // ���ô�������
  Thermometer_SetParameter( THERMOMETER_PERI, sizeof(uint8), &transNumOfADPeriod ); 
  
  // ֹͣ����
  thermoADEnabled = FALSE;
}


static void switchFromStandbyToActive( void )
{  
  curMode = MODE_ACTIVE;
  
  // ��Ӳ��
  Thermo_HardwareOn();
  
  // ��ʼ�㲥
  GAPConfig_EnableAdv(TRUE);    

  // ��ȡ���ֵ����  
  uint8 thermoCfg = Thermo_GetValueType();
  Thermometer_SetParameter( THERMOMETER_CONF, sizeof(uint8), &thermoCfg ); 
  
  // ��ʾ�ϴ����ֵ�󣬿�ʼAD����
  thermoADEnabled = TRUE;
  osal_start_timerEx( CMTechThermometer_TaskID, TH_PERIODIC_EVT, THERMO_LASTMAXTEMP_SHOWTIME);
  
  // ���¼���
  haveSendNum = 0;
  
  DP_Init(CMTechThermometer_TaskID);
}


static void switchFromActiveToStandby( void )
{ 
  curMode = MODE_STANDBY;
  
  // ֹͣAD
  thermoADEnabled = FALSE;
  osal_stop_timerEx( CMTechThermometer_TaskID, TH_PERIODIC_EVT);
  
  // ��ֹ��������
  if ( gapProfileState == GAPROLE_CONNECTED )
  {
    GAPConfig_TerminateConn();
  }
  
  // ֹͣ�㲥
  GAPConfig_EnableAdv(FALSE);   
  
  // ��Ӳ��
  Thermo_HardwareOff();
  
  initIOPin();
}




static void readAndProcessThermoData()
{
  // ��ȡ����
  uint16 value = Thermo_GetValue();  
  
  // �������ݣ������ͣ�����ʾ
  if(value == FAILURE) return;   
  
  // �����������ֵ
  Thermo_UpdateMaxValue(value);  
  
  // ��Һ��������ʾ����
  Thermo_ShowValueOnLCD(1, value);
  
  // ���˴������ڣ��ͷ���������������ֵ
  if(++haveSendNum >= transNumOfADPeriod)
  {
    Thermometer_SetParameter( THERMOMETER_DATA, THERMOMETER_DATA_LEN, (uint8*)&value); 
    haveSendNum = 0;
  }

  // ������¶����ݣ�ȥ������
  if(Thermo_GetValueType() == THERMOMETER_CONF_VALUETYPE_T)
  {
    DP_Process(value);
  }
}

// �÷�������ָ������
static void turnOnTone(uint8 times)
{
  bzTimes = times;
  osal_set_event(CMTechThermometer_TaskID, TH_TONE_ON_EVT);
}


/*********************************************************************
*********************************************************************/
