#ifndef _SPI_H_
#define _SPI_H_
#include "sys.h"

void SPI1_Init(void);
void SPI1_SetSpeed(unsigned char SetSpeed);
unsigned char SPI1_ReadWriteByte(unsigned char TXData);


#endif
