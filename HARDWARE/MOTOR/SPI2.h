#ifndef _SPI_H_
#define _SPI_H_
#include "sys.h"

#define BYTE(value, n)    (((value) >> ((n) * 8)) & 0xFF)
 
#define CHOP_CONF  0x000100C3 //0xEC,CHOPCONF
#define CURRENT_LIMIT  0x00600300 //0x90,IHOLD 0-4 0mA, IRUN 8-12  240m 
#define data3  0x0000000A  //0x91,powerdown delay
#define data4  0x00000004  //0x80,enable pwm mode
#define data5  0x00000064  //0x93,064=100,max velocity about 3500=ca.30RPM
#define data6  0x000401C8  //0xF0,
  
#define ctrl1  0x000003E8  //0xA4,0x064  A1=100 first acc
#define ctrl2  0x0000C350  //0xA5,0xa388  acc threshold V1
#define ctrl3  0x000001F4  //0xA6
#define VMAX_CONF  0x00020D40  //0xA7
#define ctrl5  0x000002BC  //0xA8
#define ctrl6  0x00000578  //0xAA
#define ctrl7  0x0000000A  //0xAB 
#define RAMP_MODE_0  0x00000000  //0xA0,RAMP mode 0	 
#define POSITION_0  0x00000000  //0xAD,
#define POSITION_P1  0x0000C800  //0xAD,200*256, 200*0.0005mm =1mm
#define POSITION_N1  0xFFFf3800  //0xAD,TO ZERO
#define READ_POSITION  0x00000000  //0x21
#define SET_HOME  0x00000000   //0xA1,write home

//Pins
#define TMC5130_DIAG0_1			GPIO_Pin_9   //PG9
#define TMC5130_DIAG1_1			GPIO_Pin_11  //PG11

#define TMC5130_DIAG0_2			GPIO_Pin_11  //PA11
#define TMC5130_DIAG1_2			GPIO_Pin_12  //PA12

#define TMC5130_ENN1        GPIO_Pin_12  //PG12
#define TMC5130_ENN1_LOW()  GPIO_ResetBits(GPIOG, TMC5130_ENN1)
#define TMC5130_ENN1_HIGH() GPIO_SetBits(GPIOG, TMC5130_ENN1)

#define TMC5130_ENN2        GPIO_Pin_2   //PD2
#define TMC5130_ENN2_LOW()  GPIO_ResetBits(GPIOD, TMC5130_ENN2)
#define TMC5130_ENN2_HIGH() GPIO_SetBits(GPIOD, TMC5130_ENN2)

#define TMC5130_CS2			    GPIO_Pin_7  //PD7
#define TMC5130_CS2_LOW()		GPIO_ResetBits(GPIOD, TMC5130_CS2)
#define TMC5130_CS2_HIGH()	GPIO_SetBits(GPIOD, TMC5130_CS2)

#define TMC5130_CS1			    GPIO_Pin_15  //PG15
#define TMC5130_CS1_LOW()		GPIO_ResetBits(GPIOG, TMC5130_CS1)
#define TMC5130_CS1_HIGH()	GPIO_SetBits(GPIOG, TMC5130_CS1)

void SPI2_Init(void);
void SPI2_SetSpeed(unsigned char SetSpeed);

void TMC5130_IO_Init(void);
void TMC5130_Setup(u8 ch);

void TMC5130_MoveForward(u8 ch,int32_t step);
void TMC5130_MoveBackward(u8 ch,int32_t step);

unsigned char SPI2_ReadWriteByte(unsigned char TXData);
int32_t SPI2_ReadWriteDatagram(u8 ch,uint8_t address,uint32_t data);
#endif
