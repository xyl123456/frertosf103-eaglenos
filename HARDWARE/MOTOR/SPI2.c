#include "SPI2.h"
#include "sys.h"
#include "delay.h"
/*SPI -> APB2
SPI1	MISO	PB4
SPI1	MOSI	PB5
SPI1	SCK		PB3
*/

void TMC5130_MoveForward(u8 ch,int32_t step)
{
	SPI2_ReadWriteDatagram(ch,0xA1,POSITION_0);
	SPI2_ReadWriteDatagram(ch,0xAD,step);
}

void TMC5130_MoveBackward(u8 ch,int32_t step)
{
	SPI2_ReadWriteDatagram(ch,0xAD,POSITION_0);
	SPI2_ReadWriteDatagram(ch,0xA1,step);
}

void TMC5130_Setup(u8 ch)
{
	SPI2_ReadWriteDatagram(ch,0xEC,CHOP_CONF);
	 SPI2_ReadWriteDatagram(ch,0x90,CURRENT_LIMIT);
	 
	 SPI2_ReadWriteDatagram(ch,0x91,data3);
	 SPI2_ReadWriteDatagram(ch,0x80,data4);
	 SPI2_ReadWriteDatagram(ch,0x93,data5);
	 SPI2_ReadWriteDatagram(ch,0xF0,data6);
	 //****************************
	 SPI2_ReadWriteDatagram(ch,0xA4,ctrl1);
	 SPI2_ReadWriteDatagram(ch,0xA5,ctrl2);
	 SPI2_ReadWriteDatagram(ch,0xA6,ctrl3);
	 SPI2_ReadWriteDatagram(ch,0xA7,VMAX_CONF);
	 SPI2_ReadWriteDatagram(ch,0xA8,ctrl5);
	 SPI2_ReadWriteDatagram(ch,0xAA,ctrl6);
	 SPI2_ReadWriteDatagram(ch,0xAB,ctrl7);
	 SPI2_ReadWriteDatagram(ch,0xA0,RAMP_MODE_0);
}
void TMC5130_IO_Init(void)
{
	//initialize CS1 pin
	GPIO_InitTypeDef GPIO_Init_Structure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = TMC5130_CS1;	//����TMC��CS
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
  GPIO_Init(GPIOG, &GPIO_Init_Structure);					 //�����趨������ʼ��GPIOG15
	
	//initialize CS2 pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = TMC5130_CS2;	//����TMC��CS
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
  GPIO_Init(GPIOD, &GPIO_Init_Structure);					 //�����趨������ʼ��GPIOD7
	
	//initialize ENN1 pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = TMC5130_ENN1;	//����TMC��CS
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
  GPIO_Init(GPIOG, &GPIO_Init_Structure);					 //�����趨������ʼ��GPIOG12
	
	//initialize ENN2 pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = TMC5130_ENN2;	//����TMC��CS
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
  GPIO_Init(GPIOD, &GPIO_Init_Structure);					 //�����趨������ʼ��GPIOD2
	
	//DIAG0_2 DIAG1_2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_Init_Structure.GPIO_Pin = TMC5130_DIAG0_2|TMC5130_DIAG1_2;	//����TMC���ж�
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_IPU; 		 //����
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
  GPIO_Init(GPIOA, &GPIO_Init_Structure);					 //�����趨������ʼ��GPIOA
	
	//DIAG0_1 DIAG1_1
	GPIO_Init_Structure.GPIO_Pin = TMC5130_DIAG0_1|TMC5130_DIAG1_1;	//����TMC���ж�
  GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
  GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
  GPIO_Init(GPIOG, &GPIO_Init_Structure);					 //�����趨������ʼ��GPIOG
	
	TMC5130_CS1_HIGH();
	TMC5130_CS2_HIGH();
	TMC5130_ENN1_HIGH();
	TMC5130_ENN2_HIGH();
}
/**********************************************************************************
//Function:	SPI1_Init
//Description: initialize SPI1
//Return: void
**********************************************************************************/
void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PA5/6/7����

	//����SPI�Ĳ���
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//����˫��ȫ��	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //MASTERģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;     //MODE 3
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;    //MODE 3
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //�����ʷ�Ƶ64
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//MSB��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //��ʼ��SPI1�Ĵ���

	SPI_Cmd(SPI2, ENABLE);  //ʹ��SPI����
	SPI2_ReadWriteByte(0xff);//��������		 
}

/**********************************************************************************
//Function:	SPI1_SetSpeed
//Description: SPI1 speed set 
//Return: void
**********************************************************************************/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI2,ENABLE);
}

/**********************************************************************************
//Function:	SPI1_ReadWriteByte
//Description: SPI1 transefer and receive data 
//Return: unsigned char
**********************************************************************************/

u8 SPI2_ReadWriteByte(u8 TXData) //////////////////////////////  
{	
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TXData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����		     
}

int32_t SPI2_ReadWriteDatagram(u8 ch,uint8_t address ,uint32_t data)
{
	int32_t ret=0;
	
	if(ch==2){
	  TMC5130_CS2_LOW();
	}else
	  TMC5130_CS1_LOW();
	delay_us(10);
	
	SPI2_ReadWriteByte(address);
	ret |= (SPI2_ReadWriteByte(BYTE(data, 3))<<24);
	ret |= (SPI2_ReadWriteByte(BYTE(data, 2))<<16);
	ret |= (SPI2_ReadWriteByte(BYTE(data, 1))<<8);
	ret |= (SPI2_ReadWriteByte(BYTE(data, 0)));
	
	if(ch==2){
	  TMC5130_CS2_HIGH();
	}else
	  TMC5130_CS1_HIGH();
	delay_us(10);

	return ret;
}

