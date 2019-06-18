#ifndef __MAX6675_H
#define __MAX6675_H
#include "stm32f10x.h"


#define MAX6675_CS1_CLK   RCC_APB2Periph_GPIOB
#define MAX6675_CS1_PORT  GPIOB
#define MAX6675_CS1_PIN   GPIO_Pin_8

#define MAX6675_CS2_PIN   GPIO_Pin_9
 
void max6675_init(void);			 //初始化max6675模块 
uint8_t max6675_readWriteByte(uint8_t txData); //max6675模块读写一个字节
uint16_t max6675_readRawValue(void);

#endif
