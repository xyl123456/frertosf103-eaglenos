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

/*标志位，用于标记对应事件发生*/
uint8_t startScanFlag=0;										//用于扫描头启动事件标记
uint8_t heatSendMessageFlag=0;							//加热事件消息发送
uint8_t startBreakVacyoleFlag=0;						//刺破液包事件标志
uint8_t startTestVacyoleFlag=0;							//检测标液体事件标志

uint8_t startTestSampleFlag=0;              //检测样品液事件标志


uint8_t startCheckImpedance=0;             //开启实时检测阻抗标志

uint8_t preTestFlag=0;
uint8_t heatBodyFlag=0;

int sensorCnt=0;
float setingTempValue=0;

TestCard_One 			testCardOne;
TestCard_Two 			testCardTwo;
TestCard_Three 		testCardThree;


extern TaskHandle_t MessageProcessTask_Handler;	//通知处理句柄
uint32_t systemState;//系统状态检测变量

extern TaskHandle_t EventProcessTask_Handler;
void ScanSensorTime(void)
{
	//启动扫描功能,超时发送关闭功能
			if(startScanFlag==1){
				sensorCnt++;
				}else{
				sensorCnt=0;
				}
			if(sensorCnt==20){
				sensorCnt=0;
				//超时，需要关闭扫码头
				StopScanCode();
			}
}

/*
* @Description：启动扫码设备
* @para ：void
* @return void
*/
void StartScanCode()
{
	startScanFlag=1;
	SCANCODEKEY=1;
	SCANCODEKEY=0;
	//vTaskDelay(10000/portTICK_PERIOD_MS);
	//SCANCODEKEY=1;

}

