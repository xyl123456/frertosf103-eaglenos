/***********************************************************************************
Copyright  agilebiotec Co.,Ltd. 2019-2029.All right reserved.
File name:     user_process.c
Author:        xuyunlong
Date:          2019-6-11
Description:   �û�������
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



int sensorCnt=0;

uint8_t heatBodyFlag=0;
float setingTempValue=0;

TestCard_One 			testCardOne;
TestCard_Two 			testCardTwo;
TestCard_Three 			testCardThree;

//int numb[5]={0x7FFFFF00,3333,5555,6666,888};
extern TaskHandle_t MessageProcessTask_Handler;	//֪ͨ������
uint32_t systemState;//ϵͳ״̬������

void ScanSensorTime(void)
{
	if(startScanFlag==1){
		sensorCnt++;
	}else{
		sensorCnt=0;
	}
	if(sensorCnt==20){
		//��ʱ����Ҫ�ر�ɨ��ͷ
		startScanFlag=0;
		SCANCODEKEY=1;
	}
}

/*
* @Description������ɨ���豸
* @para ��void
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
* @Description��ֹͣɨ���豸
* @para ��void
* @return void
*/
void StopScanCode()
{
	startScanFlag=0;
	SCANCODEKEY=1;
}

/*
* @Description����ʼ��ɨ���豸
* @para ��void
* @return uint32_t value
*/
uint32_t ScanCodeInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 //PB10 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
  GPIO_SetBits(GPIOB,GPIO_Pin_10);						 //PB10�����
	
	return ERR_SELFTEST_SUCCESS;
}

/*
* @Description��ʵʱ����¶Ⱥ���
* @para ��void
* @return ��
*/
void ScanHeatingTemp(void)
{
	float reactionTempValue;//heatingTempValue;
	if(heatBodyFlag==1){
		reactionTempValue = Max6675_readTemperature(1);//��Ӧ��
		//printf("the temp is %.2f\n",reactionTempValue);
		//HeatingTempValue = Max6675_readTemperature(2);//������
		if(reactionTempValue>=setingTempValue){
			TIM_SetCompare3(TIM3,10000);//���ռ�ձ�0%
			//heatBodyFlag=0;//�������ñ�־λ,����¶Ȼ�һֱ����ά���¶���
			if(heatSendMessageFlag==0){
				heatSendMessageFlag=1;
			//������ϢERR_TEST_PREPARE_SUCCESS
			xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);
				}
			}else{
				heatSendMessageFlag=0;
				//����
				TIM_SetCompare3(TIM3,5000);
			}
		}
}

/*
* @Description�����ȳ�ʼ������
* @para ��void
* @return ��
*/
void HeatInit(void){
	//��ͨIO
	/*
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PB0 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
  GPIO_ResetBits(GPIOB,GPIO_Pin_0);						 //PB0���0
	heatBodyFlag=0;
	*/
	heatBodyFlag=0;
	//PWM��ʽ TIME3 ��CH3
	TIM3_PWM_Init(10000,7200);        //7200��Ƶ������1S      //����Ƶ��PWMƵ��=72000000/900=80Khz
	TIM_SetCompare3(TIM3,10000);
}

/*
* @Description���忨����ʼ������
* @para ��void
* @return ��
*/
void CardCheckInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB1 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  		 //��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.1
}

/*
* @Description����������豸��ʼ������
* @para ��void
* @return ��
*/
void HalBoardInit(void)
{
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
	
	CardCheckInit();
	Max6675Init();	
	HeatInit();
	DetectBoard_Initial();//�ɼ����ʼ��
	systemState=ScanCodeInit();
	//systemState=ERR_INIT_SUCCESS;
}


/*
* @Description�����ǰ׼������
* @para ��void
* @return uint32_t success  or  error
*/
uint32_t PreTestProcess(void)
{
	return ERR_TEST_PREPARE_SUCCESS;
}

/*
* @Description���û���ʼ�����
* @para ��void
* @return uint32_t codevalue 
*/
uint32_t IntialProcess(void)
{
	return systemState;
}
/*
* @Description���û��Լ캯��,ͨ�����ö�ӦAPI���سɹ�����ʧ�ܽӿ�
* @para ��void
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


/*extern ���Ⱥ���
* @Description�����ȷ�Ӧ��
* @para ��uint8_t tempValue���趨������¶�
* @return void
*/
void StartHeatBody(float tempValue)
{
	//������־λ�����ҿ�ʼ����
	heatBodyFlag=1;
	setingTempValue=tempValue;
	TIM_SetCompare3(TIM3,5000);//���ռ�ձ�50%
	//HEATCONTROL=1;
	/*
	float ReactionTempValue;//HeatingTempValue;
			ReactionTempValue = Max6675_readTemperature(1);//��Ӧ��
			//HeatingTempValue = Max6675_readTemperature(2);//������
			if(ReactionTempValue < tempValue){
				//HEATCONTROL=1;
				TIM_SetCompare3(TIM3,5000);
			}else{
				//HEATCONTROL=0;
			}
		*/	
		vTaskDelay(1000/portTICK_PERIOD_MS);	
	
}
/*extern ֹͣ���Ⱥ���
* @Description��stop Heat
* @para ��void
* @return void
*/
void StopHeatBody(void)
{
	heatBodyFlag=0;
	TIM_SetCompare3(TIM3,10000);//
}


