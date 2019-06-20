#include "AD5933.h"
#include "AD5933_I2C.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

unsigned long currentSysClk			= AD5933_INTERNAL_SYS_CLK;
unsigned long currentClockSource 	= AD5933_CONTROL_INT_SYSCLK;
unsigned char currentRange 			= AD5933_RANGE_200mVpp;
unsigned char currentGain 			= AD5933_GAIN_X5;
const long POW_2_27 				= 134217728ul;      // 2 to the power of 27

/**********************************************************************************
//Function:	AD5933_Init
//Description: initialize DDS AD5933
//Return: void
**********************************************************************************/
void AD5933_Init(void)
{
	AD5933_IIC_Init();
	delay_ms(10);
	AD5933_Reset();
	delay_ms(10);
	
	AD5933_ConfigSweep(AD5933_STAR_FREQ, AD5933_INC_FREQ, 10);  // 30k起始，1k步长，10个点
	AD5933_ConfigSettlingCycl(AD5933_SETTLE_CYCL);
	AD5933_Standby();
	AD5933_SetSystemClk(AD5933_CONTROL_EXT_SYSCLK);
	AD5933_SetRangeAndGain(AD5933_RANGE_200mVpp, AD5933_GAIN_X5);
	AD5933_InitStartFreq();
	AD5933_StartFreqSweep();
}

/**********************************************************************************
//Function:	AD5933_SetRegisterValue
//Description:   Set register value
//Return: void
**********************************************************************************/
void AD5933_SetRegisterValue(unsigned char registerAddress, unsigned long registerValue, unsigned char bytesNumber)
{
	unsigned char byte          = 0;
    unsigned char writeData[2]  = {0, 0};

    for(byte = 0;byte < bytesNumber; byte++)
    {
    	//set resigter address
        writeData[0] = registerAddress + bytesNumber - byte - 1;
        //set register value
        writeData[1] = (unsigned char)((registerValue >> (byte * 8)) & 0xFF);
        AD5933_IIC_Write(AD5933_ADDRESS, writeData, 2, 1);
    }
}

/**********************************************************************************
//Function:	AD5933_GetRegisterValue
//Description:   Get Register Value
//Return: unsigned long
**********************************************************************************/
unsigned long AD5933_GetRegisterValue(unsigned char registerAddress, unsigned char bytesNumber)
{
	unsigned char byte = 0;
	unsigned char writeData[2]  = {0, 0};
	unsigned char readData[2] = {0, 0};
	unsigned long registerValue = 0;
	
	for(byte = 0; byte < bytesNumber; byte++)
	{
		//set register pointer
		writeData[0] = AD5933_ADDR_POINTER;
		writeData[1] = registerAddress + byte;
		//write in AD5933
		AD5933_IIC_Write(AD5933_ADDRESS,writeData,2,1);
		//read register date
    readData[0] = 0xFF;
		AD5933_IIC_Read(AD5933_ADDRESS,readData,1,1);

		//calculate register date read
		registerValue = registerValue << 8;
		registerValue += readData[0];
	}

	return registerValue;
}

/**********************************************************************************
//Function:	AD5933_Reset
//Description:   
//Return: void
**********************************************************************************/
void AD5933_Reset(void)
{
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_LB, AD5933_CONTROL_RESET | currentClockSource, 1);
}
/**********************************************************************************
//Function:	AD5933_Standby
//Description:   Standby mode
//Return: void
**********************************************************************************/
void AD5933_Standby(void)
{
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_STANDBY)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
}
/**********************************************************************************
//Function:	AD5933_SetSystemClk
//Description:   
//Return: void
**********************************************************************************/
void AD5933_SetSystemClk(unsigned char clkSource)
{
	currentClockSource = clkSource;
	
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_LB, currentClockSource, 1);
}

