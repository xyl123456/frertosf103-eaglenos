#ifndef __DETECTBOARD_H__
#define __DETECTBOARD_H__

#include <stdint.h>
#include "AD7124.h"
#include "platform_drivers.h"

/*DAC通道值*/
#define  DAC_0_value   0x8a3c    // 0.1V+1.25V
#define  DAC_1_value   0x947a    // 0.2V+1.25V
#define  DAC_2_value   0x9eb7    // 0.3V+1.25V
#define  DAC_3_value   0xa8f5    // 0.4V+1.25V
#define  DAC_4_value   0xb332    // 0.5V+1.25V 
#define  DAC_5_value   0x7fff    // 1.25V

#define  Apoints  5
#define  Bpoints  5
#define  Cpoints  5

/*检测卡数据结构*/
typedef union
{
  struct Test_Card_One
    {
      int iMg[Apoints];
      int iCa[Apoints];
      int K[Apoints];
      int Na[Apoints];
      int Cl[Apoints]; 
     }TestCardOne;   
     uint8_t buffer[sizeof(struct Test_Card_One)];
 }TestCard_One;  //A卡

typedef union
{
  struct Test_Card_Two
    {
      int PH[Bpoints];
      int CO2_1[Bpoints];
      int CO2_2[Bpoints];
      int glu[Bpoints];
      int pO2[Bpoints]; 
      int Lac[Bpoints];
     }TestCardTwo;    
     uint8_t buffer[sizeof(struct Test_Card_Two)];
 }TestCard_Two;  //B卡

 typedef union
{
  struct Test_Card_Three
    {
      int iMg[Cpoints];
      int iCa[Cpoints];
      int K[Cpoints];
      int Na[Cpoints];
      int PH[Cpoints];
			int Cl[Cpoints];
			int CO2_1[Cpoints];
			int CO2_2[Cpoints];
			int glu[Cpoints];
			int pO2[Cpoints];
			int Lac[Cpoints];
     }TestCardThree;    
     uint8_t buffer[sizeof(struct Test_Card_Three)];
 }TestCard_Three;  //C卡

 
/*API函数*/
int8_t DetectBoard_Initial(void);  //1s
 
int8_t DetectBoard_SelfCheck(void);//自检
 
int8_t DetectBoard_Check_R1(void);//进液
 
int8_t DetectBoard_Check_R2(void);//到位

int8_t DetectBoard_CardA_Config(void);
 
int8_t DetectBoard_GetCartridgeAData(TestCard_One * CartridgeA); //<1s
 
int8_t DetectBoard_GetCartridgeBData(TestCard_Two * CartridgeB); //<1s
int8_t DetectBoard_GetCartridgeCData(TestCard_Three * CartridgeC);  //<2s
 
void DetectBoardResetAll(void);//复位所有状态，需要在测试完成以后调用

#endif /* __DETECTBOARD_H__ */
