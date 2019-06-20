#include "SPI1.h"
#include "sys.h"

/*SPI -> APB2
SPI1	MISO	PB4
SPI1	MOSI	PB5
SPI1	SCK		PB3
*/

/**********************************************************************************
//Function:	SPI1_Init
//Description: initialize SPI1
//Return: void
**********************************************************************************/
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI2ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);  //PA5/6/7����

	//����SPI�Ĳ���
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//����˫��ȫ��	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //MASTERģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;     //MODE 1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;    //MODE 1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //�����ʷ�Ƶ64
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//MSB��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //��ʼ��SPI1�Ĵ���

	SPI_Cmd(SPI1, ENABLE);  //ʹ��SPI����
	SPI1_ReadWriteByte(0xff);//��������		 
}

/**********************************************************************************
//Function:	SPI1_SetSpeed
//Description: SPI1 speed set 
//Return: void
**********************************************************************************/
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE);
}

/**********************************************************************************
//Function:	SPI1_ReadWriteByte
//Description: SPI1 transefer and receive data 
//Return: unsigned char
**********************************************************************************/

u8 SPI1_ReadWriteByte(u8 TXData) //////////////////////////////  
{	
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TXData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;

			if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����		     
}

