/***********************************************************************************
Copyright  agilebiotec Co.,Ltd. 2019-2029.All right reserved.
File name:     user_process.c
Author:        xuyunlong
Date:          2019-6-11
Description:   用户处理函数
History:
***********************************************************************************/
#include "user_process.h"
#include "malloc.h"
#include "cJSON.h"
#include "error.h"
#include "cmd.h"
#include "usart.h"
#include "string.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "max6675.h"
#include "timer.h"


#include "DetectBoard.h"

uint8_t startScanFlag=0;
uint8_t heatSendMessageFlag=0;

uint8_t heatBodyFlag=0;
float setingTempValue=0;

int numb[5]={0x7FFFFF00,3333,5555,6666,888};
extern TaskHandle_t MessageProcessTask_Handler;	//通知处理句柄
uint32_t systemState;//系统状态检测变量

/*
* @Description：启动扫码设备
* @para ：void
* @return void
*/
void StartScanCode()
{
	startScanFlag=1;
	SCANCODEKEY=0;
	vTaskDelay(1000/portTICK_PERIOD_MS);
	SCANCODEKEY=1;
}

/*
* @Description：停止扫码设备
* @para ：void
* @return void
*/
void StopScanCode()
{
	SCANCODEKEY=1;
}

/*
* @Description：初始化扫码设备
* @para ：void
* @return uint32_t value
*/
uint32_t ScanCodeInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 //PB10 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
  GPIO_SetBits(GPIOB,GPIO_Pin_10);						 //PB10输出高
	
	return ERR_SELFTEST_SUCCESS;
}

/*
* @Description：实时检测温度函数
* @para ：void
* @return 无
*/
void ScanHeatingTemp(void)
{
	float reactionTempValue;//heatingTempValue;
	if(heatBodyFlag==1){
		reactionTempValue = Max6675_readTemperature(1);//反应区
		printf("the temp is %.2f\n",reactionTempValue);
		//HeatingTempValue = Max6675_readTemperature(2);//加热区
		if(reactionTempValue>=setingTempValue){
			TIM_SetCompare3(TIM3,10000);//输出占空比0%
			heatBodyFlag=0;//检测完成置标志位
			if(heatSendMessageFlag==0){
				heatSendMessageFlag=1;
			//发送消息ERR_TEST_PREPARE_SUCCESS
			xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);
				}
			}else{
				heatSendMessageFlag=0;
			}
		}
}

/*
* @Description：加热初始化函数
* @para ：void
* @return 无
*/
void HeatInit(void){
	//普通IO
	/*
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PB0 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
  GPIO_ResetBits(GPIOB,GPIO_Pin_0);						 //PB0输出0
	heatBodyFlag=0;
	*/
	heatBodyFlag=0;
	//PWM格式 TIME3 的CH3
	TIM3_PWM_Init(10000,7200);        //7200分频，周期1S      //不分频。PWM频率=72000000/900=80Khz
	TIM_SetCompare3(TIM3,10000);
}

/*
* @Description：插卡检测初始化函数
* @para ：void
* @return 无
*/
void CardCheckInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB1 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  		 //上拉输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.1
}

/*
* @Description：所有外接设备初始化函数
* @para ：void
* @return 无
*/
void HalBoardInit(void)
{
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
	
	CardCheckInit();
	Max6675Init();	
	HeatInit();
	DetectBoard_Initial();//采集板初始化
	systemState=ScanCodeInit();
	//systemState=ERR_INIT_SUCCESS;
}


/*
* @Description：检测前准备命令
* @para ：void
* @return uint32_t success  or  error
*/
uint32_t PreTestProcess(void)
{
	return ERR_TEST_PREPARE_SUCCESS;
}

