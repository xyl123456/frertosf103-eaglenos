#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "error.h"
#include "cmd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//外部中断 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/01  
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  
 
uint8_t CardCheckFlag=0;
//外部中断初始化函数
void EXTIX_Init(void)
{
 
	EXTI_InitTypeDef EXTI_InitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟

	  KEY_Init();//初始化按键对应io模式

    //GPIOB.1中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
 
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x09;//抢占优先级9
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}

//TaskHandle_t MessageProcessTask_Handler;

//extern TaskHandle_t MessageProcessTask_Handler;

void EXTI1_IRQHandler(void)
{
	//BaseType_t xHigherPriorityTaskWoken;
	delay_xms(300);
	if(CARD_CHECK==0){
		CardCheckFlag=1;
		/*
				xTaskNotifyFromISR((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_CARD_INSERT_SUCCESS,
									(eNotifyAction)eSetBits,
									(BaseType_t*)xHigherPriorityTaskWoken);
		*/
				//portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	
	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位  
}

TaskHandle_t Task2Task_Handler;

//任务句柄
extern TaskHandle_t Task2Task_Handler;

void EXTI9_5_IRQHandler(void)
{	
	BaseType_t YieldRequired;	
	delay_xms(10);   //消抖			 
	if(KEY0==0)	
	{
		YieldRequired=xTaskResumeFromISR(Task2Task_Handler);//恢复任务2
		printf("恢复任务2的运行!\r\n");
		if(YieldRequired==pdTRUE)
		{
			/*如果函数xTaskResumeFromISR()返回值为pdTRUE，那么说明要恢复的这个
			任务的任务优先级等于或者高于正在运行的任务(被中断打断的任务),所以在
			退出中断的时候一定要进行上下文切换！*/
			portYIELD_FROM_ISR(YieldRequired);
		}
	}
 	 EXTI_ClearITPendingBit(EXTI_Line5);    //清除LINE5上的中断标志位  
}
