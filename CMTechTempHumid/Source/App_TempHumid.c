
#include "Dev_ADS1100.h"

#include "bcomdef.h"

#include "osal_snv.h"

#include "Dev_HT1621B.h"

#include "App_TempHumid.h"

// NVID
#define BLE_NVID_CALI_VALUE                     0x80      // �궨ֵ��NV�е�ID
#define BLE_NVID_VALUE_TYPE                     0x81      // �ϴβɼ�����������NV�е�ID��1��ADֵ��2������ֵ��3���¶�ֵ
#define BLE_NVID_MAX_VALUE                      0x82      // �ϴβɼ������ֵ��NV�е�ID


//////////////////����Ϊ����NTCС��ͷ�Ĳ���
//��34���϶�-44���϶ȣ�0.1���϶�Ϊ���
//��35-38���϶ȣ�0.05���϶�Ϊ���
static const uint16 TTable[] = 
{
  3400,3410,3420,3430,3440,3450,3460,3470,3480,3490,      //34���϶�
  3500,3505,3510,3515,3520,3525,3530,3535,3540,3545,      //35.00-35.45���϶�
  3550,3555,3560,3565,3570,3575,3580,3585,3590,3595,      //35.50-35.95���϶�
  3600,3605,3610,3615,3620,3625,3630,3635,3640,3645,      //36.00-36.45���϶�
  3650,3655,3660,3665,3670,3675,3680,3685,3690,3695,      //36.50-36.95���϶�
  3700,3705,3710,3715,3720,3725,3730,3735,3740,3745,      //37.00-37.45���϶�
  3750,3755,3760,3765,3770,3775,3780,3785,3790,3795,      //37.50-37.95���϶�
  3800,3810,3820,3830,3840,3850,3860,3870,3880,3890,      //38���϶�
  3900,3910,3920,3930,3940,3950,3960,3970,3980,3990,      //39���϶�
  4000,4010,4020,4030,4040,4050,4060,4070,4080,4090,      //40���϶�     
  4100,4110,4120,4130,4140,4150,4160,4170,4180,4190,      //41���϶� 
  4200,4210,4220,4230,4240,4250,4260,4270,4280,4290,      //42���϶� 
  4300,4310,4320,4330,4340,4350,4360,4370,4380,4390,      //43���϶�    
  4400  //44���϶�
};




//��Ӧ�¶��µ�NTC����ֵ���ɻ����ṩ�ı��õ�
static const uint16 RTable[] = 
{
  33527,33388,33250,33113,32976,32840,32704,32570,32435,32302,      //34���϶�
  32168,32102,32036,31970,31904,31838,31773,31707,31642,31577,      //35.00-35.45���϶�
  31512,31447,31382,31318,31253,31189,31125,31061,30997,30934,      //35.50-35.95���϶�
  30870,30807,30743,30680,30617,30555,30492,30429,30367,30305,      //36.00-36.45���϶�
  30243,30181,30119,30057,29995,29934,29873,29812,29751,29690,      //36.50-36.95���϶�
  29629,29568,29508,29448,29387,29327,29267,29208,29148,29088,      //37.00-37.45���϶�
  29029,28970,28911,28852,28793,28734,28676,28617,28559,28501,      //37.50-37.95���϶�
  28442,28327,28211,28097,27983,27869,27756,27643,27531,27419,      //38���϶�
  27308,27197,27087,26977,26868,26759,26651,26543,26436,26329,      //39���϶�
  26223,26117,26012,25907,25802,25698,25595,25491,25389,25287,      //40���϶�     
  25185,25084,24983,24882,24782,24683,24584,24485,24387,24289,      //41���϶� 
  24192,24095,23999,23903,23807,23712,23617,23523,23429,23335,      //42���϶� 
  23242,23149,23057,22965,22873,22782,22692,22601,22511,22422,      //43���϶�    
  22333  //44���϶�
};




