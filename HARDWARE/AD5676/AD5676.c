#include "AD5676.h"
#include "SPI1.h"
#include "sys.h"
#include "delay.h"

/**********************************************************************************
//Function: AD5065_GPIO_Init
//Description: initialize AD5767 GPIO
//Return: void
**********************************************************************************/
void AD5676_GPIO_Init(void)
{
	//initialize CS pin
	GPIO_InitTypeDef GPIO_Init_Structure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = AD5676_CS | AD5676_LDAC | AD5676_CLR;	//配置DAC的CS/LDAC/CLR
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
  GPIO_Init(GPIOC, &GPIO_Init_Structure);					 //根据设定参数初始化GPIOB.5
}

/**********************************************************************************
//Function: AD5676_Init
//Description: initialize DAC AD5676
//Return: void
**********************************************************************************/
void AD5676_Init(void)
{
	AD5676_GPIO_Init();
	SPI1_Init();
	AD5676_LDAC_HIGH();
	AD5676_CLR_HIGH();
	AD5676_Reset();
}

/**********************************************************************************
//Function: AD5065_GPIO_Init
//Description: update and write data to AD5767 
//Return: void
**********************************************************************************/
void AD5676_WriteReg(unsigned long command, unsigned long address, unsigned int value)
{
	unsigned long DAC_command = 0;
	unsigned long regValue = 0;

	DAC_command = (command << 20) | (address << 16);
	regValue = (0xffff & value);
	regValue |= DAC_command;
  AD5676_CS_LOW();
    delay_us(100);
    
	//SPI1_ReadWriteByte(regValue >> 24);
	SPI1_ReadWriteByte(0xff&(regValue >> 16));
	SPI1_ReadWriteByte(0xff&(regValue >> 8));
	SPI1_ReadWriteByte(0xff&regValue);
 // AD5676_CLR_LOW();
//	AD5676_CLR_HIGH();
  delay_us(10);
	AD5676_CS_HIGH();
}

/**********************************************************************************
//Function: AD5676_Reset
//Description: Reset DAC AD5676
//Return: void
**********************************************************************************/
void AD5676_Reset(void)
{
	delay_us(10);
	AD5676_CLR_LOW();
	delay_us(10);
	AD5676_CLR_HIGH();
}
/**********************************************************************************
//Function: AD5065_WriteToInputReg
//Description: update and write data to AD5767 
//Return: void
**********************************************************************************/
void AD5676_WriteToInputReg(unsigned long Address, unsigned long value)
{
	AD5676_WriteReg(DAC_CMD_WriteReg, Address, value);
}

/**********************************************************************************
//Function: AD5065_UpdateDAC
//Description: update and write data to AD5767 
//Return: void
**********************************************************************************/
void AD5676_UpdateDAC(unsigned long Address)
{
	AD5676_WriteReg(DAC_CMD_LDAC, Address, 0);
	
}

/**********************************************************************************
//Function: AD5676_WriteToInputRegDAC
//Description: update and write data to AD5676
//Return: void
**********************************************************************************/
void AD5676_WriteToInputRegDAC(unsigned long Address, unsigned long value)
{
	AD5676_WriteReg(DAC_CMD_WrUp_DAC, Address, value);
}

/**********************************************************************************
//Function: AD5676_PowerDown_ALL
//Description: 
//Return: void
**********************************************************************************/
void AD5676_PowerDown_ALL(void)
{
	unsigned long regValue = DAC_CMD_PowerDown_All;

	AD5676_CS_LOW();
  delay_us(100);
    
	SPI1_ReadWriteByte(regValue >> 16);
	SPI1_ReadWriteByte(regValue >> 8);
	SPI1_ReadWriteByte(regValue);

    delay_us(100);
	AD5676_CS_HIGH();
}

/**********************************************************************************
//Function: AD5676_PowerUp_ALL
//Description: 
//Return: void
**********************************************************************************/
void AD5676_PowerUp_ALL(void)
{
	unsigned long regValue = DAC_CMD_PowerUp_All;

	AD5676_CS_LOW();
  delay_us(100);
    
	SPI1_ReadWriteByte(regValue >> 16);
	SPI1_ReadWriteByte(regValue >> 8);
	SPI1_ReadWriteByte(regValue);

    delay_us(100);
	AD5676_CS_HIGH();
}
