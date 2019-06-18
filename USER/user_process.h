#ifndef __USER_PROCESS_H
#define __USER_PROCESS_H

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif 

typedef union
{
  struct Test_Card_One
    {
			uint32_t Na;
      uint32_t K;
      uint32_t Li;
      uint32_t ph;
      uint32_t ca; 
			uint32_t o2;
     }TestCardOne;    
     uint8_t buffer[sizeof(struct Test_Card_One)];
 }TestCard_One;
 
typedef union
{
  struct Test_Card_Two
    {
			uint32_t Na;
      uint32_t K;
      uint32_t Li;
      uint32_t ph;
      uint32_t ca; 
			uint32_t o2;
     }TestCardTwo;    
     uint8_t buffer[sizeof(struct Test_Card_Two)];
 }TestCard_Two;

typedef union
{
  struct Test_Card_Three
    {
			uint32_t Na;
      uint32_t K;
      uint32_t Li;
      uint32_t ph;
      uint32_t ca; 
			uint32_t o2;
     }TestCardThree;    
     uint8_t buffer[sizeof(struct Test_Card_Three)];
 }TestCard_Three;

 typedef union
{
  struct Test_Card_Four
    {
			uint32_t Na;
      uint32_t K;
      uint32_t Li;
      uint32_t ph;
      uint32_t ca; 
			uint32_t o2;
     }TestCardFour;    
     uint8_t buffer[sizeof(struct Test_Card_Four)];
 }TestCard_Four;

 
 typedef union
{
  struct Test_Card_Five
    {
			uint32_t Na;
      uint32_t K;
      uint32_t Li;
      uint32_t ph;
      uint32_t ca; 
			uint32_t o2;
     }TestCardThree;    
     uint8_t buffer[sizeof(struct Test_Card_Five)];
 }TestCard_Five;

//#define HEATCONTROL    PBout(0)   //if use io control
#define SCANCODEKEY    PBout(10)	// PB10

void Uart1Process(char *uartMemary,u8 uartLen);
void ProcessValueKeyDate(char *keyString,char *valueString);

void Uart2Process(char *uartMemary,u8 uartLen);

void CardCheckInit(void);
void HeatInit(void);
 
uint32_t SelfTestProcess(void);
uint32_t IntialProcess(void);
uint32_t ScanCodeInit(void);
 
uint32_t PreTestProcess(void);
void StartHeatBody(float tempValue);
 
 
extern void HalBoardInit(void);
extern void MessageProcess(uint32_t NotifyValue);
extern void UartProcess(u8 uartPort,char *uartMemary,u8 uartLen);

extern void ScanHeatingTemp(void);
#ifdef __cplusplus
}
#endif

#endif /* __USER_PROCESS_H */

