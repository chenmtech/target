
#ifndef SERVICE_TEMPHUMID_H
#define SERVICE_TEMPHUMID_H

// �������
#define THERMOMETER_DATA    0       //����ֵ
#define THERMOMETER_CONF    1       //�������Ƶ�
#define THERMOMETER_PERI    2       //��������




// �����������16λUUID
#define THERMOMETER_SERV_UUID    0xAA30
#define THERMOMETER_DATA_UUID    0xAA31
#define THERMOMETER_CONF_UUID    0xAA32
#define THERMOMETER_PERI_UUID    0xAA33


// �����bit field
#define THERMOMETER_SERVICE               0x00000001

// ����ֵ���ֽڳ���
#define THERMOMETER_DATA_LEN     2

#define THERMOMETER_MIN_PERIOD        1000    //��С��������Ϊ1000ms

#define THERMOMETER_TIME_UNIT         1000    //��������ʱ�䵥λΪ1000ms


typedef NULL_OK void (*thermometerServiceCB_t)( uint8 paramID );


typedef struct
{
  thermometerServiceCB_t        pfnThermometerServiceCB;  // Called when characteristic value changes
} thermometerServiceCBs_t;


// ���ر�����
extern bStatus_t Thermometer_AddService( uint32 services );

// �Ǽ�Ӧ�ò�ص�
extern bStatus_t Thermometer_RegisterAppCBs( thermometerServiceCBs_t *appCallbacks );

// ��������ֵ
extern bStatus_t Thermometer_SetParameter( uint8 param, uint8 len, void *value );

// ��ȡ����ֵ
extern bStatus_t Thermometer_GetParameter( uint8 param, void *value );

#endif












