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
      uint32_t iMg[Apoints];
      uint32_t iCa[Apoints];
      uint32_t K[Apoints];
      uint32_t Na[Apoints];
      uint32_t Cl[Apoints]; 
     }TestCardOne;   
     uint8_t buffer[sizeof(struct Test_Card_One)];
 }TestCard_One;  //A卡

typedef union
{
  struct Test_Card_Two
    {
      uint32_t PH[Bpoints];
      uint32_t CO2_1[Bpoints];
      uint32_t CO2_2[Bpoints];
      uint32_t glu[Bpoints];
      uint32_t pO2[Bpoints]; 
      uint32_t Lac[Bpoints];
     }TestCardTwo;    
     uint8_t buffer[sizeof(struct Test_Card_Two)];
 }TestCard_Two;  //B卡

 typedef union
{
  struct Test_Card_Three
    {
      uint32_t iMg[Cpoints];
      uint32_t iCa[Cpoints];
      uint32_t K[Cpoints];
      uint32_t Na[Cpoints];
      uint32_t PH[Cpoints];
			uint32_t Cl[Cpoints];
			uint32_t CO2_1[Cpoints];
			uint32_t CO2_2[Cpoints];
			uint32_t glu[Cpoints];
			uint32_t pO2[Cpoints];
			uint32_t Lac[Cpoints];
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

#endif /* __DETECTBOARD_H__ */
