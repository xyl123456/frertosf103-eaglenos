/***********************************************************************************
Copyright  Eaglenos Inc. 2019-2029.All right reserved.
File name:     DetectBoard.c
Author:        jiaruoxue
Date:          2019-6-18
Description:   ��װ��POCT�豸�źŰ��ϵ�������Ϊ�����ṩ��ʼ�����Լ졢���Կ�ABC��
               ���Һ�嵽λ�ĺ���
History:  2019.0618 rev.1
***********************************************************************************/
#include "DetectBoard.h"
#include "AD7124.h"
#include "platform_drivers.h"
#include "AD5933.h"
#include "AD5933_I2C.h"
#include "AD5676.h"
#include "SPI1.h"
#include "AnalogSwitch.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

ad7124_device ad7124Dev;
/***********************************************************************************
** @Description:�źŰ��ʼ������
** @para :       void
** @return       int8_t  ���ش�����Ϣ��0Ϊ�ɹ���1Ϊerror
***********************************************************************************/
int8_t DetectBoard_Initial(void)
{
	int8_t ret = 0;
	delay_init();  //��ʱ��������
	
	/*ad5676��ʼ��*/
	AD5676_Init();
	AD5676_WriteToInputRegDAC(DAC_0,DAC_0_value );   
	AD5676_WriteToInputRegDAC(DAC_1,DAC_1_value );
	AD5676_WriteToInputRegDAC(DAC_2,DAC_2_value );
	AD5676_WriteToInputRegDAC(DAC_3,DAC_3_value );
	AD5676_WriteToInputRegDAC(DAC_4,DAC_4_value );   
	AD5676_WriteToInputRegDAC(DAC_5,DAC_5_value);   
	//AD5676_PowerDown_ALL(); 
	
	/*ad5933��ʼ��*/
	AD5933_Init();
	
	/*ad7124��ʼ��*/	
  AD7124_Setup(&ad7124Dev);
	
	/*ģ�⿪�س�ʼ��*/
	SW_GPIO_Init();
	SW_TEST_config(SW_SC);
	
	return ret;
}

/***********************************************************************************
** @Description:�źŰ��Լ캯��
** @para :       void
** @return       int8_t  ���ش�����Ϣ��0Ϊ�ɹ���1Ϊerror
***********************************************************************************/
int8_t DetectBoard_SelfCheck(void)
{
	int8_t ret = 0;
	uint32_t t=0;
	uint32_t SC[13];
	uint8_t i,j;
	
	/*�Լ�����*/
	//AD5676_PowerUp_ALL();
	AD7124_SetChannles(&ad7124Dev,AD7124_All_Channel);
	SW_TEST_config(SW_SC);
	delay_ms(10);
	
	/*��ȡ����ͨ�����Լ�ֵ*/
	for(i=0;i<13;i++){
		t=AD7124_ReadRawData(&ad7124Dev);
		if(i==(t&0xf))
			SC[i]=t>>8;
		else
			i--;
		//delay_ms(10);	
		//printf("AD7124 : %d\r\n",SC[i]);
		//printf("CH : %d\r\n",i);
	}
	
  /*�Լ����Ƚ�*/
	for(j=0;j<8;j++){
		if(SC[j]<9000000||SC[j]>9200000)
			ret=1;
	}
	if(SC[8]<8900000||SC[8]>9000000)
		ret=1;
	if(SC[9]<9200000||SC[9]>9400000)
		ret=1;
	if(SC[10]<9700000||SC[10]>9900000)
		ret=1;
	if(SC[11]<9900000||SC[11]>11000000)
		ret=1;
	if(SC[12]<9900000||SC[12]>11000000)
		ret=1;
	
	return ret;
}

/***********************************************************************************
** @Description:��Һλ�ü�⺯��
** @para :       void
** @return       int8_t  ���ش�����Ϣ��0ΪҺ�嵽λ��1ΪҺ��δ��λ
***********************************************************************************/
int8_t DetectBoard_Check_R1(void)
{
	//printf("R1\r\n");
	int8_t ret=1;
	uint32_t RI=0;
	SW_Impedance1_config(SW_R1);
	delay_us(20);
	RI=AD5933_CalRelatedImp(AD5933_FUNCTION_REPEAT_FREQ);
	//printf("RI: %lud\r\n",RI);
	
	/*Һ�嵽λ�ж�*/
	if(RI>30000)
		ret=0;
	return ret;
}

