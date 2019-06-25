#ifndef __USER_PROCESS_H
#define __USER_PROCESS_H

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif 

//#define HEATCONTROL    PBout(0)   //if use io control
#define SCANCODEKEY    PBout(10)	// PB10

#define SAMPLE_COLLECT_CARD_ONE      1
#define SAMPLE_COLLECT_CARD_TWO      2
#define SAMPLE_COLLECT_CARD_THREE    3
#define SAMPLE_COLLECT_CARD_FOUR     4
#define SAMPLE_COLLECT_CARD_FIVE     5

void Uart1Process(char *uartMemary,int uartLen);
void ProcessValueKeyDate(char *keyString,char *valueString);

void Uart2Process(char *uartMemary,int uartLen);

void CardCheckInit(void);
void HeatInit(void);
void StopHeatBody(void);

void ScanSensorTime(void);
 
uint32_t SelfTestProcess(void);
uint32_t IntialProcess(void);
uint32_t ScanCodeInit(void);
 
uint32_t PreTestProcess(void);
void StartHeatBody(float tempValue);
 
void SendSampleCollect(uint8_t sort);
 
extern void HalBoardInit(void);
extern void MessageProcess(uint32_t NotifyValue);
extern void UartProcess(u8 uartPort,char *uartMemary,int uartLen);

extern void ScanHeatingTemp(void);
#ifdef __cplusplus
}
#endif

#endif /* __USER_PROCESS_H */

