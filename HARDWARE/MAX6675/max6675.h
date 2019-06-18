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
  
extern void Max6675Init(void);			 //初始化max6675模块 
u16 Max6675_readRawValue(uint8_t channel);/*按位读取电平的原始数据函数*/
extern float Max6675_readTemperature(uint8_t channel);/*读取温度函数*/
 				    
#endif

