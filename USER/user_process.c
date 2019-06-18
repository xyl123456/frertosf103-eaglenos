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

uint8_t startScanFlag=0;

uint8_t heatBodyFlag=0;
float setingTempValue=0;

extern TaskHandle_t MessageProcessTask_Handler;	//֪ͨ������
uint32_t systemState;//ϵͳ״̬������

/*
* @Description������ɨ���豸
* @para ��void
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
* @Description��ֹͣɨ���豸
* @para ��void
* @return void
*/
void StopScanCode()
{
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
		//HeatingTempValue = Max6675_readTemperature(2);//������
		if(reactionTempValue>=setingTempValue){
			TIM_SetCompare3(TIM3,0);//���ռ�ձ�0%
			
			//heatBodyFlag=0;//�������ñ�־λ
			
			//������ϢERR_TEST_PREPARE_SUCCESS
			xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_PREPARE_SUCCESS,
									(eNotifyAction)eSetBits);
			
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
	if(1){
	return ERR_SELFTEST_SUCCESS;
	}
	/*
	return ERR_SELFTEST_POTENTIO;
	return ERR_SELFTEST_AMPERO;
	return ERR_SELFTEST_IMPEDANCE
	*/
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
	if(mymemcmp(keyString,CMD_TEST_VAL_SCAN_CARD,strlen(CMD_TEST_VAL_SCAN_CARD))==0){
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
	//��ӡ����
	if(mymemcmp(keyString,CMD_TEST_VAL_PRINT,strlen(CMD_TEST_VAL_PRINT))==0){
		
	}
	//����ӡ������,������Ӧ����ֵ
	if(mymemcmp(keyString,CMD_TEST_CARD_VALUE,strlen(CMD_TEST_CARD_VALUE))==0){

		if(mymemcmp(valueString,CMD_TEST_CARD_ONE,strlen(CMD_TEST_CARD_ONE))==0){
			uint32_t cardCode=GetTestCardValue(valueString);
			if(cardCode==1){
				//��������
				
			}else{
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_TEST_GET_VALUE,
									(eNotifyAction)eSetBits);
			}
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_TWO,strlen(CMD_TEST_CARD_TWO))==0){
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_THREE,strlen(CMD_TEST_CARD_THREE))==0){
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_FOUR,strlen(CMD_TEST_CARD_FOUR))==0){
		}
		if(mymemcmp(valueString,CMD_TEST_CARD_FIVE,strlen(CMD_TEST_CARD_FIVE))==0){
		}
		
	}
}

/*
* @Description������1���ݴ�����
* @para ��void
* @return ��
*/
void Uart1Process(char *uartMemary,u8 uartLen)
{
	cJSON *root;
	root = cJSON_Parse(uartMemary);
	
	if(NULL != root){
		int getCode = cJSON_GetObjectItem(root,CMD_CODE)->valueint;
		//�������code
		if(getCode == ERR_NONE){
			char *typeSting = cJSON_GetObjectItem(root,"type")->valuestring;
			if(mymemcmp(typeSting,CMD_TYPE_CONNECT,strlen(CMD_TYPE_CONNECT))==0){
				//��������
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
									(uint32_t)ERR_CONNECT_SUCCESS,
									(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeSting,CMD_TYPE_SELF_TEST,strlen(CMD_TYPE_SELF_TEST))==0){
							//�Լ�����ͨ��,�����Լ�����
				uint32_t selfTestCode = SelfTestProcess();
				xTaskNotify((TaskHandle_t)MessageProcessTask_Handler,
							(uint32_t)selfTestCode,
							(eNotifyAction)eSetBits);
			}
			if(mymemcmp(typeSting,CMD_TYPE_SAMPLE_TEST,strlen(CMD_TYPE_SAMPLE_TEST))==0){
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
			if(mymemcmp(typeSting,CMD_TYPE_QC,strlen(CMD_TYPE_QC))==0){
				//QC����
			}
			if(mymemcmp(typeSting,CMD_TYPE_SET,strlen(CMD_TYPE_SET))==0){
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
void Uart2Process(char *uartMemary,u8 uartLen)
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
		/*���ȳ���*/
		case ERR_TEST_HEAT:
			cJSON_AddStringToObject(cjson_message, CMD_MSG, "Heat Error");
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