/**********************************************************************************
//Function:	AD5933_SetRangeAndGain
//Description:   
//Return: void
**********************************************************************************/
void AD5933_SetRangeAndGain(unsigned char range, unsigned char gain)
{
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_NOP)|
							AD5933_CONTROL_RANGE(range)|
							AD5933_CONTROL_PGA_GAIN(gain),
							1);

	currentRange = range;
	currentGain = gain;
}

/**********************************************************************************
//Function:	AD5933_ConfigSweep
//Description:   
//Return: void
**********************************************************************************/
void AD5933_ConfigSweep(unsigned long startFreq, unsigned long incFreq, unsigned int incNum)
{
	//write to register
	AD5933_SetRegisterValue(AD5933_REG_FREQ_START,	startFreq,	3);
	AD5933_SetRegisterValue(AD5933_REG_FREQ_INC,	incFreq,		3);
	AD5933_SetRegisterValue(AD5933_REG_INC_NUM,		incNum,		2);
}
/**********************************************************************************
//Function:	ConfigSettlingCycl
//Description:   
//Return: void
**********************************************************************************/
void AD5933_ConfigSettlingCycl(unsigned long settlingcycle)
{
	AD5933_SetRegisterValue(AD5933_REG_SETTLING_CYCLES,	settlingcycle,	2);
}
/**********************************************************************************
//Function:	AD5933_InitStartFreq
//Description:   
//Return: void
**********************************************************************************/
void AD5933_InitStartFreq(void)
{

	//initial start frequence
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_INIT_START_FREQ)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
}
/**********************************************************************************
//Function:	AD5933_StartFreqSweep
//Description:   
//Return: void
**********************************************************************************/
void AD5933_StartFreqSweep(void)
{

	//initial start frequence
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_START_SWEEP)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
}

/**********************************************************************************
//Function:	AD5933_StartSweep
//Description:   
//Return: void
**********************************************************************************/
void AD5933_StartSweep(void)
{
	unsigned char status = 0;
	//set AD5933 in standby mode
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_STANDBY)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
	//reset AD5933
	AD5933_Reset();
	//initial start frequence
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_INIT_START_FREQ)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
	delay_ms(10);
	//start sweep
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_START_SWEEP)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
	
	//wait for the action completed
	while((status & AD5933_STAT_DATA_VALID) == 0)
	{
		status = AD5933_GetRegisterValue(AD5933_REG_STATUS,1);
	}
}

/**********************************************************************************
//Function:	AD5933_CalculateGainFactor
//Description: not Caculate in 103board  
//Return: void
**********************************************************************************/
double AD5933_CalculateGainFactor(unsigned long calibrationImpedance, unsigned char freqFunction)
{
	double gainFactor = 0;
	/*unsigned char status = 0;
	signed short realData = 0;
	signed short imagData = 0;
	double magnitude = 0;

	//set frequency function as requirement
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(freqFunction)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
	//wait for the action completed
	while((status & AD5933_STAT_DATA_VALID) == 0)
	{
		status = AD5933_GetRegisterValue(AD5933_REG_STATUS,1);
	}
	//get and calculate measurement value
	realData = AD5933_GetRegisterValue(AD5933_REG_REAL_DATA, 2);
	imagData = AD5933_GetRegisterValue(AD5933_REG_IMAG_DATA, 2);
	magnitude = __sqrtf(realData * realData + imagData * imagData);
	gainFactor = 1 / (magnitude * calibrationImpedance);
  */
	return gainFactor;
}

