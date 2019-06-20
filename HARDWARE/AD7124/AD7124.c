#include "AD7124.h"
#include "platform_drivers.h"
#include "SPI1.h"
#include "delay.h"
#include "usart.h"

/* Error codes */
#define INVALID_VAL 1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

#define AD7124_SPS_50  0x00005a  //
#define AD7124_SPS_30  0x000080  //

/*! Array holding the info for the ad7124 registers - address, initial value,
    size and access type. */
ad7124_st_reg ad7124_regs[] =
{
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	{0x01, 0x0480, 2, 1}, /* AD7124_ADC_Control */ //全功率 连续转换 单次读取 STATUS
	{0x02, 0x00000000, 4, 2}, /* AD7124_Data */
	{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,   1, 2}, /* AD7124_ID */
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	{0x07, 0x0040, 3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
	{0x09, 0x018E, 2, 1}, /* AD7124_Channel_0 */  //VP1 iMg
	{0x0A, 0x016E, 2, 1}, /* AD7124_Channel_1 */  //VP2 iCa
	{0x0B, 0x014E, 2, 1}, /* AD7124_Channel_2 */  //VP3 K
	{0x0C, 0x012E, 2, 1}, /* AD7124_Channel_3 */  //VP4 Na
	{0x0D, 0x010E, 2, 1}, /* AD7124_Channel_4 */  //VP5 pH
	{0x0E, 0x00EE, 2, 1}, /* AD7124_Channel_5 */  //VP6 Cl
	{0x0F, 0x00AE, 2, 1}, /* AD7124_Channel_6 */  //VP7 CO2
	{0x10, 0x00CE, 2, 1}, /* AD7124_Channel_7 */  //VP8 CO2
	{0x11, 0x008E, 2, 1}, /* AD7124_Channel_8 */  //VA1
	{0x12, 0x006E, 2, 1}, /* AD7124_Channel_9 */  //VA2
	{0x13, 0x004E, 2, 1}, /* AD7124_Channel_10 */ //VA3 glu
	{0x14, 0x002E, 2, 1}, /* AD7124_Channel_11 */ //VA4 pO2
	{0x15, 0x000E, 2, 1}, /* AD7124_Channel_12 */ //VA5 Lac
	{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */ 
	{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
	{0x19, 0x09E0, 2, 1}, /* AD7124_Config_0 */
	{0x1A, 0x0870, 2, 1}, /* AD7124_Config_1 */
	{0x1B, 0x0870, 2, 1}, /* AD7124_Config_2 */
	{0x1C, 0x0870, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0870, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0870, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0870, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0870, 2, 1}, /* AD7124_Config_7 */
	{0x21, AD7124_SPS_30, 3, 1}, /* AD7124_Filter_0 */  //sinc4 无后置  30sps
	{0x22, 0x060180, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */	
  {0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};

ad7124_st_reg ad7124_regs_calibration[] =
{
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	{0x01, 0x0094, 2, 1}, /* AD7124_ADC_Control */  //全功率校准
};

/***************************************************************************//**
* @brief Reads the value of the specified register without checking if the
*        device is ready to accept user requests.
*
* @param device - The handler of the instance of the driver.
* @param pReg - Pointer to the register structure holding info about the
*               register to be read. The read value is stored inside the
*               register structure.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
 int32_t AD7124_NoCheckReadRegister(ad7124_device *device, ad7124_st_reg* pReg)
{
	int32_t ret       = 0;
	uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i         = 0;
	uint8_t check8    = 0;
	uint8_t msgBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	if(!device || !pReg)
		return INVALID_VAL;

	/* Build the Command word */
	buffer[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD |
	  		AD7124_COMM_REG_RA(pReg->addr);

	/* Read data from the device */
	ret = AD7124_SPI_Read(device->slave_select_id,
			buffer,
			((device->useCRC != AD7124_DISABLE_CRC) ? pReg->size + 1
							: pReg->size) + 1+1);
	if(ret < 0)
		return ret;


	/* Check the CRC */
	if(device->useCRC == AD7124_USE_CRC)
	{
		msgBuf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | 
		  		AD7124_COMM_REG_RA(pReg->addr);
		for(i = 1; i < pReg->size + 2; ++i)
		{
			msgBuf[i] = buffer[i];
		}
		check8 = AD7124_ComputeCRC8(msgBuf, pReg->size + 2);
	}

	if(check8 != 0)
	{
		/* ReadRegister checksum failed. */
		return COMM_ERR;
	}

	/* Build the result */
	pReg->value = 0;
	for(i = 1; i < pReg->size + 1; i++)
	{
		pReg->value <<= 8;
		pReg->value += buffer[i];
	}

	return ret;
}

/***************************************************************************//**
* @brief Writes the value of the specified register without checking if the
*        device is ready to accept user requests.
*
* @param device - The handler of the instance of the driver.
* @param reg - Register structure holding info about the register to be written
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_NoCheckWriteRegister(ad7124_device *device, ad7124_st_reg reg)
{
	int32_t ret      = 0;
	int32_t regValue = 0;
	uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t i        = 0;
	uint8_t crc8     = 0;
	
	if(!device)
		return INVALID_VAL;
	/* Build the Command word */
	wrBuf[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR |
	  		AD7124_COMM_REG_RA(reg.addr);
	/* Fill the write buffer */
	regValue = reg.value;
	for(i = 0; i < reg.size; i++)
	{
		wrBuf[reg.size - i] = regValue & 0xFF;
		regValue >>= 8;
	}
	/* Compute the CRC */
	if(device->useCRC != AD7124_DISABLE_CRC)
	{
		crc8 = AD7124_ComputeCRC8(wrBuf, reg.size + 1);
		wrBuf[reg.size + 1] = crc8;
	}
	/* Write data to the device */
	ret = AD7124_SPI_Write(device->slave_select_id,
			wrBuf,
			(device->useCRC != AD7124_DISABLE_CRC) ? reg.size + 2
							: reg.size + 1);
	return ret;
}

/***************************************************************************//**
* @brief Reads the value of the specified register only when the device is ready
*        to accept user requests. If the device ready flag is deactivated the
*        read operation will be executed without checking the device state.
*
* @param device - The handler of the instance of the driver.
* @param pReg - Pointer to the register structure holding info about the
*               register to be read. The read value is stored inside the
*               register structure.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
 int32_t AD7124_ReadRegister(ad7124_device *device, ad7124_st_reg* pReg)
{
	int32_t ret;
	/*
	if (pReg->addr != AD7124_ERR_REG && device->check_ready)
	{
		ret = AD7124_WaitForSpiReady(device, device->spi_rdy_poll_cnt);
		if (ret < 0)
			return ret;
	}*/
	ret = AD7124_NoCheckReadRegister(device, pReg);
	return ret;
}

/***************************************************************************//**
* @brief Writes the value of the specified register only when the device is
*        ready to accept user requests. If the device ready flag is deactivated
*        the write operation will be executed without checking the device state.
*
* @param device - The handler of the instance of the driver.
* @param reg - Register structure holding info about the register to be written
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_WriteRegister(ad7124_device *device, ad7124_st_reg pReg)
{
	
	int32_t ret;
	/*
	if (device->check_ready)
	{  
		ret = AD7124_WaitForSpiReady(device, device->spi_rdy_poll_cnt);
		if (ret < 0)
			return ret;
	}*/
	ret = AD7124_NoCheckWriteRegister(device, pReg);	
	return ret;
}

/***************************************************************************//**
* @brief Resets the device.
*
* @param device - The handler of the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_Reset(ad7124_device *device)
{
	int32_t ret = 0;
	uint8_t wrBuf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	if(!device)
		return INVALID_VAL;

	ret = AD7124_SPI_Write(device->slave_select_id, wrBuf, 8);	
	return ret;
}

/***************************************************************************//**
* @brief Waits until the device can accept read and write user actions.
*
* @param device - The handler of the instance of the driver.
* @param timeout - Count representing the number of polls to be done until the
*                  function returns.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_WaitForSpiReady(ad7124_device *device, uint32_t timeout)
{
	ad7124_st_reg *regs;
	int32_t ret;
	int8_t ready = 0;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;

	while(!ready && --timeout)
	{
		/* Read the value of the Error Register */
		ret = AD7124_ReadRegister(device, &regs[AD7124_Error]);
		if(ret < 0)
			return ret;

		/* Check the SPI IGNORE Error bit in the Error Register */
		ready = (regs[AD7124_Error].value &
			 	AD7124_ERR_REG_SPI_IGNORE_ERR) == 0;
	}

	return timeout ? 0 : TIMEOUT;
}

/***************************************************************************//**
* @brief Waits until a new conversion result is available.
*
* @param device - The handler of the instance of the driver.
* @param timeout - Count representing the number of polls to be done until the
*                  function returns if no new data is available.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_WaitForConvReady(ad7124_device *device, uint32_t timeout)
{
	ad7124_st_reg *regs;
	int32_t ret;
	int8_t ready = 0;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;

	while(!ready && --timeout)
	{
		/* Read the value of the Status Register */
		ret = AD7124_ReadRegister(device, &regs[AD7124_Status]);
		if(ret < 0)
			return ret;

		/* Check the RDY bit in the Status Register */
		ready = (regs[AD7124_Status].value &
				AD7124_STATUS_REG_RDY) == 0;
	}

	return timeout ? 0 : TIMEOUT;
}


