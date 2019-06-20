#include "sys.h"
#include "usart.h"	 
#include "string.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用	
#include "task.h"
#include "semphr.h"	 
#endif
  
 
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
}

void _ttywrch(int ch)
{
    ch = ch;
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
uint8_t  uart1StartFlag=0;
uint8_t  uart2StartFlag=0;

void UartInit(){
	#if EN_USART1_RX   //如果使能了接收
	Uart1Init(115200);
	#endif
	#if EN_USART2_RX   //如果使能了接收
	Uart2Init(115200);
	#endif
	#if EN_USART4_RX   //如果使能了接收
	Uart4Init(115200);
	#endif
}

//串口发送函数
void MyUartSend(u8 port,char buf[],u8 len)
{
	u8 t;
	
	for(t=0;t<len;t++)
	{
		if(port==1)
		{
			USART_SendData(USART1,buf[t]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
		}
		if(port==2)
		{
			USART_SendData(USART2,buf[t]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
		if(port==3)
		{
			USART_SendData(USART3,buf[t]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
		if(port==4)
		{
			USART_SendData(UART4,buf[t]);
			while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
		}
	}
}


extern TaskHandle_t UartTask_Handler;	//接收任务通知的任务句柄

 
#if EN_USART1_RX   //如果使能了接收

char uart1RxBuf[USART1_REC_LEN];     //接收缓冲,最大USART1_REC_LEN个字节.
u16 uart1RxSta=0;       //接收状态标记	  
u16 uart1ReciveSufixCount;//后缀"}"
u16 uart1RecivePrefixCount;//前缀"{"

void Uart1Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8 ;//抢占优先级8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
}



void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	char resData;
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
		{
			resData =USART_ReceiveData(USART1);	//读取接收到的数据
			
		if(uart1RxSta==0){
			if(resData==0x7B){
				//start receive
				uart1StartFlag=1;
				}else{
				uart1StartFlag=0;
				}
			}
		if(uart1StartFlag==1){
					if((uart1RxSta&0x8000)==0){
					if(resData==0x7B){
						uart1RecivePrefixCount++;
					}else if(resData==0x7D){
						uart1ReciveSufixCount++;
					}
					if(uart1RecivePrefixCount==uart1ReciveSufixCount){
						uart1RxSta|=0x8000;
						uart1RecivePrefixCount=0;
						uart1ReciveSufixCount=0;
					}
					uart1RxBuf[uart1RxSta&0X3FFF]=resData;
					uart1RxSta++;
					//超过多个"{}"重新接收,或错误的数据超过128个
					if((uart1RecivePrefixCount>20)||(uart1ReciveSufixCount>20)||((uart1RxSta&0X3FFF)>256)){
						uart1RxSta=0;
						uart1RecivePrefixCount=0;
						uart1ReciveSufixCount=0;
						}
					}
			}else{
				uart1RxSta=0;
				uart1StartFlag=0;
			}
	}
	//发送任务通知
	if((uart1RxSta&0x8000)&&(UartTask_Handler!=NULL))//接收到数据，并且接收任务通知的任务有效
	{
		vTaskNotifyGiveFromISR(UartTask_Handler,&xHigherPriorityTaskWoken);//发送任务通知
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}	 
} 
#endif	

#if EN_USART2_RX   //如果使能了接收
 	
char uart2RxBuf[USART2_REC_LEN];     //接收缓冲,最大USART2_REC_LEN个字节

u16 uart2RxSta=0;       //接收状态标记
u16 uart2ReciveSufixCount;//后缀
u16 uart2RecivePrefixCount;//前缀

void Uart2Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2，GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8 ;//抢占优先级8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//开启IDLE中断,接收一数据帧
	USART_Cmd(USART2, ENABLE);                    //使能串口2 
}
//extern TaskHandle_t UART2Task_Handler;	//接收任务通知的任务句柄

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	char resData;
	BaseType_t xHigherPriorityTaskWoken;
	USART_ClearFlag(USART2,USART_FLAG_TC);
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
		{
//		resData =USART_ReceiveData(USART2);	//读取接收到的数据
//		if(uart2RxSta==0){
//			if(resData==0x7B){
//				//start receive
//				uart2StartFlag=1;
//				}else{
//					uart2StartFlag=0;
//				}
//			}

//		if(uart2StartFlag==1){
//				if((uart2RxSta&0x8000)==0){
//				if(resData==0x7B){
//					uart2RecivePrefixCount++;
//				}else if(resData==0x7D){
//					uart2ReciveSufixCount++;
//				}
//				if(uart2RecivePrefixCount==uart2ReciveSufixCount){
//					uart2RxSta|=0x8000;
//					uart2RecivePrefixCount=0;
//					uart2ReciveSufixCount=0;
//				}
//				uart2RxBuf[uart2RxSta&0X3FFF]=resData;
//				uart2RxSta++;
//				//超过多个"{}"重新接收
//				if((uart2RecivePrefixCount>20)||(uart2ReciveSufixCount>20)||((uart2RxSta&0X3FFF)>128)){
//					uart2RxSta=0;
//					uart2RecivePrefixCount=0;
//					uart2ReciveSufixCount=0;
//					}
//				}	
//		}else{
//			uart2RxSta=0;
//			uart2StartFlag=0;
//		}
			resData =USART_ReceiveData(USART2);
			uart2RxBuf[uart2RxSta++]=resData;
	 }
		else if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE)!=Bit_RESET)
		{
			resData=USART2->SR;
			resData=USART2->DR;
			uart2RxSta|=0x8000;
		}
	//发送任务通知
	if((uart2RxSta&0x8000)&&(UartTask_Handler!=NULL))//接收到数据，并且接收任务通知的任务有效
	{
		vTaskNotifyGiveFromISR(UartTask_Handler,&xHigherPriorityTaskWoken);//发送任务通知
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}		
} 
#endif



