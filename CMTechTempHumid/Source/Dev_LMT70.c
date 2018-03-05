


#include "hal_adc.h"
#include "Dev_LMT70.h"

static bool active = FALSE;

extern void Lmt70_Init()
{ 
  P0SEL &= ~(1<<1);   //p0_1GPIO
  P0DIR |= (1<<1);    //p0_1Ϊ���
  P0 &= ~(1<<1);      //p0_1Ϊ�͵�ƽ
  
  HalAdcSetReference( HAL_ADC_REF_125V );  //���ò����ڲ��ο���ѹ������CC2541Ϊ1.24V
             
  active = FALSE;
}

extern void Lmt70_Start()
{
  if(!active)
  {
    P0_1 = 1;                   //p0_1Ϊ�ߵ�ƽ   
    active = TRUE;
  } 
}
             
extern void Lmt70_Stop()
{
  if(active)
  {
    P0_1 = 0;                   //p0_1Ϊ�͵�ƽ   
    active = FALSE;
  }   
}
             
extern uint16 Lmt70_ReadTemperature()
{
  Lmt70_Start();
  
  // ADCͨ��5������14λ�ֱ��ʣ�ʵ��ENOBΪ12λ
  uint16 sample = HalAdcRead (HAL_ADC_CHANNEL_5, HAL_ADC_RESOLUTION_14);
    
  return sample;
}