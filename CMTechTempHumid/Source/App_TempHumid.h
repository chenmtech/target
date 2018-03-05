/*
 * ���ģ������ʵ�����¼Ƶ�Ӧ�ù���
*/

#ifndef APP_TEMPHUMID_H
#define APP_TEMPHUMID_H

#include "comdef.h"

// �ɼ���������������
#define THERMOMETER_CONF_VALUETYPE_AD    0x01    //������������ΪADֵ
#define THERMOMETER_CONF_VALUETYPE_R     0x02    //������������Ϊ����ֵ
#define THERMOMETER_CONF_VALUETYPE_T     0x03    //������������Ϊ�¶�ֵ


#define T_LOWLIMIT    3390    // �¶�ֵ����
#define T_UPLIMIT     4410    // �¶�ֵ����

#define R_LOWLIMIT    22332    // ����ֵ����
#define R_UPLIMIT     33528    // ����ֵ����

#define AD_LOWLIMIT   0        // AD����
#define AD_UPLIMIT    32768    // AD����


// ��ʼ��
extern void Thermo_Init();

// ��Ӳ��
extern void Thermo_HardwareOff();

// ��Ӳ��
extern void Thermo_HardwareOn();

// ��ȡ����
extern uint16 Thermo_GetValue();

// ���б궨
extern void Thermo_DoCalibration();

// ��ȡ��������
extern uint8 Thermo_GetValueType();

// ������������
extern void Thermo_SetValueType(uint8 type);

// ���µ�ǰ���ֵ
extern uint16 Thermo_UpdateMaxValue(uint16 value);

// ��LCD����ʾһ��ֵ
extern void Thermo_ShowValueOnLCD(uint8 location, uint16 value);

// ��LCD
extern void Thermo_TurnOn_LCD();

// ��LCD
extern void Thermo_TurnOff_LCD();

// ��AD
extern void Thermo_TurnOn_AD();

// ��AD
extern void Thermo_TurnOff_AD();

// ��������
extern void Thermo_ToneOn();

// �ط�����
extern void Thermo_ToneOff();

// ����Ԥ���¶�ֵ
extern void Thermo_SetPreTemp(uint16 temp);

// �����Ƿ���ʾԤ���¶�ֵ
extern void Thermo_SetShowPreTemp(bool isShow);


#endif