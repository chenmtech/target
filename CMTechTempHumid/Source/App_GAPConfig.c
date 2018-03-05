
#include "bcomdef.h"
#include "gap.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"
#include "peripheral.h"
#include "App_GAPConfig.h"
#include "CMUtil.h"

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
//#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
//#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

// Slave latency to use if automatic parameter update request is enabled
//#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
//#define DEFAULT_DESIRED_CONN_TIMEOUT          1000





// ������㲥��صĲ���
// ����General Discoverableģʽ�������ǳ����㲥
// advInt: �㲥���ʱ�䣬��λms
// ���ڹ㲥ʱ�����õ����⣬���Բ��� http://blog.csdn.net/zzfenglin/article/details/51166830
extern void GAPConfig_SetAdvParam(uint16 advInt, uint16 servUUID)
{
  // ���ù㲥���ʱ��
  advInt = (advInt < 20) ? 20 : ( (advInt > 10240) ? 10240 : advInt  );
  uint16 advInt625 = ((uint32)advInt)*1000L/625L;

  //GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, advInt625 );
  //GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, advInt625 );
  GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, advInt625 );
  GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, advInt625 );  
  
  // ��ͣ�ع㲥
  GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, 0 ); 
  
  
  // ���ù㲥���ݣ����31�ֽڣ�Ϊ�˽�ʡ��������������  
  uint8 advertData[] =
  {
    0x02,   // length of this data
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,  
  };  
  GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );  
  
  //����ɨ����Ӧ����
  // ɨ����Ӧ���ݣ����31�ֽ�
  uint8 scanRspData[] =
  {
    // �豸����
    0x06,   // length of this data
    GAP_ADTYPE_LOCAL_NAME_SHORT,
    'C',
    'M',
    '1',
    '.',
    '0',
  
    // ֪ͨ���������ӻ������ķ���UUID 
    0x11,   // length of this data
    GAP_ADTYPE_128BIT_MORE,      // some of the UUID's, but not all
    CM_UUID( servUUID ),  
  
    // ���书�ʵȼ�
    0x02,   // length of this data
    GAP_ADTYPE_POWER_LEVEL,
    0,       // 0dBm
  };  
  GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );  
 
}

//ʹ�ܻ�ֹͣ�㲥
extern void GAPConfig_EnableAdv(uint8 enable)
{
  GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &enable );     
}

//������������صĲ���
//���Ӽ��ʱ�䣺��Χ7.5ms~4s֮��1.25ms�������������min��max��һ����оƬ��ѡ��һ������max��ֵ
//slave latency: �ӻ���Ǳ����������Χ0 - 499������������Ӽ��������һ�����Ӽ�����ӻ�������Ӧ
//��س�ʱʱ�䣺��Χ100ms-32s�����Ͻ������ٸ��ӻ�6�������Ļ���
//minInt: ��λms
//maxInt: ��λms
//latency: Ǳ������
//timeout: ��س�ʱʱ�䣬��λms
//when: ���ӽ���������뿪ʼ���²�������λs
extern void GAPConfig_SetConnParam(uint16 minInt, uint16 maxInt, uint16 latency, uint16 timeout, uint8 when)
{
  uint8 enable_update_request = TRUE;
  minInt = (uint16)( ((float)minInt)/1.25 );
  maxInt = (uint16)( ((float)maxInt)/1.25 );  
  timeout /= 10;  

  GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
  GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &minInt );
  GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &maxInt );
  GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &latency );
  GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &timeout );  

  //���������ָ�����ӽ����󵽴ӻ��������Ӳ���֮����Ҫ��ʱ��ʱ��
  //���������ͬ����²������ӻ�����ѡ��Ͽ����ӻ�����������в���
  GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, when );    
}

//��ֹ���ӡ��ӻ�û��Ȩ���������ӣ�������������ֹ����
extern bStatus_t GAPConfig_TerminateConn()
{
  return GAPRole_TerminateConnection();
}

//����GGS������ֻ���豸�������Կ�������
//devName: �豸������ֵ
extern void GAPConfig_SetGGSParam(uint8* devName)
{
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, devName );
}

//���ð���ز���
// passkey : �������
extern void GAPConfig_SetBondingParam(uint32 passkey, uint8 pairmode)
{
  uint8 mitm = TRUE;
  uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY; //Ҫ���ֻ������������
  uint8 bonding = TRUE;
  GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
  GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairmode );
  GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
  GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
  GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
}