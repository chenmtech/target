
#include "bcomdef.h"
#include "gatt.h"
#include "gattservapp.h"
#include "App_GATTConfig.h"
#include "Service_TempHumid.h"


//����Height����
//appCBs: Ӧ�ò�Ϊ�˷����ṩ�Ļص�����
//���������������Ҫ���ã�����մ˺�������һ���º�����
extern void GATTConfig_SetThermoService(thermometerServiceCBs_t* appCBs)
{
  Thermometer_AddService( GATT_ALL_SERVICES );  // ���ط���  

  // �Ǽǻص�
  VOID Thermometer_RegisterAppCBs( appCBs );  
}
