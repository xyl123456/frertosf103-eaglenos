#include "AD5933_I2C.h"
#include "delay.h"

/**********************************************************************************
//Function:	AD5933_IIC_Init
//Description: initialize IIC port to ADS1256
//Return: void
**********************************************************************************/
void AD5933_IIC_Init(void)
{			
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    SCL_H;
    SDA_H;
}

/**********************************************************************************
//Function:	AD5933_IIC_Start
//Description: generate IIC start action
//Return: void
**********************************************************************************/
void AD5933_IIC_Start(void)
{
    //SDA_OUT();      //set SDA as output mode
    SCL_H;
    SDA_H;
    delay_us(4);
    SDA_L;
    delay_us(4);
    SDA_L;      //START: SDA changde into LOW when SCK is HIGH 
    delay_us(4);
    SCL_L;      //hold IIC data, ready for start
}

/**********************************************************************************
//Function:	AD5933_IIC_Stop
//Description: generate IIC stop action
//Return: void
**********************************************************************************/
void AD5933_IIC_Stop(void)
{
	//SDA_OUT();      //set SDA as output mode
	SCL_L;
	SDA_L;      
 	delay_us(4);
	SCL_H; 
	SDA_H;      //STOP: SDA changde into HIGH when SCK is high 
	delay_us(4);							   	
}

/**********************************************************************************
//Function:	AD5933_IIC_Wait_Ack
//Description: wait for ACK
//Return: 
//  1: sucess in ACK receive
//  0: fail in ACK receive
**********************************************************************************/
unsigned char AD5933_IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	
	//SDA_IN();      //set SDA as input mode	
	SDA_H;
	delay_us(4);	   
	SCL_H;
	delay_us(4);	
	
	while(SDA_read)
	{
		#if 1
		ucErrTime++;
		if(ucErrTime>250)
		{
			AD5933_IIC_Stop();
			return 1;
		}
		#endif
	}
	
	SCL_L;	
	return 0;  
} 

/**********************************************************************************
//Function:	AAD5933_IIC_Ack
//Description: generate IIC ACK action
//Return: void
**********************************************************************************/
void AD5933_IIC_Ack(void)
{
	SCL_L;
	//SDA_OUT();
	SDA_L;
	delay_us(4);
	SCL_H;
	delay_us(4);
	SCL_L;
}

/**********************************************************************************
//Function:	AD5933_IIC_NAck
//Description: generate IIC NACK action
//Return: void
**********************************************************************************/
void AD5933_IIC_NAck(void)
{
	SCL_L;
	//SDA_OUT();
	SDA_H;
	delay_us(4);
	SCL_H;
	delay_us(4);
	SCL_L;
}

/**********************************************************************************
//Function:	AD5933_IIC_Send_Byte
//Description: send one byte
//Return: void
**********************************************************************************/
void AD5933_IIC_Send_Byte(unsigned char TX_Byte)
{                        
    unsigned char i = 0;
    //SDA_OUT();              //set SDA as output mode	    
    SCL_L;
    
    for(i=0; i<8; i++)
    {              
        if((TX_Byte & 0x80) >> 7)
					SDA_H;
				else
					SDA_L;
        TX_Byte <<= 1; 	  
        delay_us(10);  
        SCL_H;
        delay_us(10); 
        SCL_L;	
        delay_us(10);
    }	 
}

/**********************************************************************************
//Function:	AD5933_IIC_Write
//Description: send one byte
//Return: void
**********************************************************************************/
void AD5933_IIC_Write(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit)
{
	unsigned char i;

	//generate start condition
	AD5933_IIC_Start();
	
	//write slave address
	AD5933_IIC_Send_Byte(slaveAddress << 1);	

	//wait for ACK
	AD5933_IIC_Wait_Ack();

	//write date
	for(i = 0; i < bytesNumber; i++)
	{
		AD5933_IIC_Send_Byte(dataBuffer[i]);
		//wait for ACK
		AD5933_IIC_Wait_Ack();
	}

	//wait for ACK
	//AD5933_IIC_Wait_Ack();

	//generate stop condition
	if(stopBit)
	{
		AD5933_IIC_Stop();
	}

	delay_ms(10);
}

/**********************************************************************************
//Function:	AD5933_IIC_Send_Byte
//Description: read one byte
//Return: unsigned char
**********************************************************************************/
unsigned char AD5933_IIC_Read_Byte(unsigned char Ack_Signal)
{
	unsigned char i,receive = 0;
	
	//SDA_IN();                  //set SDA as input mode
	
    for(i=0; i<8; i++ )
	{
        SCL_L; 
        delay_us(10);
		    SCL_H;
        receive <<= 1;
        if(SDA_read)
        {
            receive++;   
        }
		delay_us(10); 
    }					 
    
    if(!Ack_Signal)
        AD5933_IIC_NAck();     //send NACK signal
    else
        AD5933_IIC_Ack();      //send ACK signal  
        
    return receive;
}


/**********************************************************************************
//Function:	AD5933_IIC_Read
//Description: send one byte
//Return: void
**********************************************************************************/
void AD5933_IIC_Read(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit)
{
	unsigned char i;

	//generate start condition
	AD5933_IIC_Start();
	
	//write slave address
	AD5933_IIC_Send_Byte((slaveAddress << 1) + 1);	
	
	//wait for ACK
	AD5933_IIC_Wait_Ack();

	//receive data
	for(i = 0; i < bytesNumber; i++)
	{
		dataBuffer[i] = AD5933_IIC_Read_Byte(0);
	}

	//generate stop condition
	if(stopBit)
	{
		AD5933_IIC_Stop();
	}
}