int32_t AD7124_GetChannel(ad7124_device *device)
{
	ad7124_st_reg *regs;
	int32_t ret;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;


	/* Read the value of the Status Register */
	ret = AD7124_ReadRegister(device, &regs[AD7124_Status]);
	if(ret < 0)
	{
		return ret;
	}

	if((regs[AD7124_Status].value & AD7124_STATUS_REG_RDY) == 0)
	{
		return regs[AD7124_Status].value & 0xF;
	}

	return 0;
}

/***************************************************************************//**
* @brief Reads the conversion result from the device.
*
* @param device - The handler of the instance of the driver.
* @param pData - Pointer to store the read data.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
double AD7124_ReadData(ad7124_device *device)
{
	ad7124_st_reg *regs;
	int32_t sum =0;
	double  ret =0.0;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;
  /* Read the value of the Data Register */
	AD7124_ReadRegister(device, &regs[AD7124_Data]);

	/* Get the read result */
	sum = regs[AD7124_Data].value;
  //printf("%d\n",sum);
  ret = (sum/8388608.0-1)*2500000.0;
	//ret=sum;
	return ret;
}

double AD7124_ReadAverData(ad7124_device *device)
{
	double sum;
	int8_t i=0;
	
	for(i=0;i<10;i++)
	{
		sum += AD7124_ReadData(device);
	}
	
	return sum/10.0;
}

