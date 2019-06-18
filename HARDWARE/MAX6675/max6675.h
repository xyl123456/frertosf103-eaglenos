#ifndef __MAX6675_H
#define __MAX6675_H	 

#include "stm32f10x.h"
#include "sys.h" 

#define MAX6675_CS_H   GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define MAX6675_CS_L   GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define MAX6675_CS_2_H	GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define MAX6675_CS_2_L	GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define MAX6675_CK_H   GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define MAX6675_CK_L   GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define MAX6675_SO  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
  
extern void Max6675Init(void);			 //��ʼ��max6675ģ�� 
u16 Max6675_readRawValue(uint8_t channel);/*��λ��ȡ��ƽ��ԭʼ���ݺ���*/
extern float Max6675_readTemperature(uint8_t channel);/*��ȡ�¶Ⱥ���*/
 				    
#endif

