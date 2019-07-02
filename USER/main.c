/***********************************************************************************
Copyright  agilebiotec Co.,Ltd. 2019-2029.All right reserved.
File name:     main.c
Author:        xuyunlong
Date:          2019-6-11
Description:   main�������������
History:
***********************************************************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "exti.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "user_process.h"
#include "limits.h"

#include "cmd.h"
#include "error.h"
#include "max6675.h"
#include "DetectBoard.h"
#include "SPI2.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		512 
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//��������
#define UART_TASK_PRIO		2
#define UART_STK_SIZE 		512
TaskHandle_t UartTask_Handler;
void uart_task(void *pvParameters);


uint32_t autoLoadTimeFlag=0;
//��ʱ������
#define TIMERCONTROL_TASK_PRIO		3
#define TIMERCONTROL_STK_SIZE 		128  
TaskHandle_t TimerControlTask_Handler;
void timercontrol_task(void *pvParameters);


//��Ϣ��������
#define MESSAGEPROCESS_TASK_PRIO		4
#define MESSAGEPROCESS_STK_SIZE 		512
TaskHandle_t MessageProcessTask_Handler;
void messageprocess_task(void *pvParameters);


//�¼���������
#define EVENTPROCESS_TASK_PRIO		5
#define EVENTPROCESS_STK_SIZE 		512
TaskHandle_t EventProcessTask_Handler;
void eventprocess_task(void *pvParameters);


TimerHandle_t  AutoReloadTimer_Handle;	//�����Զ�ʱ��

void AutoReloadCallback(TimerHandle_t xTimer); 	//���ڶ�ʱ���ص�����

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 	 
	delay_init();	
	//��ʱ������ʼ��	  
	
	//������ʼ����
  xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
   vTaskStartScheduler();          //�����������						
}

//��ʼ����������
void start_task(void *pvParameters)
{
	UartInit();					//��ʼ������
	LedInit();		  		//��ʼ��LED
	HalBoardInit();			//������������ɨ���豸�������豸����ӡ�豸���迹�����豸,������忨�Ƿ�λ
	EXTIX_Init();				//�ⲿ�ж�
	my_mem_init(SRAMIN);            	//��ʼ���ڲ��ڴ��
	
	HalBoardInit();//��Χ�ӿڳ�ʼ��
  taskENTER_CRITICAL();           //�����ٽ���
		//���������Զ�ʱ��
		AutoReloadTimer_Handle = xTimerCreate(
											(const char* )"AutoReloadTimer",
											(TickType_t			)500/portTICK_PERIOD_MS,
											(UBaseType_t		)pdTRUE,
											(void*				)1,
											(TimerCallbackFunction_t)AutoReloadCallback);
			xTimerStart(AutoReloadTimer_Handle,0);	//�������ڶ�ʱ��
/*											
		//������ʱ������(�����ʱ)
    xTaskCreate((TaskFunction_t )timercontrol_task,             
                (const char*    )"timercontrol_task",           
                (uint16_t       )TIMERCONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
                (TaskHandle_t*  )&TimerControlTask_Handler);    
*/								
    //����uart����
    xTaskCreate((TaskFunction_t )uart_task,     	
                (const char*    )"uart_task",   	
                (uint16_t       )UART_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )UART_TASK_PRIO,	
                (TaskHandle_t*  )&UartTask_Handler); 

		//��������������
    xTaskCreate((TaskFunction_t )messageprocess_task,     	
                (const char*    )"messageprocess_task",   	
                (uint16_t       )MESSAGEPROCESS_STK_SIZE, 
                (void*          )NULL,		
                (UBaseType_t    )MESSAGEPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&MessageProcessTask_Handler); 
		//�����¼���������
		xTaskCreate((TaskFunction_t )eventprocess_task,     	
                (const char*    )"eventprocess_task",   	
                (uint16_t       )EVENTPROCESS_STK_SIZE, 
                (void*          )NULL,		
                (UBaseType_t    )EVENTPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&EventProcessTask_Handler); 
								
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
		
}

void timercontrol_task(void *pvParameters)
{
	if(autoLoadTimeFlag==0){
		xTimerStart(AutoReloadTimer_Handle,0);	//�������ڶ�ʱ��
		autoLoadTimeFlag=1;
	}
	while(1){
		vTaskDelay(10);
	}
	//vTaskDelete(TimerControlTask_Handler); //ɾ������
}

//���ڶ�ʱ���Ļص�����
void AutoReloadCallback(TimerHandle_t xTimer)
{
	if(CardCheckFlag==1){
		//���嵽λ��
		xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_CARD_INSERT_SUCCESS,
									(eNotifyAction)eSetBits);
		CardCheckFlag=0;	
	}
	ScanSensorTime();//
	ScanHeatingTemp();
	ScanImpedance();//ʵʱ�����迹����
	SYSLED=!SYSLED;
}

//uart������ 
void uart_task(void *pvParameters)
{
	u32	notifyValue;
  while(1)
    {
			//xTaskNotifyWait()����ulTaskNotifyTake()
			notifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
			if(notifyValue==1){//��0ǰ����֪ͨΪ1˵����Ч
				//�ж����Ǹ���������
				if(uart1RxSta&0x8000){
					u16 uart1Len;
					///char *uart1Memary;
					uart1Len=uart1RxSta&0x3fff;
					//uart1Memary=mymalloc(SRAMIN,uart1Len+1);
					//mymemcpy(uart1Memary,uart1RxBuf,uart1Len);
					//�������ݺ���
					//UartProcess(UART1_PORT,uart1Memary,uart1Len);
					UartProcess(UART1_PORT,uart1RxBuf,uart1Len);
					//�ͷſռ�
					uart1RxSta=0;
					mymemset(uart1RxBuf,0,USART1_REC_LEN);
					//myfree(SRAMIN,uart1Memary);
				}
				if(uart2RxSta&0x8000){
					u16 uart2Len;
					uart2Len=uart2RxSta&0x3fff;
					UartProcess(UART2_PORT,uart2RxBuf,uart2Len);
					mymemset(uart2RxBuf,0,USART2_REC_LEN);
					uart2RxSta=0;
				}
				if(uart4RxSta&0x8000){
					
				}
			}
			vTaskDelay(10);	
    }
}

//������������
void messageprocess_task(void *pvParameters)
{
	uint32_t NotifyValue;
	BaseType_t err;
	while(1){
		err=xTaskNotifyWait((uint32_t	)0x00,				//���뺯����ʱ���������bit
							(uint32_t	)ULONG_MAX,			//�˳�������ʱ��������е�bit
							(uint32_t*	)&NotifyValue,		//��������ֵ֪ͨ
							(TickType_t	)portMAX_DELAY);	//����ʱ��
		
		if(err==pdPASS)
		{
			MessageProcess(NotifyValue);
		}else{
			//��ȡ������Ϣʧ��
		}
	}
}

//�¼�����������Ҫ������ʱ������
void eventprocess_task(void *pvParameters)
{
	uint32_t NotifyValue;
	BaseType_t err;
	while(1){
		err=xTaskNotifyWait((uint32_t)0x00,
							(uint32_t)ULONG_MAX,
							(uint32_t*)&NotifyValue,
							(TickType_t)portMAX_DELAY);
		if(err==pdPASS)
		{
			EventProcess(NotifyValue);
		}else{
		}
	}
}

