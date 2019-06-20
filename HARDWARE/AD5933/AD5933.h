#ifndef _AD5933_H_
#define _AD5933_H_

#include "sys.h"
//AD5933 address
#define AD5933_ADDRESS 0x0D

//AD5933 Registers
#define AD5933_REG_CONTROL_HB       0x80    // HB of the Control register
#define AD5933_REG_CONTROL_LB       0x81    // LB of the Control register
#define AD5933_REG_FREQ_START       0x82    // Start frequency
#define AD5933_REG_FREQ_INC         0x85    // Frequency increment
#define AD5933_REG_INC_NUM          0x88    // Number of increments
#define AD5933_REG_SETTLING_CYCLES  0x8A    // Number of settling time cycles
#define AD5933_REG_STATUS           0x8F    // Status
#define AD5933_REG_TEMP_DATA        0x92    // Temperature data
#define AD5933_REG_REAL_DATA        0x94    // Real data
#define AD5933_REG_IMAG_DATA        0x96    // Imaginary data

//AD5933_REG_CONTROL_HB Bits
#define AD5933_CONTROL_FUNCTION(x)  ((x) << 4)     
#define AD5933_CONTROL_RANGE(x)     ((x) << 1)
#define AD5933_CONTROL_PGA_GAIN(x)  ((x) << 0)

//AD5933_REG_CONTROL_LB Bits
#define AD5933_CONTROL_RESET        (0x1 << 4)
#define AD5933_CONTROL_INT_SYSCLK   (0x0 << 3)
#define AD5933_CONTROL_EXT_SYSCLK   (0x1 << 3)

//AD5933_CONTROL_FUNCTION(x) options
#define AD5933_FUNCTION_NOP                 0x0 //无操作
#define AD5933_FUNCTION_INIT_START_FREQ     0x1 //以起始频率初始化
#define AD5933_FUNCTION_START_SWEEP         0x2 //启动频率扫描
#define AD5933_FUNCTION_INC_FREQ            0x3 //递增频率
#define AD5933_FUNCTION_REPEAT_FREQ         0x4 //重复频率
#define AD5933_FUNCTION_MEASURE_TEMP        0x9 //测量温度
#define AD5933_FUNCTION_POWER_DOWN          0xA //省电模式
#define AD5933_FUNCTION_STANDBY             0xB //待机模式

//AD5933 Specifications
#define AD5933_INTERNAL_SYS_CLK     16000000ul      // 16MHz
#define AD5933_MAX_INC_NUM          511             // Maximum increment number

//AD5933_CONTROL_RANGE(x) options
#define AD5933_RANGE_2000mVpp       0x0
#define AD5933_RANGE_200mVpp        0x1
#define AD5933_RANGE_400mVpp        0x2
#define AD5933_RANGE_1000mVpp       0x3

//AD5933_CONTROL_PGA_GAIN(x) options
#define AD5933_GAIN_X5              0
#define AD5933_GAIN_X1              1

//AD5933_InitialConfig
#define AD5933_STAR_FREQ             0xF5C28  //起始频率 0x51eb8 10k  0xF5C28 30K
#define AD5933_INC_FREQ              0x82dc   //频率增量 1k
#define AD5933_SETTLE_CYCL           0xf      //稳定周期

//AD5933_REG_STATUS Bits
#define AD5933_STAT_TEMP_VALID      (0x1 << 0)
#define AD5933_STAT_DATA_VALID      (0x1 << 1)
#define AD5933_STAT_SWEEP_DONE      (0x1 << 2)

/* AD5933 Block Commands */
#define AD5933_BLOCK_WRITE          0xA0
#define AD5933_BLOCK_READ           0xA1
#define AD5933_ADDR_POINTER         0xB0

void AD5933_Init(void);
void AD5933_InitStartFreq(void);
void AD5933_Reset(void);
void AD5933_Standby(void);//
void AD5933_SetSystemClk(unsigned char clkSource);
void AD5933_SetRangeAndGain(unsigned char range, unsigned char gain);
void AD5933_ConfigSweep(unsigned long startFreq, unsigned long incFreq, unsigned int incNum);
void AD5933_ConfigSettlingCycl(unsigned long settlingcycle);
void AD5933_StartSweep(void);
void AD5933_StartFreqSweep(void);

unsigned long ReadTemp(void);
double AD5933_CalculateImpedance(double gainFactor, unsigned char freqFunction);
uint32_t AD5933_CalRelatedImp(unsigned char);

#endif
