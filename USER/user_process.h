#ifndef __USER_PROCESS_H
#define __USER_PROCESS_H

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif 

#define DBUG_TEST     0 //关闭DBUG调试

//#define HEATCONTROL    PBout(0)   //if use io control
#define SCANCODEKEY    PBout(10)	// PB10

#define SAMPLE_COLLECT_CARD_ONE      1
#define SAMPLE_COLLECT_CARD_TWO      2
#define SAMPLE_COLLECT_CARD_THREE    3
#define SAMPLE_COLLECT_CARD_FOUR     4
#define SAMPLE_COLLECT_CARD_FIVE     5

#define CHECK_VACUOLE_TIMER          10					//s
#define CHECK_SAMPLE_TIMER           10         //s

//用于定义处理任务事件
#define EVENT_PUSH_HEAT_CUT          1             //推加热片
#define EVENT_BREAK_VACUOLE          2						 //刺破液包
#define EVENT_PUSH_TEST_VACUOLE      3						 //继续推液包里面的检测体
#define EVENT_TEST_VACUOLE_START     4     	     	 //开始检测标液
#define EVENT_TEST_SAMPLE_START      5             //开始检测样本液
#define EVENT_SAMPLE_TEST_RESECT     6             //复位所有设备

//标液检测标志停止与开始
#define START_SAMPLE_TEST            1
#define STOP_SAMPLE_TEST             0


//定义是标液检测值
#define TEST_VACUOLE_ID              0
//定义样品检测值
#define TEST_SAMPLE_ID              1

void Uart1Process(char *uartMemary,int uartLen);
void ProcessValueKeyDate(char *keyString,char *valueString);

void Uart2Process(char *uartMemary,int uartLen);

void StopScanCode(void);

void CardCheckInit(void);
void HeatInit(void);
void StopHeatBody(void);


void SelectCardSort(uint8_t sort);

void EventProcess(uint32_t NotifyValue);
void ScanSensorTime(void);
void ScanImpedance(void);//实时测试阻抗
void BreakVacuole(void);//刺破液包

void 	StartTestVacuole(uint32_t second);//液包液样品采集
void  StartSampleVacuole(uint32_t second);//样品液的数据采集
uint8_t PushHeatCut(void);
uint8_t TestImpedanceIsOk(void);//检测液体是否到位
uint8_t PushVacuole(void);//继续推里面的样品检测液体 
 
uint32_t SelfTestProcess(void);
uint32_t IntialProcess(void);
uint32_t ScanCodeInit(void);
 
uint32_t PreTestProcess(void);
void StartHeatBody(float tempValue);
 
void SendSampleCollect(uint8_t sort,uint8_t vacuoleId);


void ResectAllDevice(void);//复位所有外设
void ResectHeatMoto(void);//复位加热电机
void ResectBreakMoto(void);//复位压液包的电机设备

void SendSampleFlagData(uint8_t cmd,uint8_t cmd_value);
 
extern void HalBoardInit(void);
extern void MessageProcess(uint32_t NotifyValue);
extern void UartProcess(u8 uartPort,char *uartMemary,int uartLen);

extern void ScanHeatingTemp(void);
#ifdef __cplusplus
}
#endif

#endif /* __USER_PROCESS_H */

