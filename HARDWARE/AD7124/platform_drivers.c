#include "platform_drivers.h"
#include "SPI1.h"


/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
*******************************************************************************/
unsigned char AD7124_SPI_Init()
{
  AD7124_IO_Init();
	AD7124_SPI1_Init();
	return 0;
}


unsigned char AD7124_SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber)
{
	u8 i=0;
	AD7124_CS_0
	SPI1_ReadWriteByte(data[0]);
	for(i=1; i<bytesNumber; i++)
	{
		data[i] = SPI1_ReadWriteByte(0);
	}
	AD7124_CS_1	
	return 0;
}


unsigned char AD7124_SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber)
{
	u8 i=0;
	AD7124_CS_0
  
	for(i=0; i<bytesNumber; i++)
	{
		SPI1_ReadWriteByte(data[i]);
	}
	
	AD7124_CS_1	
	return 0;
}

/**********************************************************************************
//Function:	AD7124_SPI1_Init
//Description: initialize SPI1 mode3
//Return: void
**********************************************************************************/
void AD7124_SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTB时钟使能 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI2时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);  //PA5/6/7上拉

	//配置SPI的参数
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//两线双向全工	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;     //MODE 3
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;    //MODE 3
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	SPI1_ReadWriteByte(0xff);//启动传输		 
}


///////////////////////////////////////////////////////////////////////////////////////
/* 定义GPIO端口 */
#define RCC_SCK 	RCC_APB2Periph_GPIOA
#define PORT_SCK	GPIOA
#define PIN_SCK		GPIO_Pin_5

#define RCC_DIN 	RCC_AHB1Periph_GPIOA
#define PORT_DIN	GPIOA
#define PIN_DIN		GPIO_Pin_7

#define RCC_CS 		RCC_AHB1Periph_GPIOG
#define PORT_CS		GPIOG
#define PIN_CS		GPIO_Pin_8

#define RCC_DOUT 	RCC_AHB1Periph_GPIOA
#define PORT_DOUT	GPIOA
#define PIN_DOUT	GPIO_Pin_6

#define RCC_SYNC 	RCC_AHB1Periph_GPIOG
#define PORT_SYNC	GPIOG
#define PIN_SYNC	GPIO_Pin_7


#define CS_0()		GPIO_ResetBits(PORT_CS, PIN_CS)
#define CS_1()		GPIO_SetBits(PORT_CS, PIN_CS)

#define SCK_0()		GPIO_ResetBits(PORT_SCK, PIN_SCK)
#define SCK_1()		GPIO_SetBits(PORT_SCK, PIN_SCK)

#define DI_0()		GPIO_ResetBits(PORT_DIN, PIN_DIN)
#define DI_1()		GPIO_SetBits(PORT_DIN, PIN_DIN)

#define DO_IS_HIGH()	(GPIO_ReadInputDataBit(PORT_DOUT, PIN_DOUT) == Bit_SET)
#define SYNC_0	GPIO_ResetBits(PORT_SYNC, PIN_SYNC)
#define SYNC_1 	GPIO_SetBits(PORT_SYNC, PIN_SYNC)


//********************************************************
 void AD7124_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_Init_Structure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = PIN_SYNC | PIN_CS;	//配置DAC的CS/LDAC/CLR
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为2MHz
  GPIO_Init(GPIOG, &GPIO_Init_Structure);					 //根据设定参数初始化
	
	CS_1();
	SYNC_1;
}