/*
* @Description：用户初始化检测
* @para ：void
* @return uint32_t codevalue 
*/
uint32_t IntialProcess(void)
{
	return systemState;
}
/*
* @Description：用户自检函数,通过调用对应API返回成功或者失败接口
* @para ：void
* @return uint32_t codevalue 
*/
uint32_t SelfTestProcess(void)
{
	int8_t selfTestState =DetectBoard_SelfCheck();
	if(selfTestState==0){
		return ERR_SELFTEST_SUCCESS;
	}else{
		return ERR_SELFTEST_ERROR;
	}
}


/*extern 加热函数
* @Description：加热反应区
* @para ：uint8_t tempValue，设定到达的温度
* @return void
*/
void StartHeatBody(float tempValue)
{
	//开启标志位，并且开始加热
	heatBodyFlag=1;
	setingTempValue=tempValue;
	TIM_SetCompare3(TIM3,5000);//输出占空比50%
	//HEATCONTROL=1;
	/*
	float ReactionTempValue;//HeatingTempValue;
			ReactionTempValue = Max6675_readTemperature(1);//反应区
			//HeatingTempValue = Max6675_readTemperature(2);//加热区
			if(ReactionTempValue < tempValue){
				//HEATCONTROL=1;
				TIM_SetCompare3(TIM3,5000);
			}else{
				//HEATCONTROL=0;
			}
		*/	
		vTaskDelay(1000/portTICK_PERIOD_MS);	
	
}




