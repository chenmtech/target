


#include "hal_adc.h"
#include "Dev_BlackHead.h"

static bool active = FALSE;

extern void BlackHead_Init()
{ 
  //P0SEL &= ~(1<<1);   //p0_1GPIO
  //P0DIR |= (1<<1);    //p0_1Ϊ���
  //P0 &= ~(1<<1);      //p0_1Ϊ�͵�ƽ
  
  //HalAdcSetReference( HAL_ADC_REF_AVDD );  //���ò��õ�Դ��ѹ��Ϊ�ο���ѹ
  HalAdcSetReference( HAL_ADC_REF_DIFF );
             
  active = FALSE;
}

extern void BlackHead_Start()
{
  if(!active)
  {
    //P0_1 = 1;                   //p0_1Ϊ�ߵ�ƽ   
    active = TRUE;
  } 
}
             
extern void BlackHead_Stop()
{
  if(active)
  {
    //P0_1 = 0;                   //p0_1Ϊ�͵�ƽ   
    active = FALSE;
  }   
}
             
extern uint16 BlackHead_ReadTemperature()
{
  BlackHead_Start();
  
  uint16 sum = 0;
  for(int i = 0; i < 16; i++)
  {
    // ADCͨ��5������14λ�ֱ��ʣ�ʵ��ENOBΪ12λ
    sum += HalAdcRead (HAL_ADC_CHN_A4A5, HAL_ADC_RESOLUTION_12);
  }
  //sum /= 8;
  sum /= 16;
  
  sum = (sum>>3);
    
  return sum;
}