/**********************************************************************************
//Function:	AD5933_CalculateImpedance
//Description:   not calculate in 103 board
//Return: double
**********************************************************************************/

	double AD5933_CalculateImpedance(double gainFactor, unsigned char freqFunction)
{
	
	double impedance = 0;
	/*
	unsigned char status = 0;
	s16 realData = 0;
	s16 imagData = 0;	
	double magnitude = 0;
	unsigned long m = 0;
	//printf("AD5933 readData");
	
	//set frequency function as requirement
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(freqFunction)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
	//wait for the action completed
	while((status & AD5933_STAT_DATA_VALID) == 0)
	{
		 //printf("AD5933 GET REG");
		status = AD5933_GetRegisterValue(AD5933_REG_STATUS,1);
	}
	//get and calculate measurement value
    realData = AD5933_GetRegisterValue(AD5933_REG_REAL_DATA,2);
    imagData = AD5933_GetRegisterValue(AD5933_REG_IMAG_DATA,2);
    magnitude = __sqrtf((realData * realData) + (imagData * imagData));//103无法实现开平方
      impedance = 1 / (magnitude * gainFactor);
	
	  printf("AD5933 realData: %d\r\n",realData);
    printf("AD5933 imagData: %d\r\n",imagData);
	  printf("AD5933 magnitude: %lud\r\n",m);
		*/
    return impedance; 	
}
uint32_t AD5933_CalRelatedImp(unsigned char freqFunction)
{
	uint32_t RI = 0;
	unsigned char status = 0;
	s16 realData = 0;
	s16 imagData = 0;	
	//printf("AD5933 readData");
	
	//set frequency function as requirement
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(freqFunction)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);
	//wait for the action completed
	while((status & AD5933_STAT_DATA_VALID) == 0)
	{
		 //printf("AD5933 GET REG");
		status = AD5933_GetRegisterValue(AD5933_REG_STATUS,1);
	}
	//get and calculate measurement value
    realData = AD5933_GetRegisterValue(AD5933_REG_REAL_DATA,2);
	  delay_us(20);
    imagData = AD5933_GetRegisterValue(AD5933_REG_IMAG_DATA,2);
	  //printf("AD5933 realData: %d\r\n",realData);
    //printf("AD5933 imagData: %d\r\n",imagData);
	  RI = realData * realData + imagData * imagData;   //103无法实现开平方		
	  //printf("AD5933 magnitude: %lud\r\n",RI);
    return RI; 	
}
/**********************************************************************************
//Function:	ReadTemp
//Description:   not used in test
//Return: unsigned long
**********************************************************************************/
unsigned long ReadTemp(void)
{
	  unsigned long         temperature = 0;
	  
    unsigned char status      = 0;
    
    AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB,
                         AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_MEASURE_TEMP) |
                         AD5933_CONTROL_RANGE(currentRange) | 
                         AD5933_CONTROL_PGA_GAIN(currentGain),                             
                         1);
    while((status & AD5933_STAT_TEMP_VALID) == 0)
    {
        status = AD5933_GetRegisterValue(AD5933_REG_STATUS,1);
    }
    
    temperature = AD5933_GetRegisterValue(AD5933_REG_TEMP_DATA,2);
    if((unsigned long)temperature < 8192)
    {
        temperature /= 32;
    }
    else
    {
        temperature -= 16384;
        temperature /= 32;
    }
    
    return temperature;
}

/**********************************************************************************
//Function:	AD5933_GetTemperature
//Description:   not used in test
//Return: unsigned char
**********************************************************************************/
unsigned char AD5933_GetTemperature(void)
{
	
	unsigned char temperature = 0;
	/*
	unsigned char stateValue = 0;
	AD5933_SetRegisterValue(AD5933_REG_CONTROL_HB, 
							AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_MEASURE_TEMP)|
							AD5933_CONTROL_RANGE(currentRange)|
							AD5933_CONTROL_PGA_GAIN(currentGain),
							1);

	//read till temperature convert completed
	while(stateValue &AD5933_STAT_TEMP_VALID)
	{
		stateValue = AD5933_GetRegisterValue(AD5933_REG_STATUS,1);
	}

	//read temperature data
	temperature = AD5933_GetRegisterValue(AD5933_REG_TEMP_DATA,2);

	//calculate temperature
    if(temperature < 8192)
    {
        temperature /= 32;
    }
    else
    {
        temperature -= 16384;
        temperature /= 32;
    }
    */
    return (char)temperature;
}