/*extern 函数
* @Description：发送检测到的数据
* @para ：void*buf
* @return void
*/
void SendSampleCollect(uint8_t sort ,void *buf)
{
	uint8_t i=sort;
	int cnt_i;
	TestCard_One 			testCardOne;
	TestCard_Two 			testCardTwo;
	TestCard_Three 		testCardThree;
	int MgInt[Apoints];
	int CaInt[Apoints];
	int KInt[Apoints];
	int NaInt[Apoints];
	int ClInt[Apoints];
	
	int len;
	char *out;
	//封装JSON数据
	cJSON *cjson_message,*mes_data,*data_item;
	cjson_message = cJSON_CreateObject();
	cJSON_AddNumberToObject(cjson_message,CMD_CODE,ERR_NONE);
	cJSON_AddStringToObject(cjson_message, CMD_MSG, "SampleCollect Success");
	cJSON_AddStringToObject(cjson_message, CMD_TYPE, CMD_TYPE_DATA_REPORT);
	
	switch(i){
		case 1:
			mymemcpy(testCardOne.buffer,buf,sizeof(buf));
			cJSON_AddItemToObject(cjson_message, CMD_VAL, mes_data = cJSON_CreateArray());
			cJSON_AddItemToArray(mes_data, data_item=cJSON_CreateObject());
		for(cnt_i=0;cnt_i<5;cnt_i++){
			MgInt[cnt_i]=(testCardOne.TestCardOne.iMg[cnt_i]>>8)&0xFFFFFFFF;
			CaInt[cnt_i]=(testCardOne.TestCardOne.iCa[cnt_i]>>8)&0xFFFFFFFF;
			KInt[cnt_i]=(testCardOne.TestCardOne.K[cnt_i]>>8)&0xFFFFFFFF;
			NaInt[cnt_i]=(testCardOne.TestCardOne.Na[cnt_i]>>8)&0xFFFFFFFF;
			ClInt[cnt_i]=(testCardOne.TestCardOne.Cl[cnt_i]>>8)&0xFFFFFFFF;
		}
		
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_IMG, cJSON_CreateIntArray(MgInt,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_CA, cJSON_CreateIntArray(CaInt,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_K, cJSON_CreateIntArray(KInt,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_NA, cJSON_CreateIntArray(NaInt,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_CL, cJSON_CreateIntArray(ClInt,Apoints));
		
		
		 //cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_CL, cJSON_CreateIntArray(numb,5));
		
			break;
		case 2:
			mymemcpy(testCardTwo.buffer,buf,sizeof(buf));
			break;
		case 3:
			mymemcpy(testCardThree.buffer,buf,sizeof(buf));
			break;
		case 4:
			//mymemcpy(testCardTwo.buffer,buf,sizeof(buf));
			break;
		case 5:
			//mymemcpy(testCardTwo.buffer,buf,sizeof(buf));
			break;
		default:
			break;	
	}
	
	out = cJSON_PrintUnformatted(cjson_message);
	len=strlen(out);
	MyUartSend(UART1_PORT,out,len);
	free(out);
	cJSON_Delete(cjson_message);
}



/*extern 函数
* @Description：获取测试卡的数值
* @para ：void
* @return uint8_t
*/
uint32_t GetTestCardValue(char *keyString){
	return 1;
}
/*
* @Description：处理样本检测的具体的value值的内容，主要依据的是键值对进行处理
* @para ：void
* @return 无
*/
void ProcessValueKeyDate(char *keyString,char *valueString)
{
	//开关机
	if(mymemcmp(keyString,CMD_TEST_VAL_POWER,strlen(CMD_TEST_VAL_POWER))==0){
		
		if(mymemcmp(valueString,"on",2)==0){
			
		}else{
			
		}
	}
	//扫描功能
	if(mymemcmp(keyString,CMD_TEST_VAL_SCAN_CARD,strlen(CMD_TEST_VAL_SCAN_CARD))==0){
		if(mymemcmp(valueString,"on",2)==0){
			StartScanCode();
		}else{
			StopScanCode();
		}
	}
	//插卡处理,主动上报命令
//	if(mymemcmp(keyString,CMS_TEST_INSERT_CARD,strlen(CMS_TEST_INSERT_CARD))==0){
//		if(mymemcmp(valueString,"on",2)==0){
//			
//		}
//		
//	}
	
	//样本检测前预处理命令功能，包含加热，推动电机等
	if(mymemcmp(keyString,CMD_TEST_VAL_PRE_TEST,strlen(CMD_TEST_VAL_PRE_TEST))==0){
		double tempValue;
		//1.推加热片
		//2.加热  class_value=1 为加热电机,
		//int8_t MotorStart(uint8_t class_value,int angle,int speed);
		//int8_t MotorStop(uint8_t class_value);
		//开始加热
		//StartHeatBody(float tempValue);
		tempValue=atof(valueString);
		StartHeatBody((float)tempValue);
	}
	//样品采集命令
	if(mymemcmp(keyString,CMD_TEST_VAL_COLLECT,strlen(CMD_TEST_VAL_COLLECT))==0){
		if(mymemcmp(valueString,CMD_TEST_CARD_ONE,strlen(CMD_TEST_CARD_ONE))==0){
			TestCard_One testCardOne;
			int8_t sampleCollectState;
			sampleCollectState=DetectBoard_GetCartridgeAData(&testCardOne);
			if(sampleCollectState!=0)
			{
				//数据检测错误,发送检测失败消息
							xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_SAMPLE_COLLECT_ERR,
									(eNotifyAction)eSetBits);
			}else{
			//发送数据
			SendSampleCollect(SAMPLE_COLLECT_CARD_ONE,testCardOne.buffer);
			}
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_TWO,strlen(CMD_TEST_CARD_TWO))==0){
			TestCard_Two testCardTwo;
			DetectBoard_GetCartridgeBData(&testCardTwo);
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_THREE,strlen(CMD_TEST_CARD_THREE))==0){
			TestCard_Three testCardThree;
			DetectBoard_GetCartridgeCData(&testCardThree); 
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_FOUR,strlen(CMD_TEST_CARD_FOUR))==0){
			
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_FIVE,strlen(CMD_TEST_CARD_FIVE))==0){
			
		}
	}
	//打印命令
	if(mymemcmp(keyString,CMD_TEST_VAL_PRINT,strlen(CMD_TEST_VAL_PRINT))==0){
		
	}
}

