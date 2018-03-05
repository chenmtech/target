
#include "Dev_HT1621B.h"

//����0��9�ıʶ���
//����С����{0,1,2,3,4,5,6,7,8,9}
static uint8 Digital[10] = {0xbe,0X06,0X7c,0X5e,0Xc6,0Xda,0Xfa,0X0e,0Xfe,0Xde};
//��С����{0.,1.,2.,3.,4.,5.,6.,7.,8.,9.}
static uint8 Digital_Dot[10] = {0Xbf,0X07,0X7d,0X5f,0Xc7,0Xdb,0Xfb,0X0f,0Xff,0Xdf};

#define LCODE 0xb0      //"L"�Ķ���
#define HCODE 0xe6      //"H"�Ķ���

// ��Դ�ܽ�����ΪP1.0
#define HT1621_VDD_LOW  P1 &= ~(1<<0)
#define HT1621_VDD_HIGH P1 |= (1<<0)

// CS����ΪP0_4
#define HT1621_CS_LOW   P0 &= ~(1 << 4)
#define HT1621_CS_HIGH  P0 |= (1 << 4)
// WR����ΪP0_5
#define HT1621_WR_LOW   P0 &= ~(1 << 5)
#define HT1621_WR_HIGH  P0 |= (1 << 5)
// DAT����ΪP0_6
#define HT1621_DATA_LOW   P0 &= ~(1 << 6)
#define HT1621_DATA_HIGH  P0 |= (1 << 6)


//commond ID
#define	RD_CMD_ID		0XC0
#define WR_CMD_ID		0XA0
#define	CMD_MODE_ID		0X80
//commond type
#define	SYS_DIS_CMD		0X00
#define	SYS_EN_CMD		0X01
#define	LCD_OFF_CMD		0X02
#define	LCD_ON_CMD		0X03
#define	TMR_DIS_CMD		0X04
#define	WDT_DIS_CMD		0X05
#define	TMR_EN_CMD		0X06
#define	WDT_EN_CMD		0X07
#define	TONE_OFF_CMD	        0X08
#define	TONE_ON_CMD		0X09
#define	CLR_TMR_CMD		0X0C
#define	CLR_WDT_CMD		0X0E
#define	XT_32K_CMD		0X14
#define	RC_256K_CMD		0X18
#define	EXT_256K_CMD	0X1C
#define	BIAS_Half_CMD	0X28	//Attention:4MUX  LCD��1/2ƫѹ��4��������
#define	BIAS_TRI_CMD	0X29	//Attention:4MUX  LCD��1/3ƫѹ��4��������
#define	TONE_4K_CMD		0X40
#define	TONE_2K_CMD		0X60
#define	IRQ_DIS_CMD		0X80
#define	IRQ_EN_CMD		0X88
#define	NORMOAL_CMD		0XE3


/* Ative delay: 125 cycles ~1 msec�������̫׼ */
#define ST_HAL_DELAY(n) st( { volatile uint32 i; for (i=0; i<(n); i++) { }; } )

// LCD�Ƿ��
static bool isLCDOn = false;

// дָ�����ȵ�bitλ
static void HT1621B_WR_nbit_HL(uint8 dat, uint8 nbit);


/**************************************************************************************************
��������:static void WR_HT1621B_nbit_HL(uint8 dat, uint8 nbit)
��������: ��HT1621Bд��ָ���ֽڵ�ǰ��λ��MSB.....LSB, ��λ��ǰ, ��λ�ں�
��ڲ���: ��
���ز���: ��
**************************************************************************************************/
static void HT1621B_WR_nbit_HL(uint8 dat, uint8 nbit)
{
  uint8 i;
  for(i=0; i<nbit; i++)
  {
    HT1621_WR_LOW;
    
    if((dat & 0x80) == 0)
      HT1621_DATA_LOW;
    else
      HT1621_DATA_HIGH;

    HT1621_WR_HIGH;
    
    dat<<=1;		
  }  	
}




///////////////////////////////////////////// �ⲿ����

// ��ʼ��HT IO�˿�
// ��VDD, CS, WR, DATA����ΪGPIO����͵�ƽ���Ա㽵�͹���
extern void HT1621B_InitToPowerDown()
{
  // P1.0 ��Դ�ܽ�����
  P1SEL &= ~(1<<0);   // ����ΪGPIO
  HT1621_VDD_LOW;    // ��ʼΪ�͵�ƽ
  P1DIR |= (1<<0);    // ����Ϊ���
  
  //P0.4 CS�ܽ�����
  P0SEL &= ~(1<<4); //����ΪGPIO
  HT1621_CS_LOW;   //��ʼΪ�͵�ƽ
  P0DIR |= (1<<4);  //����Ϊ���  
  
  //P0.5 WR�ܽ�����
  P0SEL &= ~(1<<5); //����ΪGPIO
  HT1621_WR_LOW;   //��ʼΪ�͵�ƽ
  P0DIR |= (1<<5);  //����Ϊ���  
  
  //P0.6 DATA�ܽ�����
  P0SEL &= ~(1<<6);   //����ΪGPIO
  HT1621_DATA_LOW;   //��ʼΪ�͵�ƽ
  P0DIR |= (1<<6);    //����Ϊ���    
  
  isLCDOn = false;
}


/**************************************************************************************************
��������: void HT1621B_Init_LCD (void)
��������: ��Һ���ϵ�, ����Ƶ��256Hz,ϵͳ��, LCD��, 1/3ƫѹ, 4MUX
��ڲ���: ��
���ز���: ��
**************************************************************************************************/
extern void HT1621B_TurnOnLCD (void)
{ 
  if(isLCDOn) return;
  
  // �ϵ�
  HT1621_VDD_HIGH;
  ST_HAL_DELAY(125);  // ��ʱ  
  
  // ��ҪCS��һ���ߵ�ƽ�������ͨ�ų�ʼ��
  HT1621_CS_HIGH;
  ST_HAL_DELAY(250);  // ��ʱ
  
  HT1621_CS_LOW;
  //
  HT1621B_WR_nbit_HL(CMD_MODE_ID, 3);	// д������ģʽ100
  HT1621B_WR_nbit_HL(SYS_EN_CMD, 9);   	//
  HT1621B_WR_nbit_HL(LCD_ON_CMD, 9);   	//
  HT1621B_WR_nbit_HL(RC_256K_CMD, 9);   //
  HT1621B_WR_nbit_HL(BIAS_TRI_CMD, 9); 	//
  HT1621B_WR_nbit_HL(TONE_4K_CMD, 9); 	//
  //
  HT1621_CS_HIGH;
  
  ST_HAL_DELAY(125);  // ��ʱ  
  isLCDOn = true;
}



// ��LCD�����ص��͹���״̬
extern void HT1621B_TurnOffLCD(void)
{
  if(isLCDOn)
  {
    HT1621_CS_LOW;
    
    HT1621B_WR_nbit_HL(CMD_MODE_ID, 3);
    HT1621B_WR_nbit_HL(LCD_OFF_CMD, 9); 
    HT1621B_WR_nbit_HL(SYS_DIS_CMD, 9);
    
    HT1621_CS_HIGH;  
    
    ST_HAL_DELAY(125);  // ��ʱ
    
    HT1621B_InitToPowerDown();   // �ص��͹���״̬
    
    isLCDOn = false;
  }
}

/**************************************************************************************************
��������:void LCD_CLR(void)
��������: ������ʾ��
��ڲ���: ��
���ز���: ��
**************************************************************************************************/
extern void HT1621B_ClearLCD(void)
{
  uint8 i;
  HT1621_CS_LOW;
  HT1621B_WR_nbit_HL(WR_CMD_ID,3);	//
  HT1621B_WR_nbit_HL(0x00,6);		//6λ��ʼ��ַ, �˴�Ϊ"000000"
  //��������д�뷽ʽд��16�ֽ�0x00
  for(i=0;i<16;i++)
  {
    HT1621B_WR_nbit_HL(0x00,8);
  }
  HT1621_CS_HIGH;
}


