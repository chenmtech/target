

#ifndef APP_DATAPROCESSOR_H
#define APP_DATAPROCESSOR_H

#include "comdef.h"


// ��ʼ��
extern void DP_Init(uint8 TaskID);

// �����¶�����
extern void DP_Process(uint16 data);

// Ԥ���¶Ȳ���
extern uint16 getPrecastTemp();

// �����ȶ���ʾ����
extern void notifyTempStable();






#endif