uint32_t AD7124_ReadRawData(ad7124_device *device)
{
	ad7124_st_reg *regs;

	if(!device)
		return INVALID_VAL;

	regs = device->regs;
/* Read the value of the Data Register */
	AD7124_WaitForConvReady(device, 0xffffff);
	AD7124_ReadRegister(device, &regs[AD7124_Data]);

	//ret=sum;
	return regs[AD7124_Data].value;
}

/***************************************************************************//**
* @brief Computes the CRC checksum for a data buffer.
*
* @param pBuf - Data buffer
* @param bufSize - Data buffer size in bytes
*
* @return Returns the computed CRC checksum.
*******************************************************************************/
uint8_t AD7124_ComputeCRC8(uint8_t * pBuf, uint8_t bufSize)
{
	uint8_t i   = 0;
	uint8_t crc = 0;

	while(bufSize)
	{
		for(i = 0x80; i != 0; i >>= 1)
		{
			if(((crc & 0x80) != 0) != ((*pBuf & i) != 0)) /* MSB of CRC register XOR input Bit from Data */
			{
				crc <<= 1;
				crc ^= AD7124_CRC8_POLYNOMIAL_REPRESENTATION;
			}
			else
			{
				crc <<= 1;
			}
		}
		pBuf++;
		bufSize--;
	}
	return crc;
}

/***************************************************************************//**
* @brief Updates the CRC settings.
*
* @param device - The handler of the instance of the driver.
*
* @return None.
*******************************************************************************/
void AD7124_UpdateCRCSetting(ad7124_device *device)
{
	ad7124_st_reg *regs;

	if(!device)
		return;

	regs = device->regs;
	
	/* Get CRC State. */
	if (regs[AD7124_Error_En].value & AD7124_ERREN_REG_SPI_CRC_ERR_EN)
	{
		device->useCRC = AD7124_USE_CRC;
	}
	else
	{
		device->useCRC = AD7124_DISABLE_CRC;
	}
	//device->useCRC = AD7124_DISABLE_CRC;
}

/***************************************************************************//**
* @brief Updates the device SPI interface settings.
*
* @param device - The handler of the instance of the driver.
*
* @return None.
*******************************************************************************/
void AD7124_UpdateDevSpiSettings(ad7124_device *device)
{
	ad7124_st_reg *regs;

	if(!device)
		return;

	regs = device->regs;
	
	if (regs[AD7124_Error_En].value & AD7124_ERREN_REG_SPI_IGNORE_ERR_EN)
	{
		device->check_ready = 1;
	}
	else
	{
		device->check_ready = 0;
	}
	//device->check_ready = 0;
}

