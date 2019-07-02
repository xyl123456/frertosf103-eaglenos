#ifndef __USER_PROCESS_H
#define __USER_PROCESS_H

#include "sys.h"

#ifdef __cplusplus
 extern "C" {
#endif 

#define DBUG_TEST     0 //�ر�DBUG����

//#define HEATCONTROL    PBout(0)   //if use io control
#define SCANCODEKEY    PBout(10)	// PB10

#define SAMPLE_COLLECT_CARD_ONE      1
#define SAMPLE_COLLECT_CARD_TWO      2
#define SAMPLE_COLLECT_CARD_THREE    3
#define SAMPLE_COLLECT_CARD_FOUR     4
#define SAMPLE_COLLECT_CARD_FIVE     5

#define CHECK_VACUOLE_TIMER          10					//s
#define CHECK_SAMPLE_TIMER           10         //s

//���ڶ��崦�������¼�
#define EVENT_PUSH_HEAT_CUT          1             //�Ƽ���Ƭ
#define EVENT_BREAK_VACUOLE          2						 //����Һ��
#define EVENT_PUSH_TEST_VACUOLE      3						 //������Һ������ļ����
#define EVENT_TEST_VACUOLE_START     4     	     	 //��ʼ����Һ
#define EVENT_TEST_SAMPLE_START      5             //��ʼ�������Һ
#define EVENT_SAMPLE_TEST_RESECT     6             //��λ�����豸

//��Һ����־ֹͣ�뿪ʼ
#define START_SAMPLE_TEST            1
#define STOP_SAMPLE_TEST             0


//�����Ǳ�Һ���ֵ
#define TEST_VACUOLE_ID              0
//������Ʒ���ֵ
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
void ScanImpedance(void);//ʵʱ�����迹
void BreakVacuole(void);//����Һ��

void 	StartTestVacuole(uint32_t second);//Һ��Һ��Ʒ�ɼ�
void  StartSampleVacuole(uint32_t second);//��ƷҺ�����ݲɼ�
uint8_t PushHeatCut(void);
uint8_t TestImpedanceIsOk(void);//���Һ���Ƿ�λ
uint8_t PushVacuole(void);//�������������Ʒ���Һ�� 
 
uint32_t SelfTestProcess(void);
uint32_t IntialProcess(void);
uint32_t ScanCodeInit(void);
 
uint32_t PreTestProcess(void);
void StartHeatBody(float tempValue);
 
void SendSampleCollect(uint8_t sort,uint8_t vacuoleId);


void ResectAllDevice(void);//��λ��������
void ResectHeatMoto(void);//��λ���ȵ��
void ResectBreakMoto(void);//��λѹҺ���ĵ���豸

void SendSampleFlagData(uint8_t cmd,uint8_t cmd_value);
 
extern void HalBoardInit(void);
extern void MessageProcess(uint32_t NotifyValue);
extern void UartProcess(u8 uartPort,char *uartMemary,int uartLen);

extern void ScanHeatingTemp(void);
#ifdef __cplusplus
}
#endif

#endif /* __USER_PROCESS_H */

