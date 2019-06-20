#ifndef __AD7124_H__
#define __AD7124_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "AD7124_regs.h"

#define  AD7124_All_Channel   0x1fff
#define  AD7124_CartridgeA_Channel   0x002f
#define  AD7124_CartridgeB_Channel   0x1cd0
#define  AD7124_CartridgeC_Channel   0x1fff

/*! Device register info */
typedef struct _ad7124_st_reg
{
	int32_t addr;
	uint32_t value;
	int32_t size;
	int32_t rw;
}ad7124_st_reg;

/*! AD7124 registers list*/
typedef enum ad7124_reg_access ad7124_reg_access;
enum ad7124_registers
{
	AD7124_Status = 0x00,
	AD7124_ADC_Control,
	AD7124_Data,
	AD7124_IOCon1,
	AD7124_IOCon2_,
	AD7124_ID,
	AD7124_Error,
	AD7124_Error_En,
	AD7124_Mclk_Count,
	AD7124_Channel_0,
	AD7124_Channel_1,
	AD7124_Channel_2,
	AD7124_Channel_3,
	AD7124_Channel_4,
	AD7124_Channel_5,
	AD7124_Channel_6,
	AD7124_Channel_7,
	AD7124_Channel_8,
	AD7124_Channel_9,
	AD7124_Channel_10,
	AD7124_Channel_11,
	AD7124_Channel_12,
	AD7124_Channel_13,
	AD7124_Channel_14,
	AD7124_Channel_15,	
	AD7124_Config_0,
	AD7124_Config_1,
	AD7124_Config_2,
	AD7124_Config_3,
	AD7124_Config_4,
	AD7124_Config_5,
	AD7124_Config_6,
	AD7124_Config_7,
	AD7124_Filter_0,
	AD7124_Filter_1,
	AD7124_Filter_2,
	AD7124_Filter_3,
	AD7124_Filter_4,
	AD7124_Filter_5,
	AD7124_Filter_6,
	AD7124_Filter_7,
	AD7124_Offset_0,
	AD7124_Offset_1,
	AD7124_Offset_2,
	AD7124_Offset_3,
	AD7124_Offset_4,
	AD7124_Offset_5,
	AD7124_Offset_6,
	AD7124_Offset_7,
	AD7124_Gain_0,
	AD7124_Gain_1,
	AD7124_Gain_2,
	AD7124_Gain_3,
	AD7124_Gain_4,
	AD7124_Gain_5,
	AD7124_Gain_6,
	AD7124_Gain_7,
	AD7124_REG_NO
};

/*
 * The structure describes the device and is used with the ad7124 driver.
 * @slave_select_id: The ID of the Slave Select to be passed to the SPI calls.
 * @regs: A reference to the register list of the device that the user must
 *       provide when calling the Setup() function.
 * @userCRC: Whether to do or not a cyclic redundancy check on SPI transfers.
 * @check_ready: When enabled all register read and write calls will first wait
 *               until the device is ready to accept user requests.
 * @spi_rdy_poll_cnt: Number of times the driver should read the Error register
 *                    to check if the device is ready to accept user requests,
 *                    before a timeout error will be issued.
 */
typedef struct ad7124_device ad7124_device;
struct ad7124_device {
	int slave_select_id;
	ad7124_st_reg *regs;
	int useCRC;
	int check_ready;
	int spi_rdy_poll_cnt;
};

/******************************************************************************/
/******************* AD7124 Constants *****************************************/
/******************************************************************************/
#define AD7124_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */
#define AD7124_DISABLE_CRC 0
#define AD7124_USE_CRC 1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Reads the value of the specified register. */
int32_t AD7124_ReadRegister(ad7124_device *device, ad7124_st_reg* pReg);

/*! Writes the value of the specified register. */
int32_t AD7124_WriteRegister(ad7124_device *device, ad7124_st_reg reg);

/*! Reads the value of the specified register without a device state check. */
int32_t AD7124_NoCheckReadRegister(ad7124_device *device, ad7124_st_reg* pReg);

/*! Writes the value of the specified register without a device state check. */
int32_t AD7124_NoCheckWriteRegister(ad7124_device *device, ad7124_st_reg reg);

/*! Resets the device. */
int32_t AD7124_Reset(ad7124_device *device);

/*! Waits until the device can accept read and write user actions. */
int32_t AD7124_WaitForSpiReady(ad7124_device *device, uint32_t timeout);

/*! Waits until a new conversion result is available. */
int32_t AD7124_WaitForConvReady(ad7124_device *device, uint32_t timeout);


/*! Computes the CRC checksum for a data buffer. */
uint8_t AD7124_ComputeCRC8(uint8_t* pBuf, uint8_t bufSize);

/*! Updates the CRC settings. */
void AD7124_UpdateCRCSetting(ad7124_device *device);

/*! Updates the device SPI interface settings. */
void AD7124_UpdateDevSpiSettings(ad7124_device *device);

int32_t AD7124_GetChannel(ad7124_device *device);

/*! Initializes the AD7124. */
int32_t AD7124_Setup(ad7124_device *device);
int32_t AD7124_Calibration(ad7124_device *device);
int32_t AD7124_Calibration(ad7124_device *device); 
 
/*! Set the AD7124 test channle. */
int32_t AD7124_SetChannle(ad7124_device *device,u8 ad7124_channle);
int32_t AD7124_SetChannles(ad7124_device *device,u16 ad7124_channles);

/*! Reads the conversion result from the device. */
double AD7124_ReadData(ad7124_device *device);
double AD7124_ReadAverData(ad7124_device *device);
uint32_t AD7124_ReadRawData(ad7124_device *device);


#endif /* __AD7124_H__ */
