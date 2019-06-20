#ifndef __AD5933IIC_H
#define __AD5933IIC_H

#include "sys.h" 
   	   		   
//IO
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//set PE7 as input
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //set PE7 as output

#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6

#define SDA_H         GPIOB->BSRR = GPIO_Pin_7  
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7 

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6  
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7  


void AD5933_IIC_Init(void);                                     //initilize IO			 
void AD5933_IIC_Start(void);                                   //generate start action
void AD5933_IIC_Stop(void);                                   //generate stop action
void AD5933_IIC_Ack(void);                                    //send ACK signal
void AD5933_IIC_NAck(void);	                               //send NACK signal
unsigned char AD5933_IIC_Wait_Ack(void);                     //wait for ACK
void AD5933_IIC_Send_Byte(u8 txd);                          //send one byte
unsigned char  AD5933_IIC_Read_Byte(u8 txd);
void AD5933_IIC_Write(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit);
void AD5933_IIC_Read(unsigned char slaveAddress, unsigned char* dataBuffer, unsigned char bytesNumber, unsigned char stopBit);
#endif
