/***************************************************************************//**
* @brief Initializes the AD7124.
*
* @param device - The handler of the instance of the driver.
* @param slave_select - The Slave Chip Select Id to be passed to the SPI calls.
* @param regs - The list of registers of the device (initialized or not) to be
*               added to the instance of the driver.
*
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_Setup(ad7124_device *device)
{
	//printf("setup");
	int32_t ret;
	int i=0;
	enum ad7124_registers regNr;

	if(!device)
		return INVALID_VAL;

	device->regs = ad7124_regs;
	device->slave_select_id = 0;
	device->spi_rdy_poll_cnt = 25000;

	/* Initialize the SPI communication. */
	ret = AD7124_SPI_Init();
	delay_ms(500);
	/*  Reset the device interface.*/
	ret = AD7124_Reset(device);
	if (ret < 0)
		return ret;
	
	delay_ms(500);
	/* Update the device structure with power-on/reset settings */
	device->check_ready = 1;

	/* Initialize registers AD7124_ADC_Control through AD7124_Filter_7. */
	for(regNr = AD7124_Status; regNr < AD7124_REG_NO; regNr++)
	{
		if (ad7124_regs[regNr].rw == AD7124_RW)
		{
			ret = AD7124_WriteRegister(device, ad7124_regs[regNr]);
			if (ret < 0)
		  		break;
		}
		//delay_ms(1);
		/* Get CRC State and device SPI interface settings */
		if (regNr == AD7124_Error_En)
		{
			AD7124_UpdateCRCSetting(device);
			AD7124_UpdateDevSpiSettings(device);
		}
		i++;
	}
	/* Internal calibration AD7124_Offset_0 */
	//ret = AD7124_Calibration(device);

	return ret;
}

/***************************************************************************//**
* @brief Set the AD7124 test channle.
* @param device - The handler of the instance of the driver.
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_Calibration(ad7124_device *device)
{
	int32_t ret;
	ret = AD7124_WriteRegister( device, ad7124_regs_calibration[AD7124_ADC_Control]);
	delay_ms(100);
	ret = AD7124_WriteRegister(device, ad7124_regs[AD7124_ADC_Control]);
	delay_ms(10);
	return ret;
}
/***************************************************************************//**
* @brief Set the AD7124 test channle.
* @param device - The handler of the instance of the driver.
* @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t AD7124_SetChannle(ad7124_device *device,u8 ad7124_channle)
{
	int32_t ret;
	enum ad7124_registers regNr;

	if(!device)
		return INVALID_VAL;
	
	for(regNr = AD7124_Channel_0; regNr < AD7124_Channel_13; regNr++)
	{
		if (regNr == ad7124_channle)
		{
			device->regs[regNr].value |= 0x8000;
			ret = AD7124_WriteRegister(device, ad7124_regs[regNr]);
			if (ret < 0)   break;
		}
		else
		{
			device->regs[regNr].value &= 0x7fff;
			ret = AD7124_WriteRegister(device, ad7124_regs[regNr]);
			if (ret < 0) break;
		}
	}
	return ret;
}

int32_t AD7124_SetChannles(ad7124_device *device,u16 ad7124_channles)
{
	int32_t ret;
	uint16_t CH_NUM;
	
	enum ad7124_registers regNr;

	CH_NUM = ad7124_channles;
	
	if(!device)
		return INVALID_VAL;
		
	for(regNr = AD7124_Channel_0; regNr < AD7124_Channel_13; regNr++)
	{
		if (CH_NUM & 0x01)
		{
			device->regs[regNr].value |= 0x8000;
			CH_NUM = CH_NUM >> 1;								
		}
		else
		{
			device->regs[regNr].value &= 0x7fff;
			CH_NUM = CH_NUM >> 1;			
		}
	}
	
	for(regNr = AD7124_Status; regNr < AD7124_REG_NO; regNr++)
	{
		if (ad7124_regs[regNr].rw == AD7124_RW)
		{
			ret = AD7124_WriteRegister(device, ad7124_regs[regNr]);
			if (ret < 0)
		  		break;
		}
		//delay_ms(1);
		/* Get CRC State and device SPI interface settings */
		if (regNr == AD7124_Error_En)
		{
			AD7124_UpdateCRCSetting(device);
			AD7124_UpdateDevSpiSettings(device);
		}
	}
	return ret;
}