int8_t DetectBoard_Check_R2(void)
{
	int8_t ret=1;
	uint32_t RI=0;
	SW_Impedance1_config(SW_R2);
	delay_us(20);
	RI=AD5933_CalRelatedImp(AD5933_FUNCTION_REPEAT_FREQ);
	
	/*Һ�嵽λ�ж�*/
	if(RI>30000)
		ret=0;
	return ret;
}

/***********************************************************************************
** @Description: ��⿨���Ժ��� A��
** @para :       TestCard_One * CartridgeA
** @return       int8_t  ���ش�����Ϣ��0���ݶ�ȡ��ɣ�1Ϊ����error
***********************************************************************************/
int8_t DetectBoard_GetCartridgeAData(TestCard_One * CartridgeA)
{
	int8_t ret = 0;
	int8_t count[5] = {0,0,0,0,0};
	uint32_t t=0;
	
	uint8_t i,j,n;
	/*��A��������*/
	AD7124_SetChannles(&ad7124Dev,AD7124_CartridgeA_Channel);
	SW_TEST_config(SW_P1);
	delay_ms(10);
	
	/*����*/
	for(i=0;i<Apoints;i++){
		for(j=0;j<6;j++){
			t=AD7124_ReadRawData(&ad7124Dev);
			if(j<=(t&0xf)){
				j=(t&0xf);
				switch (j){
					case 0:
						CartridgeA->TestCardOne.iMg[i] =0x00ffffff&(t>>8);
					  printf("CH:%d\r\n",j);
					  printf("Value:%d\r\n",CartridgeA->TestCardOne.iMg[i]);
					  count[0]++;
					  break;
					case 1:
						CartridgeA->TestCardOne.iCa[i] = 0x00ffffff&(t>>8);
					  printf("CH:%d\r\n",j);
					  printf("Value:%d\r\n",CartridgeA->TestCardOne.iCa[i]);
					  count[1]++;
					  break;
					case 2:
						CartridgeA->TestCardOne.K[i] = 0x00ffffff&(t>>8);
					  printf("CH:%d\r\n",j);
					  printf("Value:%d\r\n",CartridgeA->TestCardOne.K[i]);
					  count[2]++;
					  break;
					case 3:
						CartridgeA->TestCardOne.Na[i] = 0x00ffffff&(t>>8);
					  printf("CH:%d\r\n",j);
					  printf("Value:%d\r\n",CartridgeA->TestCardOne.Na[i]);
					  count[3]++;
					  break;
					case 5:
						CartridgeA->TestCardOne.Cl[i] = 0x00ffffff&(t>>8);
					  printf("CH:%d\r\n",j);
					  printf("Value:%d\r\n",CartridgeA->TestCardOne.Cl[i]);
					  count[4]++;
					  break;
				}	
			}else	
			{
			 // j--;	
			}
		  //delay_ms(10);	
		}
	  //delay_ms(10);	
	}
	
	/*error check*/
	for(n=0;n<5;n++){
		//printf("%d\r\n",count[n]);
		if (count[n]!=Apoints)
		{
			ret = 1;
		}
	}
	return ret;
}

/***********************************************************************************
** @Description: ��⿨���Ժ��� B��
** @para :       TestCard_Two * CartridgeB
** @return       int8_t  ���ش�����Ϣ��0���ݶ�ȡ��ɣ�1Ϊ����error
***********************************************************************************/
int8_t DetectBoard_GetCartridgeBData(TestCard_Two * CartridgeB)
{
	int8_t ret = 0;
	int8_t count[6]= {0,0,0,0,0,0};
	uint32_t t=0;
	uint8_t i,j,n;
	/*��A��������*/
	AD7124_SetChannles(&ad7124Dev,AD7124_CartridgeB_Channel);//����ͨ��������ת��
	SW_TEST_config(SW_P1);
	delay_ms(10);
	
	/*����*/
	for(i=0;i<Apoints;i++){
		for(j=0;j<13;j++){
			
			t=AD7124_ReadRawData(&ad7124Dev);
			if(j<=(t&0xf)){
				j=t&0xf;
				switch (j){
					case 4:
						CartridgeB->TestCardTwo.PH[i] = 0x00ffffff&(t>>8);	
            count[0]++;
					  break;
					case 6:
						CartridgeB->TestCardTwo.CO2_1[i] = 0x00ffffff&(t>>8);
					  count[1]++;
					  break;
					case 7:
						CartridgeB->TestCardTwo.CO2_2[i] = 0x00ffffff&(t>>8);
					  count[2]++;
					  SW_TEST_config(SW_A3);
					  break;
					case 10:
						CartridgeB->TestCardTwo.glu[i] = 0x00ffffff&(t>>8);
					  count[3]++;
					  SW_TEST_config(SW_A4);
					  break;
					case 11:
						CartridgeB->TestCardTwo.pO2[i] = 0x00ffffff&(t>>8);
					  count[4]++;
					  SW_TEST_config(SW_A5);
					  break;
					case 12:
						CartridgeB->TestCardTwo.Lac[i] = 0x00ffffff&(t>>8);
					  count[5]++;
					  SW_TEST_config(SW_P1);
					  break;
				}	
			}
			else	
			  j--;	
		  //delay_ms(10);	
		}
	  //delay_ms(10);	
	}
	
	/*error check*/
	for(n=0;n<6;n++){
		printf("%d\r\n",count[n]);
		if (count[n]!=Bpoints)
		ret = 1;
	}
	return ret;
}

