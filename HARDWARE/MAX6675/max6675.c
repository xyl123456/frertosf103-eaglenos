#include "max6675.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"	

/*max6675��ʼ������*/

/*ʹ������оƬ�������ֿ�д�������Ժ���*/
/*Ƭѡ��һ����CS1----PB8,cs2---PB9*/
/*����--����PB4*/
/*ʱ��--���PB3*/
void Max6675Init(void){	
  GPIO_InitTypeDef GPIO_InitStructure;
	
	//DBGMCU->CR = DBGMCU->CR & ~((uint32_t)1<<5);
  //RCC->APB2ENR = 1 |  (1<<3);  /*AFIO/ GPIOB*/
  //AFIO->MAPR = (AFIO->MAPR & ~((uint32_t)0x7 << 24)) | (2 << 24); /*�ͷ� PA15 PB3 PB4*/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ��
	//CS1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB8�������  GPIO_Mode_AF_PP ƬѡCS1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB8
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	//CS2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB9�������  GPIO_Mode_AF_PP ƬѡCS2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB8
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	//SO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;//PB4�������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB4
	
	//CLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB11�����������  GPIO_Mode_AF_PP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB3	
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	
}

u16 Max6675_readRawValue(uint8_t channel)
{
  u16 tmp=0;
  u16 i;
  if(channel==1)
  {
	MAX6675_CS_2_H;
	MAX6675_CS_L;//cs��
  }
	if(channel==2){
	MAX6675_CS_H;
	MAX6675_CS_2_L;
	}
  /*��λ��ȡ��ƽ*/
  for(i=0;i<16;i++)
    {  
	/*����һ���أ����ض�ȡPB4�Ķ˿ڵ�ƽ*/
		MAX6675_CK_H;
		delay_us(10);
		if(MAX6675_SO)tmp|=0x0001;
		//printf("tmp%d---:%d\n",i,tmp);/*����Ϊ�˼�����ʱ��λ��ӡÿ����*/
		tmp<<=1;//��λ����
		MAX6675_CK_L;
		delay_us(10);
		}
	if(channel==1){
		MAX6675_CS_H;//cs��	
		}
	if(channel==2){
		MAX6675_CS_2_H;
	}  
   if (tmp&0X04) 
 {
    tmp = 0; //δ��⵽�ȵ�ż
	// printf("δ��⵽�ȵ�ż\n");//������
  }
  else 
 {
	tmp<<=1;//ȥ��D15λ
	tmp>>=4;//ȥ��D0��1��2λ
  }
	return tmp;
}
 
float Max6675_readTemperature(uint8_t channel)
{
	u16 d;
	float i,S;
	S=2.142857;//ʵ����������ˮ����ˮ ������õ�һ��ʵ�ʲ���
	delay_ms(300);
	d=Max6675_readRawValue(channel);
	i=d*1023.75/4095/S;
    return i;
}