/*
* @Description：串口1数据处理函数
* @para ：void
* @return 无
*/
void Uart1Process(char *uartMemary,u8 uartLen)
{
	cJSON *root;
	root = cJSON_Parse(uartMemary);
	
	if(NULL != root){
		int getCode = cJSON_GetObjectItem(root,CMD_CODE)->valueint;
		//其它类的code
		if(getCode == ERR_NONE){
			char *typeSting = cJSON_GetObjectItem(root,"type")->valuestring;
			if(mymemcmp(typeSting,CMD_TYPE_CONNECT,strlen(CMD_TYPE_CONNECT))==0){
				//串口连接
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_CONNECT_SUCCESS,
									(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeSting,CMD_TYPE_SELF_TEST,strlen(CMD_TYPE_SELF_TEST))==0){
							//自检命令通信,处理自检任务
				uint32_t selfTestCode = SelfTestProcess();
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
							(uint32_t)selfTestCode,
							(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeSting,CMD_TYPE_SAMPLE_TEST,strlen(CMD_TYPE_SAMPLE_TEST))==0){
				//样品检测类命令
				//获取value命令值
				cJSON *valueArry = cJSON_GetObjectItem(root,"value");//获取value数组
				int arraySize = cJSON_GetArraySize(valueArry);//获取数组大小
				int i,j;
				for(i=0;i<arraySize;i++){
					cJSON *valueList = cJSON_GetArrayItem(valueArry,i);
					for(j=0;j<cJSON_GetArraySize(valueList);j++){
						char *keyString = cJSON_GetArrayItem(valueList,j)->string;
						char *valueString = cJSON_GetArrayItem(valueList,j)->valuestring;
						ProcessValueKeyDate(keyString,valueString);
						}	
				}
			}
			if(mymemcmp(typeSting,CMD_TYPE_QC,strlen(CMD_TYPE_QC))==0){
				//QC测试
			}
			if(mymemcmp(typeSting,CMD_TYPE_SET,strlen(CMD_TYPE_SET))==0){
				//SET设置
			}
		}
		
		/*
		if(parseValueDataLog==1){
			char *typeSting = cJSON_GetObjectItem(root,"type")->valuestring;
			//SendString(typeSting,strlen(typeSting));
			//根据设备类型处理
			if(memcmp(typeSting,"heart",5)==0){
				//处理心跳数值
				ResponHeart();
			}
			if(memcmp(typeSting,"sample_test",11)==0){
				//获取命令值
				cJSON *valueArry = cJSON_GetObjectItem(root,"value");//获取value数组
				int arraySize = cJSON_GetArraySize(valueArry);//获取数组大小
				
				int i,j;
				for(i=0;i<arraySize;i++){
					cJSON *valueList = cJSON_GetArrayItem(valueArry,i);
					for(j=0;j<cJSON_GetArraySize(valueList);j++){
						char *keyString = cJSON_GetArrayItem(valueList,j)->string;
						char *valueString = cJSON_GetArrayItem(valueList,j)->valuestring;
						
						ProcessAdc(keyString,valueString);//处理具体的命令
						//SendString(keySting,strlen(keySting));
						}
						//char * keyString = cJSON_GetObjectItem(valueList,);	
				}
			}
			cJSON_free((void *) typeSting);
		}
		*/
		cJSON_Delete(root);
	}else{
	}
}

/*
* @Description：串口2数据处理函数
* @para ：char *uartMemary,u8 uartLen
* @return 无
*/
void Uart2Process(char *uartMemary,u8 uartLen)
{
	int len;
	char *out;
	//封装JSON数据
	cJSON *cjson_message,*mes_data,*data_item;
	cjson_message = cJSON_CreateObject();
	cJSON_AddNumberToObject(cjson_message,CMD_CODE,ERR_NONE);
	cJSON_AddStringToObject(cjson_message, CMD_MSG, "Scan Success");
	cJSON_AddStringToObject(cjson_message, CMD_TYPE, CMD_TYPE_DATA_REPORT);
	cJSON_AddItemToObject(cjson_message, CMD_VAL, mes_data = cJSON_CreateArray());
	cJSON_AddItemToArray(mes_data, data_item=cJSON_CreateObject());
	cJSON_AddStringToObject(data_item, CMD_REPORT_VAL_TESTCARD_INFO, uartMemary);
	out = cJSON_PrintUnformatted(cjson_message);
	len=strlen(out);
	if(startScanFlag==1){
		MyUartSend(UART1_PORT,out,len);
		startScanFlag=0;
	}
	free(out);
	//cJSON_Delete(mes_data);
	//cJSON_Delete(data_item);
	cJSON_Delete(cjson_message);
}