//��Ӧ�����µ�AD���ֵ����ʵ��õ�
static const uint16 ADTable[] = 
{
  16247,16281,16316,16349,16384,16419,16452,16486,16520,16554,      //34���϶�
  16588,16605,16621,16639,16655,16671,16689,16706,16723,16740,      //35.00-35.45���϶�
  16756,16773,16789,16807,16824,16840,16857,16874,16891,16907,      //35.50-35.95���϶�
  16925,16942,16958,16976,16992,17009,17026,17042,17059,17075,      //36.00-36.45���϶�
  17092,17110,17127,17144,17161,17177,17194,17211,17228,17245,      //36.50-36.95���϶�
  17261,17278,17295,17311,17327,17344,17361,17378,17394,17411,      //37.00-37.45���϶�
  17427,17444,17461,17477,17494,17511,17528,17544,17560,17577,      //37.50-37.95���϶�
  17594,17627,17660,17693,17726,17759,17793,17825,17858,17892,      //38���϶�
  17925,17958,17990,18023,18057,18090,18122,18155,18188,18221,      //39���϶�
  18254,18286,18318,18351,18384,18416,18448,18481,18514,18546,      //40���϶�
  18579,18612,18644,18677,18709,18741,18773,18806,18837,18869,      //41���϶�
  18901,18933,18966,18997,19030,19062,19094,19125,19158,19189,      //42���϶�
  19221,19253,19284,19316,19348,19379,19410,19443,19474,19505,      //43���϶�
  19536       //44���϶�
};




//37�ȵ�ʱ���ADֵ�����ڱ궨
#define ADVALUE37   17261   

// ������ݳ���
static uint8 LEN = sizeof(ADTable)/sizeof(uint16);

// ��ǰ�����ڱ���е�����ֵ
static uint8 idx = 0;

// Ŀǰ�����ĸ߾��Ȳο����趼��33009Ohm
//static uint16 RREF = 33009;   // �߾��Ȳο����裬��λOhm

// Ŀǰ��1���������˵İ壩�ı궨ֵΪ0����������ݱ��Ǹ��ݰ�1�����ģ�
// ��2�ı궨ֵΪ14������2�����ֵ�Ȱ�1Ҫ��14��
static int16  caliValue = 0;    // ����ADS1100��Offset Error�����������Ҫ���б궨������Ǳ궨ֵ

// ��������
static uint8 valueType = THERMOMETER_CONF_VALUETYPE_T;    

// �������͵�����
static uint16 valueLowLimit = T_LOWLIMIT;

// �������͵�����
static uint16 valueUpLimit = T_UPLIMIT;

// �ϴβ��������ֵ
static uint16 lastMaxValue = 0;

// ���β��������ֵ
static uint16 maxValue = 0;

// ���β����ĵ�ǰֵ
static uint16 curValue = 32768;   // 32768��һ�������ܲɼ�����ֵ

// Ԥ����¶�
static uint16 preTemp = 0;

// �Ƿ���ʾԤ���¶�
static bool isShowPreTemp = FALSE;



// ��AD���ֵ���������ֵ
static uint16 calcRFromADValue(uint16 ADValue);

// ��AD���ֵ�������¶�ֵ
static uint16 calcTFromADValue(uint16 ADValue);

// ���汾�β��������ֵ��NV
static void saveMaxValueToNV();

// ��NV��ȡ�ϴβ��������ֵ
static void readLastMaxValueFromNV();

// ��ȡADֵ
static uint16 getADValue();

// ��ȡ�¶�ֵ
static uint16 getTemperature();

// ��ȡ����ֵ
static uint16 getResistor();


///////////////////////////////////////////////////////////////////////////////
// �ⲿ����
//////////////////////////////////////////////////////////////////////////////

// ��ʼ��
extern void Thermo_Init()
{
  // ��ʼ��HT1621���͹���״̬
  HT1621B_InitToPowerDown();  
  
  // ��ʼ��AD
  ADS1100_Init();    
  
  // ��NV��ȡ�궨ֵ��������
  uint8 rtn = osal_snv_read(BLE_NVID_CALI_VALUE, sizeof(int16), (uint8*)&caliValue);
  if(rtn != SUCCESS)
    caliValue = 0;    
}

// ��Ӳ������LCD����AD
extern void Thermo_HardwareOff()
{  
  // �������ֵ��NV
  saveMaxValueToNV();  
  
  // ����LCD
  HT1621B_ClearLCD();    
  // ��LCD
  HT1621B_TurnOffLCD();    

  // ��AD
  ADS1100_TurnOff(); 
}


