
#include "Dev_ADS1100.h"
#include "hal_i2c.h"

/* Ative delay: 125 cycles ~1 msec */
#define ST_HAL_DELAY(n) st( { volatile uint32 i; for (i=0; i<(n); i++) { }; } )

#define ADS1100_I2C_ADDR 0x48   //AD0��7λ��ַ,0b1001000

#define ADS1100_CONFIG  0x1C    //��ʼ���ã�0b0001 1100������single conversionģʽ��DR=8SPS, PGA=1

#define ADS1100_START_CMD  0x9C  //����������0b1001 1100

// ��Դ�ܽ�����ΪP1.1
#define ADS1100_VDD_LOW  P1 &= ~(1<<1)
#define ADS1100_VDD_HIGH P1 |= (1<<1)



static uint8 buf[3] = {0};

static bool isADOn = false;


////////////////////////////////////////////////////////////////////////////


// ���üĴ���Ϊ��ʼ״̬
static void config();

// ��������ת��
static void convert();




// ��ʼ��
extern void ADS1100_Init()
{
  ADS1100_TurnOn();
  
  HalI2CInit(ADS1100_I2C_ADDR, i2cClock_123KHZ);
  config();
  convert();
  
  ADS1100_TurnOff();
}


// ��ADS1100��Դ
extern void ADS1100_TurnOn()
{
  if(!isADOn)
  {
    // P1.0 ��Դ�ܽ�����
    P1SEL &= ~(1<<1);   // ����ΪGPIO
    ADS1100_VDD_HIGH;   // ����Ϊ�ߵ�ƽ
    P1DIR |= (1<<1);    // ����Ϊ���  
    
    ST_HAL_DELAY(3000);   // ��ʱ�ȴ���Դ�ȶ�������ʱ���Ī�����������
    
    isADOn = true;
  }
}

// �ر�ADS1100��Դ
extern void ADS1100_TurnOff()
{
  if(isADOn)
  {
    // һ��Ҫ��I2C�Ľӿ���ΪGPIO����ʡ��
    HalI2CWrapperEnable();
  
  // P1.0 ��Դ�ܽ�����
    P1SEL &= ~(1<<1);   // ����ΪGPIO
    ADS1100_VDD_LOW;    // ����Ϊ�͵�ƽ
    P1DIR |= (1<<1);    // ����Ϊ���  
    
    ST_HAL_DELAY(3000);   // ��ʱ���ȴ���Դ�ȶ�
  
    isADOn = false;  
  }
}

// ��ȡADת��ֵ
// ����SUCCESS����FAILURE
extern uint8 ADS1100_GetADValue(uint16 * pData)
{
  // ���û�д�AD����򿪣����Ǳ��ζ�ȡΪ�쳣
  if(!isADOn)
  {
    ADS1100_TurnOn();
    HalI2CInit(ADS1100_I2C_ADDR, i2cClock_123KHZ);
    HalI2CRead(3, buf);     //�Ѵ���Ķ���������
    convert();
    *pData = 0;
    return FAILURE;   // ����AD����
  }
  
  HalI2CInit(ADS1100_I2C_ADDR, i2cClock_123KHZ);  
  // ֻ�ܶ���һ�ε�ֵ
  HalI2CRead(3, buf);  
  // �������β���
  convert();
  
  *pData = (((uint16)buf[0]) << 8) + ((uint16)buf[1]);
  
  return SUCCESS;
}



static void config()
{
  uint8 data = ADS1100_CONFIG;
  HalI2CWrite(1, &data);
}

static void convert()
{
  uint8 data = ADS1100_START_CMD;
  HalI2CWrite(1, &data);  
}
