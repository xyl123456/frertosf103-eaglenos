#ifndef __ANALOGSWITCH_H
#define __ANALOGSWITCH_H
#include "sys.h"


//address
#define SW_IMP_SEL0			GPIO_Pin_9
#define SW_IMP_SEL0_LOW()		GPIO_ResetBits(GPIOC, SW_IMP_SEL0)
#define SW_IMP_SEL0_HIGH()	GPIO_SetBits(GPIOC, SW_IMP_SEL0)
#define SW_IMP_SEL1			GPIO_Pin_8
#define SW_IMP_SEL1_LOW()	GPIO_ResetBits(GPIOA, SW_IMP_SEL1)
#define SW_IMP_SEL1_HIGH()	GPIO_SetBits(GPIOA, SW_IMP_SEL1)

//channel
#define SW_P1      0xff40
#define SW_A1      0x00c8
#define SW_A2      0x00d0
#define SW_A3      0x00e0
#define SW_A4      0x00c4
#define SW_A5      0x00c2
#define SW_SC      0x0000

#define SW_R1      0x00
#define SW_R2      0x01

void SW_Impedance1_config(u8 R);
void SW_TEST_config(u16 REG);

void SW_GPIO_Init(void);
void SW_IIC_Write(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit);
void SW_IIC_Read(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit);

//void setSwitch();

#endif
