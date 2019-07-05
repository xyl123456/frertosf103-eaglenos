#ifndef __DETECTBOARD_H__
#define __DETECTBOARD_H__

#include <stdint.h>
#include "AD7124.h"
#include "platform_drivers.h"

/*DAC通道值*/
#define  DAC_0_VALUE   0x8a3c    // 0.1V+1.25V
#define  DAC_1_VALUE   0x947a    // 0.2V+1.25V
#define  DAC_2_VALUE   0x9eb7    // 0.3V+1.25V
#define  DAC_3_VALUE   0xa8f5    // 0.4V+1.25V
#define  DAC_4_VALUE   0xb332    // 0.5V+1.25V 
#define  DAC_5_VALUE   0x7fff    // 1.25V

#define  A_POINTS  5
#define  B_POINTS  5
#define  C_POINTS  5

/*检测卡数据结构*/
typedef union
{
  struct Test_Card_One
    {
      int iMg[A_POINTS];
      int iCa[A_POINTS];
      int K[A_POINTS];
      int Na[A_POINTS];
      int Cl[A_POINTS]; 
     }TestCardOne;   
     uint8_t buffer[sizeof(struct Test_Card_One)];
 }TestCard_One;  //A卡

typedef union
{
  struct Test_Card_Two
    {
      int PH[B_POINTS];
      int CO2_1[B_POINTS];
      int CO2_2[B_POINTS];
      int glu[B_POINTS];
      int pO2[B_POINTS]; 
      int Lac[B_POINTS];
     }TestCardTwo;    
     uint8_t buffer[sizeof(struct Test_Card_Two)];
 }TestCard_Two;  //B卡

 typedef union
{
  struct Test_Card_Three
    {
      int iMg[C_POINTS];
      int iCa[C_POINTS];
      int K[C_POINTS];
      int Na[C_POINTS];
      int PH[C_POINTS];
			int Cl[C_POINTS];
			int CO2_1[C_POINTS];
			int CO2_2[C_POINTS];
			int glu[C_POINTS];
			int pO2[C_POINTS];
			int Lac[C_POINTS];
     }TestCardThree;    
     uint8_t buffer[sizeof(struct Test_Card_Three)];
 }TestCard_Three;  //C卡

 
/*API函数*/
int8_t DetectBoard_Initial(void);  //1s
int8_t DetectBoard_SelfCheck(void);
int8_t DetectBoard_Check_R1(void);
int8_t DetectBoard_Check_R2(void);

int8_t DetectBoard_CardA_Config(void);
int8_t DetectBoard_GetCartridgeAData(TestCard_One * CartridgeA); //<1s
int8_t DetectBoard_GetCartridgeBData(TestCard_Two * CartridgeB); //<1s
int8_t DetectBoard_GetCartridgeCData(TestCard_Three * CartridgeC);  //<2s

#endif /* __DETECTBOARD_H__ */
