

#ifndef DEV_ADS1100_H
#define DEV_ADS1100_H

#include "comdef.h"
#include <iocc2541.h>


// ��ADS1100��Դ
extern void ADS1100_TurnOn();

// �ر�ADS1100��Դ
extern void ADS1100_TurnOff();

// ��ʼ��
extern void ADS1100_Init();

// ��ȡADת��ֵ
// ����SUCCESS����FAILURE
extern uint8 ADS1100_GetADValue(uint16 * pData);





#endif