/*
* @Description：数据处理函数
* @para ：void
* @return 无
*/
void UartProcess(u8 uartPort,char *uartMemary,u8 uartLen)
{
	switch(uartPort)
	{
		case UART1_PORT:
			//MyUartSend(UART1_PORT,uartMemary,uartLen);
			Uart1Process(uartMemary,uartLen);
			break;
		case UART2_PORT:
			//MyUartSend(UART1_PORT,uartMemary,uartLen);
			Uart2Process(uartMemary,uartLen);
			break;
		case UART3_PORT:
			break;
		case UART4_PORT:
			break;
	}
}

/*
//{
    "code":  200,
    "messg":”success”,
//}
*/

/*
* @Description：消息处理函数
* @para ：uint32_t NotifyValue 传递的消息值
* @return 无
*/
void MessageProcess(uint32_t NotifyValue)
{
	int len;
	char *out;
	//封装JSON数据
	cJSON *cjson_message;
	uint32_t notifyValue;
	notifyValue=NotifyValue;
	cjson_message = cJSON_CreateObject();
	cJSON_AddNumberToObject(cjson_message,CMD_CODE,notifyValue);
	
	switch(notifyValue){
		/*初始化成功*/
		case ERR_INIT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Init Success");
			break;
		case ERR_INIT_UART:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Uart Init Error");
			break;
		/* 自检 */
		case ERR_SELFTEST_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Self Test Success");
			break;
		case ERR_SELFTEST_POTENTIO:
		case ERR_SELFTEST_AMPERO:
		case ERR_SELFTEST_IMPEDANCE:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Self Test Error");
			break;
		/*样本检测*/
		case ERR_TEST_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Simple Test Success");
			break;
		case ERR_TEST_HAVE_RESIDUE_SOLUTION:
		case ERR_TEST_INSERT_CARD_TOUT:
		case ERR_TEST_GET_VALUE:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Test Card Error");
			break;
		/*电机驱动错误*/
		case ERR_TEST_MOTOR_PUSH:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Motor Error");
			break;
		/*预加热处理任务成功*/
		case ERR_TEST_PREPARE_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Pre test Success");
			break;
		case ERR_TEST_HEAT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Heat to set value");
			break;
		/*加热出错*/
		case ERR_TEST_HEAT:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Heat Error");
			break;
		/*检测卡检测出错*/
		case ERR_TEST_SAMPLE_COLLECT_ERR:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Sample Collect Error");
			break;
		/*串口连接成功*/
		case ERR_CONNECT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Uart Success");
		break;
		/*卡插入成功*/
		case ERR_TEST_CARD_INSERT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Card Insert Success");
		
		default:
			break;
	}

	
	//cJSON_AddStringToObject(cjson_message, "type", "data");
	//cJSON_AddItemToObject(cjson_message, "value", mes_data = cJSON_CreateArray());
	//cJSON_AddItemToArray(mes_data, data_item=cJSON_CreateObject());
	//cJSON_AddNumberToObject(data_item, "adc", fVoltageADC1);
	
	//out = cJSON_Print(cjson_message);
	
	out = cJSON_PrintUnformatted(cjson_message);
	len=strlen(out);
 	MyUartSend(UART1_PORT,out,len);
	free(out);
	cJSON_Delete(cjson_message);
}
