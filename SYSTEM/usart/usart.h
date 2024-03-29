#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
#define USART1_REC_LEN  			512  	//定义最大接收字节数 512
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

#define USART2_REC_LEN  			512  	//定义最大接收字节数 512
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收


#define USART4_REC_LEN  			512  	//定义最大接收字节数 512
#define EN_USART4_RX 			1	//使能（1）/禁止（0）串口4接收

#define UART1_PORT    1
#define UART2_PORT    2
#define UART3_PORT    3
#define UART4_PORT    4
	  	
extern char  uart1RxBuf[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 uart1RxSta;         		//接收状态标记	

extern char  uart2RxBuf[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 uart2RxSta;         		//接收状态标记	

extern char  uart4RxBuf[USART4_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 uart4RxSta;         		//接收状态标记	

//如果想串口中断接收，请不要注释以下宏定义
void Uart1Init(u32 bound);
void Uart2Init(u32 bound);
void Uart4Init(u32 bound);


extern void MyUartSend(u8 port,char buf[],int len);
extern void UartInit(void);
#endif