/*extern ����
* @Description�����ͼ�⵽������
* @para ��void*buf
* @return void
*/
void SendSampleCollect(uint8_t sort)
{
	uint8_t i=sort;
	int len;
	char *out;
	//��װJSON����
	cJSON *cjson_message,*mes_data,*data_item;
	cjson_message = cJSON_CreateObject();
	cJSON_AddNumberToObject(cjson_message,CMD_CODE,ERR_NONE);
	cJSON_AddStringToObject(cjson_message, CMD_MSG, "SampleCollect Success");
	cJSON_AddStringToObject(cjson_message, CMD_TYPE, CMD_TYPE_DATA_REPORT);
	
	switch(i){
		case 1:
			cJSON_AddItemToObject(cjson_message, CMD_VAL, mes_data = cJSON_CreateArray());
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



/*extern ����
* @Description����ȡ���Կ�����ֵ
* @para ��void
* @return uint8_t
*/
uint32_t GetTestCardValue(char *keyString){
	return 1;
}
/*
* @Description�������������ľ����valueֵ�����ݣ���Ҫ���ݵ��Ǽ�ֵ�Խ��д���
* @para ��void
* @return ��
*/
void ProcessValueKeyDate(char *keyString,char *valueString)
{
	//���ػ�
	if(mymemcmp(keyString,CMD_TEST_VAL_POWER,strlen(CMD_TEST_VAL_POWER))==0){
		
		if(mymemcmp(valueString,"on",2)==0){
			
		}else{
			
		}
	}
	//ɨ�蹦��
	if(mymemcmp(keyString,CMD_TEST_VAL_SCAN,strlen(CMD_TEST_VAL_SCAN))==0){
		if(mymemcmp(valueString,"on",2)==0){
			StartScanCode();
		}else{
			StopScanCode();
		}
	}
	//�忨����,�����ϱ�����
//	if(mymemcmp(keyString,CMS_TEST_INSERT_CARD,strlen(CMS_TEST_INSERT_CARD))==0){
//		if(mymemcmp(valueString,"on",2)==0){
//			
//		}
//		
//	}
	
	//�������ǰԤ��������ܣ��������ȣ��ƶ������
	if(mymemcmp(keyString,CMD_TEST_VAL_PRE_TEST,strlen(CMD_TEST_VAL_PRE_TEST))==0){
		double tempValue;
		//1.�Ƽ���Ƭ
		//2.����  class_value=1 Ϊ���ȵ��,
		//int8_t MotorStart(uint8_t class_value,int angle,int speed);
		//int8_t MotorStop(uint8_t class_value);
		//��ʼ����
		//StartHeatBody(float tempValue);
		tempValue=atof(valueString);
		StartHeatBody((float)tempValue);
	}
	//���ȿ�������
		if(mymemcmp(keyString,CMD_TEST_VAL_ON_OFF_HEAT,strlen(CMD_TEST_VAL_ON_OFF_HEAT))==0){
			//��������
			if(mymemcmp(valueString,"on",2)==0){
				StartHeatBody(setingTempValue);
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_START_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);	
			}
			if(mymemcmp(valueString,"off",3)==0){
				//ֹͣ����
				StopHeatBody();
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_STOP_HEAT_SUCCESS,
									(eNotifyAction)eSetBits);
			}

		}
	//��Ʒ�ɼ�����
	if(mymemcmp(keyString,CMD_TEST_VAL_COLLECT,strlen(CMD_TEST_VAL_COLLECT))==0){
		if(mymemcmp(valueString,CMD_TEST_CARD_A,strlen(CMD_TEST_CARD_A))==0){
			int8_t sampleCollectState;
			sampleCollectState=DetectBoard_GetCartridgeAData(&testCardOne);
			if(sampleCollectState!=0)
			{
				//���ݼ�����,���ͼ��ʧ����Ϣ
							xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_SAMPLE_COLLECT_ERR,
									(eNotifyAction)eSetBits);
			}else{
			//��������
			SendSampleCollect(SAMPLE_COLLECT_CARD_ONE);
			}
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
	//��ӡ����
	if(mymemcmp(keyString,CMD_TEST_VAL_PRINT,strlen(CMD_TEST_VAL_PRINT))==0){
		
	}
}

/*
* @Description������1���ݴ�����
* @para ��void
* @return ��
*/
void Uart1Process(char *uartMemary,int uartLen)
{
	cJSON *root;
	root = cJSON_Parse(uartMemary);
	
	if(NULL != root){
		int getCode = cJSON_GetObjectItem(root,CMD_CODE)->valueint;
		//�������code
		if(getCode == ERR_NONE){
			char *typeString = cJSON_GetObjectItem(root,"type")->valuestring;
			if(mymemcmp(typeString,CMD_TYPE_CONNECT,strlen(CMD_TYPE_CONNECT))==0){
				//��������
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_CONNECT_SUCCESS,
									(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeString,CMD_TYPE_SELF_TEST,strlen(CMD_TYPE_SELF_TEST))==0){
							//�Լ�����ͨ��,�����Լ�����
				uint32_t selfTestCode = SelfTestProcess();
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
							(uint32_t)selfTestCode,
							(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeString,CMD_TYPE_SAMPLE_TEST,strlen(CMD_TYPE_SAMPLE_TEST))==0){
				//��Ʒ���������
				//��ȡvalue����ֵ
				cJSON *valueArry = cJSON_GetObjectItem(root,"value");//��ȡvalue����
				int arraySize = cJSON_GetArraySize(valueArry);//��ȡ�����С
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
				//QC����
			}
			if(mymemcmp(typeString,CMD_TYPE_SET,strlen(CMD_TYPE_SET))==0){
				//SET����
			}
		}
		
		/*
		if(parseValueDataLog==1){
			char *typeSting = cJSON_GetObjectItem(root,"type")->valuestring;
			//SendString(typeSting,strlen(typeSting));
			//�����豸���ʹ���
			if(memcmp(typeSting,"heart",5)==0){
				//����������ֵ
				ResponHeart();
			}
			if(memcmp(typeSting,"sample_test",11)==0){
				//��ȡ����ֵ
				cJSON *valueArry = cJSON_GetObjectItem(root,"value");//��ȡvalue����
				int arraySize = cJSON_GetArraySize(valueArry);//��ȡ�����С
				
				int i,j;
				for(i=0;i<arraySize;i++){
					cJSON *valueList = cJSON_GetArrayItem(valueArry,i);
					for(j=0;j<cJSON_GetArraySize(valueList);j++){
						char *keyString = cJSON_GetArrayItem(valueList,j)->string;
						char *valueString = cJSON_GetArrayItem(valueList,j)->valuestring;
						
						ProcessAdc(keyString,valueString);//������������
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
* @Description������2���ݴ�����
* @para ��char *uartMemary,u8 uartLen
* @return ��
*/
void Uart2Process(char *uartMemary,int uartLen)
{
	int len;
	char *out;
	//��װJSON����
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
		SCANCODEKEY=1;
		startScanFlag=0;
	}
	free(out);
	//cJSON_Delete(mes_data);
	//cJSON_Delete(data_item);
	cJSON_Delete(cjson_message);
}



/*
* @Description�����ݴ�����
* @para ��void
* @return ��
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
//{
    "code":  200,
    "messg":��success��,
//}
*/

/*
* @Description����Ϣ������
* @para ��uint32_t NotifyValue ���ݵ���Ϣֵ
* @return ��
*/
void MessageProcess(uint32_t NotifyValue)
{
	int len;
	char *out;
	//��װJSON����
	cJSON *cjson_message;
	uint32_t notifyValue;
	notifyValue=NotifyValue;
	cjson_message = cJSON_CreateObject();
	cJSON_AddNumberToObject(cjson_message,CMD_CODE,notifyValue);
	
	switch(notifyValue){
		/*��ʼ���ɹ�*/
		case ERR_INIT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Init Success");
			break;
		case ERR_INIT_UART:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Uart Init Error");
			break;
		/* �Լ� */
		case ERR_SELFTEST_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Self Test Success");
			break;
		case ERR_SELFTEST_POTENTIO:
		case ERR_SELFTEST_AMPERO:
		case ERR_SELFTEST_IMPEDANCE:
		case ERR_SELFTEST_ERROR:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Self Test Error");
			break;
		/*�������*/
		case ERR_TEST_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Simple Test Success");
			break;
		case ERR_TEST_HAVE_RESIDUE_SOLUTION:
		case ERR_TEST_INSERT_CARD_TOUT:
		case ERR_TEST_GET_VALUE:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Test Card Error");
			break;
		/*�����������*/
		case ERR_TEST_MOTOR_PUSH:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Motor Error");
			break;
		/*Ԥ���ȴ�������ɹ�*/
		case ERR_TEST_PREPARE_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Pre test Success");
			break;
		/*��ʼ���ȷ�����Ϣ*/
		case ERR_TEST_START_HEAT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Heat on Success");
			break;
		/*ֹͣ���ȷ�����Ϣ*/
		case ERR_TEST_STOP_HEAT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Heat off Success");
			break;
		case ERR_TEST_HEAT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG,"Heat to set value");
			break;
		/*���ȳ���*/
		case ERR_TEST_HEAT:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Heat Error");
			break;
		/*��⿨������*/
		case ERR_TEST_SAMPLE_COLLECT_ERR:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Sample Collect Error");
			break;
		/*�������ӳɹ�*/
		case ERR_CONNECT_SUCCESS:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Uart Success");
		break;
		/*������ɹ�*/
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
