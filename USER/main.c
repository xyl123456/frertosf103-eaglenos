/***********************************************************************************
Copyright  agilebiotec Co.,Ltd. 2019-2029.All right reserved.
File name:     main.c
Author:        xuyunlong
Date:          2019-6-11
Description:   main函数，任务分配
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

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		512 
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//串口任务
#define UART_TASK_PRIO		2
#define UART_STK_SIZE 		512
TaskHandle_t UartTask_Handler;
void uart_task(void *pvParameters);


uint32_t autoLoadTimeFlag=0;
//定时器任务
#define TIMERCONTROL_TASK_PRIO		3
#define TIMERCONTROL_STK_SIZE 		128  
TaskHandle_t TimerControlTask_Handler;
void timercontrol_task(void *pvParameters);


//消息处理任务
#define MESSAGEPROCESS_TASK_PRIO		4
#define MESSAGEPROCESS_STK_SIZE 		512
TaskHandle_t MessageProcessTask_Handler;
void messageprocess_task(void *pvParameters);


//事件处理任务
#define EVENTPROCESS_TASK_PRIO		5
#define EVENTPROCESS_STK_SIZE 		512
TaskHandle_t EventProcessTask_Handler;
void eventprocess_task(void *pvParameters);


TimerHandle_t  AutoReloadTimer_Handle;	//周期性定时器

void AutoReloadCallback(TimerHandle_t xTimer); 	//周期定时器回调函数

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 	 
	delay_init();	
	//延时函数初始化	  
	
	//创建开始任务
  xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
   vTaskStartScheduler();          //开启任务调度						
}

//开始任务任务函数
void start_task(void *pvParameters)
{
	UartInit();					//初始化串口
	LedInit();		  		//初始化LED
	HalBoardInit();			//外设驱动包含扫码设备，加热设备，打印设备，阻抗电流设备,外设检测插卡是否到位
	EXTIX_Init();				//外部中断
	my_mem_init(SRAMIN);            	//初始化内部内存池
	
	HalBoardInit();//外围接口初始化
  taskENTER_CRITICAL();           //进入临界区
		//创建周期性定时器
		AutoReloadTimer_Handle = xTimerCreate(
											(const char* )"AutoReloadTimer",
											(TickType_t			)500/portTICK_PERIOD_MS,
											(UBaseType_t		)pdTRUE,
											(void*				)1,
											(TimerCallbackFunction_t)AutoReloadCallback);
			xTimerStart(AutoReloadTimer_Handle,0);	//开启周期定时器
/*											
		//创建定时器任务(软件定时)
    xTaskCreate((TaskFunction_t )timercontrol_task,             
                (const char*    )"timercontrol_task",           
                (uint16_t       )TIMERCONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
                (TaskHandle_t*  )&TimerControlTask_Handler);    
*/								
    //创建uart任务
    xTaskCreate((TaskFunction_t )uart_task,     	
                (const char*    )"uart_task",   	
                (uint16_t       )UART_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )UART_TASK_PRIO,	
                (TaskHandle_t*  )&UartTask_Handler); 

		//创建错误处理任务
    xTaskCreate((TaskFunction_t )messageprocess_task,     	
                (const char*    )"messageprocess_task",   	
                (uint16_t       )MESSAGEPROCESS_STK_SIZE, 
                (void*          )NULL,		
                (UBaseType_t    )MESSAGEPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&MessageProcessTask_Handler); 
		//创建事件处理任务
		xTaskCreate((TaskFunction_t )eventprocess_task,     	
                (const char*    )"eventprocess_task",   	
                (uint16_t       )EVENTPROCESS_STK_SIZE, 
                (void*          )NULL,		
                (UBaseType_t    )EVENTPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&EventProcessTask_Handler); 
								
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
		
}

void timercontrol_task(void *pvParameters)
{
	if(autoLoadTimeFlag==0){
		xTimerStart(AutoReloadTimer_Handle,0);	//开启周期定时器
		autoLoadTimeFlag=1;
	}
	while(1){
		vTaskDelay(10);
	}
	//vTaskDelete(TimerControlTask_Handler); //删除任务
}

//周期定时器的回调函数
void AutoReloadCallback(TimerHandle_t xTimer)
{
	if(CardCheckFlag==1){
		//卡插到位了
		xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_CARD_INSERT_SUCCESS,
									(eNotifyAction)eSetBits);
		CardCheckFlag=0;	
	}
	ScanSensorTime();//
	ScanHeatingTemp();
	ScanImpedance();//实时测试阻抗函数
	SYSLED=!SYSLED;
}

//uart任务函数 
void uart_task(void *pvParameters)
{
	u32	notifyValue;
  while(1)
    {
			//xTaskNotifyWait()类似ulTaskNotifyTake()
			notifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
			if(notifyValue==1){//清0前任务通知为1说明有效
				//判断是那个串口数据
				if(uart1RxSta&0x8000){
					u16 uart1Len;
					///char *uart1Memary;
					uart1Len=uart1RxSta&0x3fff;
					//uart1Memary=mymalloc(SRAMIN,uart1Len+1);
					//mymemcpy(uart1Memary,uart1RxBuf,uart1Len);
					//处理数据函数
					//UartProcess(UART1_PORT,uart1Memary,uart1Len);
					UartProcess(UART1_PORT,uart1RxBuf,uart1Len);
					//释放空间
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

//错误处理任务函数
void messageprocess_task(void *pvParameters)
{
	uint32_t NotifyValue;
	BaseType_t err;
	while(1){
		err=xTaskNotifyWait((uint32_t	)0x00,				//进入函数的时候不清除任务bit
							(uint32_t	)ULONG_MAX,			//退出函数的时候清除所有的bit
							(uint32_t*	)&NotifyValue,		//保存任务通知值
							(TickType_t	)portMAX_DELAY);	//阻塞时间
		
		if(err==pdPASS)
		{
			MessageProcess(NotifyValue);
		}else{
			//获取错误信息失败
		}
	}
}

//事件处理函数，主要处理延时的任务
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