// ��Ӳ������LCD����AD, ��ʾ�ϴ����ֵ
extern void Thermo_HardwareOn()
{
  // ��NV��ȡ�ϴ�����¶�ֵ��������
  readLastMaxValueFromNV();
  
  maxValue = 0;  
  
  curValue = 32768;
  
  preTemp = 0;
  isShowPreTemp = FALSE;

  // ��LCD
  HT1621B_TurnOnLCD();  
  // ����LCD
  HT1621B_ClearLCD();   
  
  // ��AD
  ADS1100_TurnOn();  
  
  //��ʾ�ϴ����ֵ
  Thermo_ShowValueOnLCD(1, lastMaxValue);  
}

// ��ȡ��������
extern uint8 Thermo_GetValueType()
{
  return valueType;
}

// �������ݵ�����
extern void Thermo_SetValueType(uint8 type)
{
  if(valueType == type) return;
  
  switch(type)
  {
  case THERMOMETER_CONF_VALUETYPE_AD:
    valueLowLimit = AD_LOWLIMIT;
    valueUpLimit = AD_UPLIMIT;
    break;
  case THERMOMETER_CONF_VALUETYPE_R:
    valueLowLimit = R_LOWLIMIT;
    valueUpLimit = R_UPLIMIT;
    break;
  case THERMOMETER_CONF_VALUETYPE_T:
    valueLowLimit = T_LOWLIMIT;
    valueUpLimit = T_UPLIMIT;
    break;
  default:
    return;
  }   
  valueType = type;
  maxValue = 0;  
  curValue = 32768;
  HT1621B_ClearLCD();
}

// ��ȡ����
extern uint16 Thermo_GetValue()
{
  switch(valueType)
  {
  case THERMOMETER_CONF_VALUETYPE_AD:
    return getADValue();
  case THERMOMETER_CONF_VALUETYPE_R:
    return getResistor();
  case THERMOMETER_CONF_VALUETYPE_T:
    return getTemperature();
  }
  return FAILURE;
}


// ���б궨
extern void Thermo_DoCalibration()
{
  uint16 sum = 0;
  uint16 data = 0;
  
  for(int i = 0; i < 3; i++)
  {
    ADS1100_GetADValue(&data);
    sum += data;
  }
  
  // ȡ37��ʱ������AD���ƽ��ֵ�����׼��37�����ֵ���бȽϣ��õ��궨���ֵ
  caliValue = (int16)((double)sum/3 - ADVALUE37 + 0.5);  // ��������
  
  // ���궨ֵ������NV��
  osal_snv_write(BLE_NVID_CALI_VALUE, sizeof(int16), (uint8*)&caliValue);
}


// �������ֵ
extern uint16 Thermo_UpdateMaxValue(uint16 value)
{
  if(maxValue < value) maxValue = value;
  return maxValue;
}


// ��LCD����ʾһ��ֵ
extern void Thermo_ShowValueOnLCD(uint8 location, uint16 value)
{
  // ���Ҫ��ʾԤ���¶�
  if( isShowPreTemp && (valueType == THERMOMETER_CONF_VALUETYPE_T) )
  {
    HT1621B_ShowTemperature(location, preTemp, TRUE);
    curValue = 32768;
    return;
  }
  
  // ���ֵ�뵱ǰֵ��ͬ���Ͳ���ʾ�ˡ�
  if( curValue == value ) return;
  
  curValue = value;
  
  if(value <= valueLowLimit)    //��ʾ"L"
  {
    HT1621B_ShowL(location);
    return;
  }
  else if(value >= valueUpLimit)   //��ʾ"H"
  {
    HT1621B_ShowH(location);
    return;
  }    
  else
  {
    // ��ʾ�¶�����
    if(valueType == THERMOMETER_CONF_VALUETYPE_T)
    {
      HT1621B_ShowTemperature(location, value, FALSE);
    }
    else  // ��ʾ������������
      HT1621B_ShowUint16Data(location, value);  
  }
}

// ��LCD
extern void Thermo_TurnOn_LCD()
{
  HT1621B_TurnOnLCD();
  HT1621B_ClearLCD();
}

// ��LCD
extern void Thermo_TurnOff_LCD()
{
  HT1621B_TurnOffLCD();
}

// ��AD
extern void Thermo_TurnOn_AD()
{
  ADS1100_TurnOn();
}

// ��AD
extern void Thermo_TurnOff_AD()
{
  ADS1100_TurnOff();
}

// ��������
extern void Thermo_ToneOn()
{
  HT1621B_ToneOn();
}

