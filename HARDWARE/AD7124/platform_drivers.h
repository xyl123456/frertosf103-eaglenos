#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#define AD7124_CS_0 GPIO_ResetBits(GPIOG,GPIO_Pin_8);
#define AD7124_CS_1 GPIO_SetBits(GPIOG,GPIO_Pin_8);

void AD7124_SPI1_Init(void);
void AD7124_IO_Init(void);

/*! Initializes the SPI communication peripheral. */
unsigned char AD7124_SPI_Init(void);

/*! Reads data from SPI. */
unsigned char AD7124_SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber);

/*! Writes data to SPI. */
unsigned char AD7124_SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber);

#endif /* _COMMUNICATION_H_ */
