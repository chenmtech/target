

#include "App_DataProcessor.h"
#include "App_TempHumid.h"
#include "CMTechTempHumid.h"
#include "OSAL.h"

#include "Dev_HT1621B.h"



// ������ʼ��Ч�������¶�ֵ���¶�ֵֻ�д��ڵ��ڴ��¶ȣ����㿪ʼ��Ч����
#define START_TEMP   3400

// �����ȶ����ж���ֵ��һ����֮����¶ȱ仯��ֵ
#define DELTA_TEMP    3    // 0.03���϶�


// ���������ȣ�60������
#define NBUF    60   // ����һ��������

// �ӿ�������ʼ��Ч�¶Ȳ���֮���ʱ����
#define NBEG    60    // ������һ���ӣ���û�н�����Ч�������ͻᴥ���ػ�

// �ӿ�ʼ��Ч����������Ԥ���¶ȣ���������ݳ��ȣ�Ӧ��С��NBUF
#define NPRE    51    // ��ʼ������50�뿪ʼԤ��

// �ӿ�ʼ��Ч�������ж��¶��ȶ����������Сʱ�䣬Ӧ�ô���NBUF
#define NSTA    300   // ����5���Ӳſ�ʼ�ж��Ƿ��ȶ�

// �ӿ�ʼ��Ч�������ػ��ļ���ʱ��
#define NMAX    1800    // ���Сʱһ����ػ�

// �Ӳ����ȶ��󵽹ػ���ʱ����
#define NAFT    180     // �ȶ���3���ӹػ������Կ���ÿ������ʾһ��

/*
// ����Ԥ��5���ӵ�����ģ�Ͳ���
#define A30      4.7330          // 30-20���¶ȱ仯����
#define A40     -12.1624         // 40-30���¶ȱ仯����
#define A50      14.5035         // 50-40���¶ȱ仯����
*/

// ����Ԥ��10���ӵ�����ģ�Ͳ���
#define A30      9.4545          // 30-20���¶ȱ仯����
#define A40     -23.6205         // 40-30���¶ȱ仯����
#define A50      23.0105         // 50-40���¶ȱ仯����


// ���ݻ�����
static uint16 buf[NBUF] = {0};

// ��ǰ���ݱ���Ļ�����λ������
static uint8 idx = 0;

// ����Ӧ�ò������ID���Ա���Է���֪ͨ�¼�
static uint8 taskID;

// ��ʼ��Ч�������¼��������
static uint16 totalNum = 0;

// �����ȶ����¼������
static uint16 stableNum = 0;

// ���¼��Ƿ��ʼ��������ʼ�¶�ֵ����С��34��
static bool isInit = FALSE;

// �Ƿ�ʼ��Ч���£��ж������ǣ��¶�ֵ >= START_TEMP��
static bool isStart = FALSE;

// �����Ƿ��Ѿ��ȶ����ж������ǣ�һ�������¶ȱ仯ֵ <= DELTA_TEMP��
static bool isStable = FALSE;

// 1���ӵ��¶ȱ仯ֵ
static int16 delta = 0;


// �����¼�����
static void trigEvent(uint16 event);




// ��ʼ��
extern void DP_Init(uint8 TaskID)
{
  taskID = TaskID;
  
  for(int i = 0; i < NBUF; i++) buf[i] = 0;
  
  idx = 0;
  
  totalNum = 0;
  
  stableNum = 0;
  
  isInit = FALSE;
  
  isStart = FALSE;
  
  isStable = FALSE;
}

// �����¶�����
extern void DP_Process(uint16 data)
{
  // �����û�г�ʼ��
  if( !isInit )
  {
    // ���Գ�ʼ����
    if(data < START_TEMP)
      isInit = TRUE;
    
    return;
  }
  
  // �����û��Ч��ʼ
  if(!isStart)
  {
    // ��Ч��ʼ��
    if( isStart = (data >= START_TEMP) )
    {
      // �����һ����Ч��������
      buf[0] = data;
      idx = 1;
      // ��ʼ����
      totalNum = 1;
      return;
    }
    
    // ����ָ��ʱ�仹û��ʼ��Ч�������Բ���Ҫ�ػ�ʡ��
    if(++totalNum == NBEG)
    {
      // ֪ͨת��������״̬
      trigEvent(TH_SWITCH_MODE_EVT);
      return;
    }
    
    // ����ָ��ʱ�仹û��ʼ��Ч��������ʲôҲ�������ȴ�
    // �����ܳ���ָ��ʱ��
    return;
  }
  
  // �������Ѿ���ʼ��Ч������Ĳ���
  
  // �Ȱ����ݴ���
  buf[idx++] = data;
  if(idx >= NBUF) idx = 0;
  totalNum++;
  
  // ����Ԥ���ʱ��
  if(totalNum == NPRE)
  {
    // ����Ԥ���¼�
    trigEvent(TH_DP_PRECAST_EVT);
    return;    
  }
  
  // �ڵ����ȶ��ж�ʱ��֮ǰ��ֱ�ӷ���
  if(totalNum < NSTA)
  {
    return;
  }

  // �����Ǵﵽ�ȶ��ж�ʱ��
  
  // �Ѿ��ﵽ�����ʱ�䣬�ػ�
  if(totalNum > NMAX)
  {
    trigEvent(TH_SWITCH_MODE_EVT);
    return;
  }
  
  // ��û���ȶ�
  if(!isStable)
  {
    //��ǰ�¶���һ����ǰ���¶�֮��
    delta = data - buf[idx];
    isStable = ( (delta >= 0) && (delta <= DELTA_TEMP)  );
    if(isStable)  // ��һ���ȶ�
    {
      stableNum = 1;
      // ���������ȶ��¼�
      trigEvent(TH_DP_STABLE_EVT);
      return;
    }
  }
  else  // �����Ѿ��ȶ�
  {
    // �Ѿ��ȶ���ָ��ʱ���ˣ�Ҫ�ػ�
    if(++stableNum == NAFT)
    {
      trigEvent(TH_SWITCH_MODE_EVT);
    }    
  } 
}


// Ԥ���¶Ȳ���
extern uint16 getPrecastTemp()
{
  // ������ģ��Ԥ���¶�ֵ
  return A30*(buf[30]-buf[20])+A40*(buf[40]-buf[30])+A50*(buf[50]-buf[40])+buf[50];
}


// �����ȶ���ʾ����
extern void notifyTempStable()
{
  // ����������
  //osal_set_event( taskID, TH_TONE_ON_EVT);
  //osal_start_timerEx(taskID, TH_TONE_ON_EVT, 1000 );
  //osal_start_timerEx(taskID, TH_TONE_ON_EVT, 2000 );
  
  return;
}


// �����¼�����
static void trigEvent(uint16 event)
{
  osal_set_event( taskID, event);
}
