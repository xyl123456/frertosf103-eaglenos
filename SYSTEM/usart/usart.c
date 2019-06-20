#include "sys.h"
#include "usart.h"	 
#include "string.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��	
#include "task.h"
#include "semphr.h"	 
#endif
  
 
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
}

void _ttywrch(int ch)
{
    ch = ch;
}

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
uint8_t  uart1StartFlag=0;
uint8_t  uart2StartFlag=0;

void UartInit(){
	#if EN_USART1_RX   //���ʹ���˽���
	Uart1Init(115200);
	#endif
	#if EN_USART2_RX   //���ʹ���˽���
	Uart2Init(115200);
	#endif
	#if EN_USART4_RX   //���ʹ���˽���
	Uart4Init(115200);
	#endif
}

//���ڷ��ͺ���
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


extern TaskHandle_t UartTask_Handler;	//��������֪ͨ��������

 
#if EN_USART1_RX   //���ʹ���˽���

char uart1RxBuf[USART1_REC_LEN];     //���ջ���,���USART1_REC_LEN���ֽ�.
u16 uart1RxSta=0;       //����״̬���	  
u16 uart1ReciveSufixCount;//��׺"}"
u16 uart1RecivePrefixCount;//ǰ׺"{"

void Uart1Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8 ;//��ռ���ȼ�8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}



void USART1_IRQHandler(void)                	//����1�жϷ������
{
	char resData;
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
		{
			resData =USART_ReceiveData(USART1);	//��ȡ���յ�������
			
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
					//�������"{}"���½���,���������ݳ���128��
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
	//��������֪ͨ
	if((uart1RxSta&0x8000)&&(UartTask_Handler!=NULL))//���յ����ݣ����ҽ�������֪ͨ��������Ч
	{
		vTaskNotifyGiveFromISR(UartTask_Handler,&xHigherPriorityTaskWoken);//��������֪ͨ
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}	 
} 
#endif	

#if EN_USART2_RX   //���ʹ���˽���
 	
char uart2RxBuf[USART2_REC_LEN];     //���ջ���,���USART2_REC_LEN���ֽ�

u16 uart2RxSta=0;       //����״̬���
u16 uart2ReciveSufixCount;//��׺
u16 uart2RecivePrefixCount;//ǰ׺

void Uart2Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8 ;//��ռ���ȼ�8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//����IDLE�ж�,����һ����֡
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
}
//extern TaskHandle_t UART2Task_Handler;	//��������֪ͨ��������

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	char resData;
	BaseType_t xHigherPriorityTaskWoken;
	USART_ClearFlag(USART2,USART_FLAG_TC);
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
		{
//		resData =USART_ReceiveData(USART2);	//��ȡ���յ�������
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
//				//�������"{}"���½���
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
	//��������֪ͨ
	if((uart2RxSta&0x8000)&&(UartTask_Handler!=NULL))//���յ����ݣ����ҽ�������֪ͨ��������Ч
	{
		vTaskNotifyGiveFromISR(UartTask_Handler,&xHigherPriorityTaskWoken);//��������֪ͨ
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}		
} 
#endif



#if EN_USART4_RX   //���ʹ���˽���
//����4�жϷ������

char uart4RxBuf[USART2_REC_LEN];     //���ջ���,���USART4_REC_LEN���ֽ�.

u16 uart4RxSta=0;       //����״̬���
u16 uart4ReciveSufixCount;//��׺"}"
u16 uart4RecivePrefixCount;//ǰ׺"{"
  
void Uart4Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//ʹ��USART4��GPIOCʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//USART4_TX   GPIOC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.10
   
	//USART4_RX	  GPIOC.11��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.11 

	//Usart4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8 ;//��ռ���ȼ�8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART4, &USART_InitStructure); //��ʼ������4
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4
}

//extern TaskHandle_t UART4Task_Handler;	//��������֪ͨ��������

void UART4_IRQHandler(void)                	//����4�жϷ������
{
	char resData;
	BaseType_t xHigherPriorityTaskWoken;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		resData =USART_ReceiveData(UART4);	//��ȡ���յ�������
		 
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
			//�������"{}"���½���,���߳���һ��������������
			if((uart4RecivePrefixCount>20)||(uart4ReciveSufixCount>20)||((uart4RxSta&0X3FFF)>128)){
				uart4RxSta=0;
				uart4RecivePrefixCount=0;
				uart4ReciveSufixCount=0;
			}
     }
	 }
	//��������֪ͨ
	if((uart4RxSta&0x8000)&&(UartTask_Handler!=NULL))//���յ����ݣ����ҽ�������֪ͨ��������Ч
	{
		vTaskNotifyGiveFromISR(UartTask_Handler,&xHigherPriorityTaskWoken);//��������֪ͨ
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}		
} 
#endif



