#ifndef _AD5676_H_
#define _AD5676_H_

#include "sys.h"

//pin
#define AD5676_CS			GPIO_Pin_6
#define AD5676_CS_LOW()		GPIO_ResetBits(GPIOC, AD5676_CS)
#define AD5676_CS_HIGH()	GPIO_SetBits(GPIOC, AD5676_CS)
#define AD5676_LDAC			GPIO_Pin_7
#define AD5676_LDAC_LOW()	GPIO_ResetBits(GPIOC, AD5676_LDAC)
#define AD5676_LDAC_HIGH()	GPIO_SetBits(GPIOC, AD5676_LDAC)
#define AD5676_CLR			GPIO_Pin_8 
#define AD5676_CLR_LOW()	GPIO_ResetBits(GPIOC, AD5676_CLR)
#define AD5676_CLR_HIGH()	GPIO_SetBits(GPIOC, AD5676_CLR)

//address
#define DAC_0				0x0		 //
#define DAC_1				0x1		
#define DAC_2			  0x2		
#define DAC_3				0x3		
#define DAC_4				0x4		
#define DAC_5			  0x5	
#define DAC_6			  0x6	
#define DAC_7			  0x7	

//command
#define DAC_CMD_WriteReg	        0x1		//write to input register
#define DAC_CMD_UpdateReg	        0x2		//update DAC register
#define DAC_CMD_WrUp_DAC	        0x3		//write to and update DAC channel
#define DAC_CMD_PowerDown	        0x4		//power down/power up DAC
#define DAC_CMD_PowerDown_All     0x405555   //power down all DAC, 1k to GND
#define DAC_CMD_PowerUp_All       0x400000   //power up all DAC
#define DAC_CMD_LDAC			        0x5		//Hardware LDAC mask register 
#define DAC_CMD_RESET	            0x6		//software reset
#define DAC_CMD_SetGain    	    	0x7		//Gain setup register LFCSP package only
#define DAC_CMD_Setup_DCEN	      0x8		//set up DCEN register
#define DAC_CMD_Setup_ReadBackReg	0x9   //Set up the readback register readback enable
#define DAC_CMD_UpdateAllReg    	0x10  //Update all channels of the input registe
#define DAC_CMD_WrUp_ALLDAC       0x11  //Update all channels of the DAC register and input register


//FOR POWER DOWN
#define NORMAL				0x00 
#define K1Ohm				0x10 
#define K100Ohm				0x20 
#define ThreeState			0x30

void AD5676_Init(void);
void AD5676_GPIO_Init(void);
void AD5676_Reset(void);
void AD5676_WriteReg(unsigned long command, unsigned long address, unsigned int value);
void AD5676_PowerDown_ALL(void);
void AD5676_PowerUp_ALL(void);
void AD5676_WriteToInputRegDAC(unsigned long Address, unsigned long value);
void AD5676_UpdateDAC(unsigned long Address);
void AD5676_WriteToInputReg(unsigned long Address, unsigned long value);

#endif