// �ط�����
extern void Thermo_ToneOff()
{
  HT1621B_ToneOff(); 
}

// ����Ԥ���¶�ֵ
extern void Thermo_SetPreTemp(uint16 temp)
{
  preTemp = temp;
}

// �����Ƿ���ʾԤ���¶�ֵ
extern void Thermo_SetShowPreTemp(bool isShow)
{
  isShowPreTemp = isShow;  
}

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
// ��̬����
/////////////////////////////////////////////////////////////////////////////

// ��ȡADֵ
static uint16 getADValue()
{
  // ��ȡADֵ
  uint16 ADValue = 0;
  if(ADS1100_GetADValue(&ADValue) == FAILURE) return FAILURE;
  
  // ��ȥ�궨ֵ���
  return (uint16)((double)ADValue - caliValue);  
}

// ��ȡ�¶�ֵ
static uint16 getTemperature()
{
  uint16 ADValue = getADValue();
  if(ADValue == FAILURE) return FAILURE;
  
  return calcTFromADValue(ADValue);
}

// ��ȡ����ֵ
static uint16 getResistor()
{
  uint16 ADValue = getADValue();
  if(ADValue == FAILURE) return FAILURE;
  
  return calcRFromADValue(ADValue);
}



// ��NV��ȡ�ϴβ��������ֵ
static void readLastMaxValueFromNV()
{
  // ��NV��ȡ�ϴ���������
  uint8 rtn = osal_snv_read(BLE_NVID_VALUE_TYPE, sizeof(uint8), (uint8*)&valueType);
  if(rtn != SUCCESS)
    valueType = THERMOMETER_CONF_VALUETYPE_T;    
  
  // �����������͵�������
  switch(valueType)
  {
  case THERMOMETER_CONF_VALUETYPE_AD:
    valueLowLimit = AD_LOWLIMIT;
    valueUpLimit = AD_UPLIMIT;
    break;
  case THERMOMETER_CONF_VALUETYPE_R:
    valueLowLimit = R_LOWLIMIT;
    valueUpLimit = R_UPLIMIT;
    break;
  case THERMOMETER_CONF_VALUETYPE_T:
    valueLowLimit = T_LOWLIMIT;
    valueUpLimit = T_UPLIMIT;
    break;
  }
  
  // ��NV��ȡ�ϴ����ֵ��������
  rtn = osal_snv_read(BLE_NVID_MAX_VALUE, sizeof(uint16), (uint8*)&lastMaxValue);
  if(rtn != SUCCESS)
    lastMaxValue = 0;    
}


// �������ֵ���������͵�NV
static void saveMaxValueToNV()
{
  osal_snv_write(BLE_NVID_VALUE_TYPE, sizeof(uint8), (uint8*)&valueType);
  osal_snv_write(BLE_NVID_MAX_VALUE, sizeof(uint16), (uint8*)&maxValue);
}



// ��AD���ֵ�������ֵ
static uint16 calcRFromADValue(uint16 ADValue)
{
  if(ADValue < ADTable[0])
  {
    idx = 0;
    return R_UPLIMIT;
  } 
  
  if(ADValue > ADTable[LEN-1])
  {
    idx = LEN-1;
    return R_LOWLIMIT;    
  }
  
  while(ADTable[idx] > ADValue)
  {
    idx--;      
  }
  
  while(ADTable[idx+1] < ADValue)
  {
    idx++;
  }

  uint16 result = (uint16)( RTable[idx] - ((double)(RTable[idx]-RTable[idx+1]))/(ADTable[idx+1]-ADTable[idx])*(ADValue-ADTable[idx]) );
  
  return result;  
}

// ��AD���ֵ�������¶�ֵ
static uint16 calcTFromADValue(uint16 ADValue)
{
  if(ADValue < ADTable[0])
  {
    idx = 0;
    return T_LOWLIMIT;
  } 
  
  if(ADValue > ADTable[LEN-1])
  {
    idx = LEN-1;
    return T_UPLIMIT;    
  }
  
  while(ADTable[idx] > ADValue)
  {
    idx--;      
  }
  
  while(ADTable[idx+1] < ADValue)
  {
    idx++;
  }
  
  uint16 result = (uint16)( ((double)(TTable[idx+1]-TTable[idx]))/(ADTable[idx+1]-ADTable[idx])*(ADValue-ADTable[idx])+TTable[idx] );
  
  return result;  
}