// ��ָ��Һ��λ��дһ���ֽڶ���
extern void HT1621B_WR_ONEBYTE(uint8 location, uint8 segCode)
{
  HT1621_CS_LOW;
  //
  HT1621B_WR_nbit_HL(WR_CMD_ID, 3);			// д����
  HT1621B_WR_nbit_HL(location<<2, 6);	 		//��HT1621Bֻ��Ҫ6bit�ĵ�ַ�����Ե�ַҪ����2λ
  HT1621B_WR_nbit_HL(segCode, 8);
  //
  HT1621_CS_HIGH;	  
}


/**************************************************************************************************
��������: extern void WR_LCD_MEM(uint8 location, uint8 * Seg_Buf, uint8 nWord)
��������: дH1621B����
��ڲ���: ��
���ز���: 
**************************************************************************************************/
extern void HT1621B_WR_LCD_MEM(uint8 location, uint8 * Seg_Buf, uint8 nWord)
{
  int i;
  HT1621_CS_LOW;
  //
  HT1621B_WR_nbit_HL(WR_CMD_ID, 3);			// д����
  HT1621B_WR_nbit_HL(location<<2, 6);	 		//��HT1621Bֻ��Ҫ6bit�ĵ�ַ�����Ե�ַҪ����2λ
  for(i=0; i<nWord; i++)
  {
    HT1621B_WR_nbit_HL(*Seg_Buf++, 8);		//   
  }
  //
  HT1621_CS_HIGH;	
}


// ��ʾһ��uint16��ʮ��������
extern void HT1621B_ShowUint16Data(uint8 location, uint16 data)
{
  if(!isLCDOn) return;
  
  uint16 UNIT[5] = {10000, 1000, 100, 10, 1};
  uint8 code[5] = {0};
  uint8 num = 0;
  for(uint8 i = 0; i < 5; i++)
  {
    num = data / UNIT[i];
    data = data % UNIT[i];
    code[i] = Digital[num];
  }
  HT1621B_WR_LCD_MEM(location, code, 5);
}

// ��ʾһ���¶ȣ���С����
extern void HT1621B_ShowTemperature(uint8 location, uint16 temp, bool isPreTemp)
{
  if(!isLCDOn) return;
  
  uint16 UNIT[4] = {1000, 100, 10, 1};
  uint8 code[5] = {0};
  uint8 num = 0;
  for(uint8 i = 0; i < 4; i++)
  {
    num = temp / UNIT[i];
    temp = temp % UNIT[i];
    if(i != 1)
      code[i] = Digital[num];
    else  // ��λ����Ҫ��ʾС����
      code[i] = Digital_Dot[num];
  }
  // ��ʾ���϶ȷ���
  if(isPreTemp)
    code[4] = 0x30;   //Ԥ���¶�
  else
    code[4] = 0x20;   //ʵ���¶�
  HT1621B_WR_LCD_MEM(location, code, 5);
}

// ��ʾ"L"
extern void HT1621B_ShowL(uint8 location)
{
  if(!isLCDOn) return;
  
  HT1621B_ClearLCD();
  HT1621B_WR_ONEBYTE(location, LCODE);
}

// ��ʾ"H"
extern void HT1621B_ShowH(uint8 location)
{
  if(!isLCDOn) return;  
  
  HT1621B_ClearLCD();
  HT1621B_WR_ONEBYTE(location, HCODE);
}

// ��������
extern void HT1621B_ToneOn()
{
  if(!isLCDOn) return;  

  HT1621_CS_LOW;
  //
  HT1621B_WR_nbit_HL(CMD_MODE_ID, 3);	// д������ģʽ100
  HT1621B_WR_nbit_HL(TONE_ON_CMD, 9);   //
  //
  HT1621_CS_HIGH;  
}

// �ط�����
extern void HT1621B_ToneOff()
{
  if(!isLCDOn) return;  

  HT1621_CS_LOW;
  //
  HT1621B_WR_nbit_HL(CMD_MODE_ID, 3);	// д������ģʽ100
  HT1621B_WR_nbit_HL(TONE_OFF_CMD, 9);   //
  //
  HT1621_CS_HIGH;  
}