/*
* @Description：停止扫码设备
* @para ：void
* @return void
*/
void StopScanCode(void)
{
	startScanFlag=0;
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
* @Description：判断液体是否到位
* @para ：void
* @return uint8_t 1表示成功，0表示失败
*/
uint8_t TestImpedanceIsOk(void){
		return 1;//测试功能函数
	/*
		int8_t inImpedance,outImpedance;
	
	//插破液包以后检测电阻值,0为液体到位，1为液体未到位
		inImpedance=DetectBoard_Check_R1();
		outImpedance=DetectBoard_Check_R2();
		if((inImpedance==0)&&(outImpedance==0)){
			//液体到位
			return 1;
		}else{
			return 0;
		}
	*/
}


/*
* @Description：实时检测阻抗函数
* @para ：void
* @return 无
*/
void ScanImpedance(void)
{
	//1为停止测试阻抗，0为开始测试阻抗
	if(startCheckImpedance==1){
		return ;
	}
	//已经刺破液包了，开始检测阻抗
	if(startBreakVacyoleFlag==1){
		if(TestImpedanceIsOk()==1){
			//发送开始检测命令，在事件处理任务函数中处理相应的数据消息
			
			if(startTestVacyoleFlag==0){
				startTestVacyoleFlag=1;
				//发送消息处理检测标液数据
				xTaskNotify((TaskHandle_t)EventProcessTask_Handler,
					(uint32_t)EVENT_TEST_VACUOLE_START,
					(eNotifyAction)eSetBits);
			}
			
			//发送检测样品液数据消息
			if(startTestSampleFlag==1){
				startTestSampleFlag=0;
			
			xTaskNotify((TaskHandle_t)EventProcessTask_Handler,
						(uint32_t)EVENT_TEST_SAMPLE_START,
						(eNotifyAction)eSetBits);	
			}
			
		}
	}
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
		//printf("the temp is %.2f\n",reactionTempValue);
		//HeatingTempValue = Max6675_readTemperature(2);//加热区
		if(reactionTempValue>=setingTempValue){
			TIM_SetCompare3(TIM3,10000);//输出占空比0%
			//heatBodyFlag=0;//检测完成置标志位,检测温度会一直处于维持温度中
			if(heatSendMessageFlag==0){
				//发送消息ERR_TEST_PREPARE_SUCCESS
				heatSendMessageFlag=1;
				/*
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);
				*/
				//开始刺破液包
				//发送刺破液包的消息
				xTaskNotify((TaskHandle_t)EventProcessTask_Handler,
					(uint32_t)EVENT_BREAK_VACUOLE,
					(eNotifyAction)eSetBits);
			  }
			}else{
				//加热
				TIM_SetCompare3(TIM3,5000);
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
/*extern 停止加热函数
* @Description：stop Heat
* @para ：void
* @return void
*/
void StopHeatBody(void)
{
	heatBodyFlag=0;
	TIM_SetCompare3(TIM3,10000);//
}

/*extern 函数
* @Description：发送检测到的数据
* @para ：uint8_t sort---卡的种类,uint8_t VacuoleId---样品(1)还是标液(1)
* @return void
*/
void SendSampleCollect(uint8_t sort,uint8_t vacuoleId)
{
	uint8_t i=sort;
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
			if(vacuoleId==0){
			cJSON_AddItemToObject(cjson_message, CMD_VACUOLE_VAL, mes_data = cJSON_CreateArray());
			}else{
			cJSON_AddItemToObject(cjson_message, CMD_SAMPLE_VAL, mes_data = cJSON_CreateArray());
			}
			cJSON_AddItemToArray(mes_data, data_item=cJSON_CreateObject());
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_IMG, cJSON_CreateIntArray(testCardOne.TestCardOne.iMg,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_CA, cJSON_CreateIntArray(testCardOne.TestCardOne.iCa,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_K, cJSON_CreateIntArray(testCardOne.TestCardOne.K,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_NA, cJSON_CreateIntArray(testCardOne.TestCardOne.Na,Apoints));
			cJSON_AddItemToObject(data_item, CMD_REPORT_VAL_CL, cJSON_CreateIntArray(testCardOne.TestCardOne.Cl,Apoints));
			break;
		case 2:
			//mymemcpy(testCardTwo.buffer,buf,sizeof(buf));
			break;
		case 3:
			//mymemcpy(testCardThree.buffer,buf,sizeof(buf));
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
/*压破液包 函数
* @Description：执行压破液包的电机操作
* @para ：void
* @return void
*/
void BreakVacuole(void){
	//标记刺破液包的标记为1
	startBreakVacyoleFlag=1;
	//
}

/*继续推加热片函数(推样品检测函数)
* @Description：推样品检测函数
* @para ：void
* @return uint8_t ,error---0  success---1
*/
uint8_t PushVacuole(void){
	return 1;
}

/*推加热片 函数
* @Description：推加热片函数
* @para ：void
* @return uint8_t ,error---0  success---1
*/
uint8_t PushHeatCut(void){
	
	return 1;
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
	if(mymemcmp(keyString,CMD_TEST_VAL_SCAN,strlen(CMD_TEST_VAL_SCAN))==0){
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
	//样本检测前预处理命令功能，包含刺破液包，推加热片，加热，推动电机压气囊等
	if(mymemcmp(keyString,CMD_TEST_VAL_PRE_TEST,strlen(CMD_TEST_VAL_PRE_TEST))==0){
		double tempValue;
		uint8_t pushStatus;
		tempValue=atof(valueString);
		
		if(preTestFlag==0){
		//1.推加热片
		//2.刺破液包
		//3.加热  class_value=1 为加热电机,
		//int8_t MotorStart(uint8_t class_value,int angle,int speed);
		//int8_t MotorStop(uint8_t class_value);
		//开始加热
		//StartHeatBody(float tempValue);
		
		//推加热片
		pushStatus=PushHeatCut();
		if(pushStatus==1){
			StartHeatBody((float)tempValue);
		}else{
			//发送电机失败消息，加热电机复原
		}
		
	}
		preTestFlag=1;//标记命令已经发送过，重复发送将不处理
		
	}
	//加热控制命令
		if(mymemcmp(keyString,CMD_TEST_VAL_ON_OFF_HEAT,strlen(CMD_TEST_VAL_ON_OFF_HEAT))==0){
			//开启加热
			if(mymemcmp(valueString,"on",2)==0){
				StartHeatBody(setingTempValue);
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_START_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);	
			}
			if(mymemcmp(valueString,"off",3)==0){
				//停止加热
				StopHeatBody();
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_STOP_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);
			}

		}
	//样品采集命令
	if(mymemcmp(keyString,CMD_TEST_VAL_COLLECT,strlen(CMD_TEST_VAL_COLLECT))==0){
		if(mymemcmp(valueString,CMD_TEST_CARD_A,strlen(CMD_TEST_CARD_A))==0){
				StartTestVacuole(CHECK_VACUOLE_TIMER);//
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_B,strlen(CMD_TEST_CARD_B))==0){
			
			DetectBoard_GetCartridgeBData(&testCardTwo);
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_C,strlen(CMD_TEST_CARD_C))==0){
			
			DetectBoard_GetCartridgeCData(&testCardThree); 
		}
		/*
		if(mymemcmp(valueString,CMD_TEST_CARD_A,strlen(CMD_TEST_CARD_A))==0){
			
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_A,strlen(CMD_TEST_CARD_A))==0){
			
		}
		*/
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
void Uart1Process(char *uartMemary,int uartLen)
{
	cJSON *root;
	root = cJSON_Parse(uartMemary);
	
	if(NULL != root){
		int getCode = cJSON_GetObjectItem(root,CMD_CODE)->valueint;
		//其它类的code
		if(getCode == ERR_NONE){
			char *typeString = cJSON_GetObjectItem(root,"type")->valuestring;
			if(mymemcmp(typeString,CMD_TYPE_CONNECT,strlen(CMD_TYPE_CONNECT))==0){
				//串口连接
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_CONNECT_SUCCESS,
									(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeString,CMD_TYPE_SELF_TEST,strlen(CMD_TYPE_SELF_TEST))==0){
							//自检命令通信,处理自检任务
				uint32_t selfTestCode = SelfTestProcess();
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
							(uint32_t)selfTestCode,
							(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeString,CMD_TYPE_SAMPLE_TEST,strlen(CMD_TYPE_SAMPLE_TEST))==0){
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
			if(mymemcmp(typeString,CMD_TYPE_QC,strlen(CMD_TYPE_QC))==0){
				//QC测试
			}
			if(mymemcmp(typeString,CMD_TYPE_SET,strlen(CMD_TYPE_SET))==0){
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
void Uart2Process(char *uartMemary,int uartLen)
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
		StopScanCode();
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
void UartProcess(u8 uartPort,char *uartMemary,int uartLen)
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
* @Description：开始检测标液函数，通过不断发送检测数据
* @para ：void
* @return 无
*/
void 	StartTestVacuole(uint32_t second){
			int8_t sampleCollectState;
			uint32_t timeCnt;
			
			for(timeCnt=0;timeCnt<second;timeCnt++){
				//sampleCollectState=DetectBoard_GetCartridgeAData(&testCardOne);//注释以后调试
					sampleCollectState=0;
					if(sampleCollectState!=0)
					{
						//数据检测错误,发送检测失败消息
						xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
											(uint32_t)ERR_TEST_SAMPLE_COLLECT_ERR,
											(eNotifyAction)eSetBits);
								break;
					}else{
					//发送数据
					SendSampleCollect(SAMPLE_COLLECT_CARD_ONE,TEST_VACUOLE_ID);
					}
				
			}

}

/*
* @Description：开始检测样品液的函数，通过不断发送检测数据
* @para ：void
* @return 无
*/
void  StartSampleVacuole(uint32_t second)
{
			int8_t sampleCollectState;
			uint32_t timeCnt;
			
			for(timeCnt=0;timeCnt<second;timeCnt++){
				//sampleCollectState=DetectBoard_GetCartridgeAData(&testCardOne);//注释以后调试
					sampleCollectState=0;
					if(sampleCollectState!=0)
					{
						//数据检测错误,发送检测失败消息
						xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
											(uint32_t)ERR_TEST_SAMPLE_COLLECT_ERR,
											(eNotifyAction)eSetBits);
								break;
					}else{
					//发送数据
					SendSampleCollect(SAMPLE_COLLECT_CARD_ONE,TEST_SAMPLE_ID);
					}
				
			}
}
/*
* @Description：复位加热电机外设
* @para ：void
* @return void
*/
void	ResectHeatMoto(void){
	
}
/*
* @Description：复位压破液包电机外设
* @para ：void
* @return void
*/
void	ResectBreakMoto(void){
	
}

/*
* @Description：复位所有外设
* @para ：void
* @return void
*/
void ResectAllDevice(void){
	//清空各个标志位,主要是处理加热任务
	/*标志位，用于标记对应事件发生*/
	heatBodyFlag=0;
	heatSendMessageFlag=0;							//加热事件消息发送
	startBreakVacyoleFlag=0;						//刺破液包事件标志
	
	startTestVacyoleFlag=0;							//检测标液体事件标志
	startTestSampleFlag=0;              //检测样品液事件标志
	startCheckImpedance=0;             //开启实时检测阻抗标志
	preTestFlag=0;

	
	ResectHeatMoto();
	ResectBreakMoto();
}

/*
* @Description：事件处理函数
* @para ：void
* @return 无
*/
void EventProcess(uint32_t NotifyValue){
	uint32_t notifyValue;
	notifyValue = NotifyValue;
	switch(notifyValue)
	{
		//case EVENT_PUSH_HEAT_CUT:
			//推加热片
		//	break;
		case EVENT_BREAK_VACUOLE:
			//刺破液包
			BreakVacuole();
		 
		  printf("break vacuole\r\n");
			break;
		case EVENT_PUSH_TEST_VACUOLE:
			//继续推液包里面的检测体
			PushVacuole();
		  printf("continue push vacuole\r\n");
		  //开启阻抗检测功能
			startCheckImpedance=0;
		  startTestSampleFlag=1;
			break;
		case EVENT_TEST_VACUOLE_START:
			//开始检测标液
		  printf("start check vacuole\r\n");
			StartTestVacuole(CHECK_VACUOLE_TIMER);
			DetectBoardResetAll();	
			//停止电阻检测
		  startCheckImpedance=1;
		  //推样本检测液体
			xTaskNotify((TaskHandle_t)EventProcessTask_Handler,
								(uint32_t)EVENT_PUSH_TEST_VACUOLE,
								(eNotifyAction)eSetBits);
			break;
		case EVENT_TEST_SAMPLE_START:
			//开始样本液的检测
			printf("start check sample vacuole\r\n");
			StartSampleVacuole(CHECK_SAMPLE_TIMER);
		  //停止电阻检测
			startCheckImpedance=1; 
			xTaskNotify((TaskHandle_t)EventProcessTask_Handler,
								(uint32_t)EVENT_SAMPLE_TEST_RESECT,
								(eNotifyAction)eSetBits);
			break;
		case EVENT_SAMPLE_TEST_RESECT:
			//复位
		  ResectAllDevice();
			break;
		default:
			break;
		
	}
}



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
		case ERR_SELFTEST_ERROR:
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
		case ERR_TEST_BREAK_MOTOR:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Motor Error");
			break;
		
		/*预加热处理任务成功*/
		case ERR_TEST_PREPARE_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Pre test Success");
			break;
		/*开始加热返回消息*/
		case ERR_TEST_START_HEAT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Heat on Success");
			break;
		/*停止加热返回消息*/
		case ERR_TEST_STOP_HEAT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Heat off Success");
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
