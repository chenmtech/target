/**
* ��ģ����Ҫ����������GAP�йصĲ����������㲥���������Ӳ�����GGS�����Ͱ󶨲���
*/
#ifndef APP_GAPCONFIG_H
#define APP_GAPCONFIG_H


//������㲥��صĲ��������Ǵ�����㲥���ݺ�ɨ����Ӧ���ݻ��Ǿ�̬����
//advInt: �㲥���ʱ�䣬��λms����Χ20~10240ms
//servUUID�����豸�����ķ���UUID
extern void GAPConfig_SetAdvParam(uint16 advInt, uint16 servUUID);

//ʹ�ܻ�ֹͣ�㲥
extern void GAPConfig_EnableAdv(uint8 enable);

//������������صĲ���
//���Ӽ��ʱ�䣺��Χ7.5ms~4s֮��1.25ms�������������min��max��һ����оƬ��ѡ��һ������max��ֵ
//slave latency: �ӻ���Ǳ����������Χ0 - 499������������Ӽ��������һ�����Ӽ�����ӻ�������Ӧ
//��س�ʱʱ�䣺��Χ100ms-32s�����Ͻ������ٸ��ӻ�6�������Ļ���
//minInt: ��λms
//maxInt: ��λms
//latency: Ǳ������
//timeout: ��س�ʱʱ�䣬��λms
//when: ���ӽ���������뿪ʼ���²�������λs
extern void GAPConfig_SetConnParam(uint16 minInt, uint16 maxInt, uint16 latency, uint16 timeout, uint8 when);

//��ֹ���ӡ��ӻ�û��Ȩ���������ӣ�������������ֹ����
extern bStatus_t GAPConfig_TerminateConn();

//����GGS����
//devName: �豸������ֵ
extern void GAPConfig_SetGGSParam(uint8* devName);

//���ð���ز���
// passkey : �������
// pairmode : ���ģʽ��
//      GAPBOND_PAIRING_MODE_NO_PAIRING: ���������
//      GAPBOND_PAIRING_MODE_WAIT_FOR_REQ: �ȴ��Է������ȫ��Ҫʱ����
//      GAPBOND_PAIRING_MODE_INITIATE: �����������
extern void GAPConfig_SetBondingParam(uint32 passkey, uint8 pairmode);

#endif





