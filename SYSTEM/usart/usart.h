#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
#define USART1_REC_LEN  			512  	//�����������ֽ��� 512
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define USART2_REC_LEN  			512  	//�����������ֽ��� 512
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����


#define USART4_REC_LEN  			512  	//�����������ֽ��� 512
#define EN_USART4_RX 			1	//ʹ�ܣ�1��/��ֹ��0������4����

#define UART1_PORT    1
#define UART2_PORT    2
#define UART3_PORT    3
#define UART4_PORT    4
	  	
extern char  uart1RxBuf[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 uart1RxSta;         		//����״̬���	

extern char  uart2RxBuf[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 uart2RxSta;         		//����״̬���	

extern char  uart4RxBuf[USART4_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 uart4RxSta;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void Uart1Init(u32 bound);
void Uart2Init(u32 bound);
void Uart4Init(u32 bound);


extern void MyUartSend(u8 port,char buf[],int len);
extern void UartInit(void);
#endif


