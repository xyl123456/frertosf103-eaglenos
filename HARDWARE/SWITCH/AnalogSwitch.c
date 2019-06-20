#include "AnalogSwitch.h"
#include "AD5933_I2C.h"
#include "delay.h"


/**********************************************************************************
//Function:	SW_Impedance1_config
//Description: 
//Return: void
**********************************************************************************/
void SW_Impedance1_config(u8 R){
	SW_IMP_SEL0_HIGH();
	if (R)
	  SW_IMP_SEL1_HIGH();
	else
		SW_IMP_SEL1_LOW();
	delay_us(20);		
	SW_IIC_Write(0x20,0x00,1,1);
	delay_us(20);
	SW_IIC_Write(0x21,0x00,1,1);
	delay_us(20);
}

/**********************************************************************************
//Function:	SW_TEST_config
//Description: 
//Return: void
**********************************************************************************/
void SW_TEST_config(u16 REG){
	unsigned char reg = 0xff&(REG>>8);
	SW_IMP_SEL0_LOW();
	SW_IMP_SEL1_LOW();
	SW_IIC_Write(0x20,&reg,1,1);
	delay_us(20);
	reg = 0xff&REG;
	SW_IIC_Write(0x21,&reg,1,1);
	delay_us(20);
}
/**********************************************************************************
//Function:	SW_IIC_Write
//Description: send one byte
//Return: void
**********************************************************************************/
void SW_IIC_Write(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit)
{
	AD5933_IIC_Write(slaveAddress, dataBuffer, bytesNumber,stopBit);
}

/**********************************************************************************
//Function:	SW_IIC_Read
//Description: send one byte
//Return: void
**********************************************************************************/
void SW_IIC_Read(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit)
{
	AD5933_IIC_Read(slaveAddress,dataBuffer, bytesNumber, stopBit);
}

/**********************************************************************************
//Function: AD5065_GPIO_Init
//Description: initialize AD5767 GPIO
//Return: void
**********************************************************************************/
void SW_GPIO_Init(void)
{
	//initialize IMP_sel0 pin C9
	GPIO_InitTypeDef GPIO_Init_Structure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &GPIO_Init_Structure); 
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
	
	//initialize IMP_sel1 pin A8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA, &GPIO_Init_Structure);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