#if EN_USART4_RX   //如果使能了接收
//串口4中断服务程序

char uart4RxBuf[USART2_REC_LEN];     //接收缓冲,最大USART4_REC_LEN个字节.

u16 uart4RxSta=0;       //接收状态标记
u16 uart4ReciveSufixCount;//后缀"}"
u16 uart4RecivePrefixCount;//前缀"{"
  
void Uart4Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART4，GPIOC时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//USART4_TX   GPIOC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.10
   
	//USART4_RX	  GPIOC.11初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.11 

	//Usart4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8 ;//抢占优先级8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART4, &USART_InitStructure); //初始化串口4
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(UART4, ENABLE);                    //使能串口4
}

//extern TaskHandle_t UART4Task_Handler;	//接收任务通知的任务句柄

void UART4_IRQHandler(void)                	//串口4中断服务程序
{
	char resData;
	BaseType_t xHigherPriorityTaskWoken;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		resData =USART_ReceiveData(UART4);	//读取接收到的数据
		 
		if((uart4RxSta&0x8000)==0){
			if(resData==0x7B){
				uart4RecivePrefixCount++;
			}else if(resData==0x7D){
				uart4ReciveSufixCount++;
			}
			if(uart4RecivePrefixCount==uart4ReciveSufixCount){
				uart4RxSta|=0x8000;
				uart4RecivePrefixCount=0;
				uart4ReciveSufixCount=0;
			}
			uart4RxBuf[uart4RxSta&0X3FFF]=resData;
			uart4RxSta++;
			//超过多个"{}"重新接收,或者超过一定的最大接收数据
			if((uart4RecivePrefixCount>20)||(uart4ReciveSufixCount>20)||((uart4RxSta&0X3FFF)>128)){
				uart4RxSta=0;
				uart4RecivePrefixCount=0;
				uart4ReciveSufixCount=0;
			}
     }
	 }
	//发送任务通知
	if((uart4RxSta&0x8000)&&(UartTask_Handler!=NULL))//接收到数据，并且接收任务通知的任务有效
	{
		vTaskNotifyGiveFromISR(UartTask_Handler,&xHigherPriorityTaskWoken);//发送任务通知
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}		
} 
#endif



