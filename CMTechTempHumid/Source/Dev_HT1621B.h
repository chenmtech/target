#ifndef _HT1621B_H_
#define _HT1621B_H_

#include "comdef.h"
#include <iocc2541.h>




// ��ʼ��HT IO�˿�
// ��VDD, CS, WR, DATA����ΪGPIO����͵�ƽ���Ա㽵�͹���
extern void HT1621B_InitToPowerDown();

// ��LCD�ϵ磬ͨ�������ʼ��LCD��������
extern void HT1621B_TurnOnLCD(void);

// ��LCD�����ص��͹���״̬
extern void HT1621B_TurnOffLCD(void);

// �����Ļ
extern void HT1621B_ClearLCD(void);

// ��ָ��Һ��λ��дһ���ֽڶ���
extern void HT1621B_WR_ONEBYTE(uint8 location, uint8 segCode);

// ��ָ��Һ��λ�ÿ�ʼ��дnWord���ȵĶ�������
extern void HT1621B_WR_LCD_MEM(uint8 location, uint8 * Seg_Buf, uint8 nWord);

// ��ָ��λ�ÿ�ʼ����ʾһ��uint16��ʮ��������
extern void HT1621B_ShowUint16Data(uint8 location, uint16 data);

// ��ʾһ���¶ȣ���С���㣬isPreTemp�����ж��Ƿ�Ԥ���¶�
extern void HT1621B_ShowTemperature(uint8 location, uint16 temp, bool isPreTemp);

// ��ʾ"L"
extern void HT1621B_ShowL(uint8 location);

// ��ʾ"H"
extern void HT1621B_ShowH(uint8 location);

// ��������
extern void HT1621B_ToneOn();

// �ط�����
extern void HT1621B_ToneOff();

// 
#endif