/***********************************************************************************
** @Description: ��⿨���Ժ��� C��
** @para :       TestCard_Three * CartridgeC
** @return       int8_t  ���ش�����Ϣ��0���ݶ�ȡ��ɣ�1Ϊ����error
***********************************************************************************/
int8_t DetectBoard_GetCartridgeCData(TestCard_Three * CartridgeC)
{
	int8_t ret = 0;
	int8_t count[11]= {0,0,0,0,0,0,0,0,0,0,0};
	uint32_t t=0;
	uint8_t i,j,n;
	/*��A��������*/
	AD7124_SetChannles(&ad7124Dev,AD7124_CartridgeC_Channel);//����ͨ��������ת��
	SW_TEST_config(SW_P1);
	delay_ms(10);
	
	/*����*/
	for(i=0;i<Apoints;i++){
		for(j=0;j<13;j++){
			
			t=AD7124_ReadRawData(&ad7124Dev);
			if(j<=(t&0xf)){
				j=t&0xf;
				switch (j){
					case 0:
						CartridgeC->TestCardThree.iMg[i] = 0x00ffffff&(t>>8);	
            count[0]++;
					  break;
					case 1:
						CartridgeC->TestCardThree.iCa[i] = 0x00ffffff&(t>>8);
					  count[1]++;
					  break;
					case 2:
						CartridgeC->TestCardThree.K[i] = 0x00ffffff&(t>>8);
					  count[2]++;
					  break;
					case 3:
						CartridgeC->TestCardThree.Na[i] = 0x00ffffff&(t>>8);
					  count[3]++;
					  break;
					case 4:
						CartridgeC->TestCardThree.PH[i] = 0x00ffffff&(t>>8);
					  count[4]++;
					  break;
					case 5:
						CartridgeC->TestCardThree.Cl[i] = 0x00ffffff&(t>>8);
					  count[5]++;
					  SW_TEST_config(SW_P1);
					  break;
					case 6:
						CartridgeC->TestCardThree.CO2_1[i] = 0x00ffffff&(t>>8);
					  count[6]++;
					  break;
					case 7:
						CartridgeC->TestCardThree.CO2_2[i] = 0x00ffffff&(t>>8);
					  count[7]++;
					  SW_TEST_config(SW_A3);
					  break;
					case 10:
						CartridgeC->TestCardThree.glu[i] = 0x00ffffff&(t>>8);
					  count[8]++;
					  SW_TEST_config(SW_A4);
					  break;
					case 11:
						CartridgeC->TestCardThree.pO2[i] = 0x00ffffff&(t>>8);
					  count[9]++;
					  SW_TEST_config(SW_A5);
					  break;
					case 12:
						CartridgeC->TestCardThree.Lac[i] = 0x00ffffff&(t>>8);
					  count[10]++;
					  SW_TEST_config(SW_P1);
					  break;
				}	
			}
			else	
			  j--;	
		  //delay_ms(10);	
		}
	  //delay_ms(10);	
	}
	
	/*error check*/
	for(n=0;n<11;n++){
		//printf("%d\r\n",count[n]);
		if (count[n]!=Cpoints)
		ret = 1;
	}
	return ret;
}

/********************************** End of file ***********************************/
