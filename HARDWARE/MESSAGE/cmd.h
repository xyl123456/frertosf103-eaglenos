/***********************************************************************************
Copyright  agilebiotec Co.,Ltd. 2019-2029.All right reserved.
File name:     cmd.h
Author:        wenyalei
Date:          2019-06-10
Description:   ���İ�ͻ���ͨ�����ڽ���������cjson �����ʽ�����ļ������������꣬��
               ���İ�ͻ���ʹ��
History:
***********************************************************************************/
/*
    �����ʽ����
{
    ��code��:  200,
    ��msg��:��success��,
    ��type��:��cmd��,
    ��value��:
    [
        {
            ��data1��:��value1��,
            ��data2��:��value2��,
        },
        {
            ��data3��:��value3��,
            ��data4��:��value4��,
        },
    ...
    ]
}

*/
#define      CMD_CODE                           "code"                 /*״̬�루�����룩,�����Ե�ֵ��err.h*/
#define      CMD_MSG                            "msg"                  /*��Ϣ˵��*/
#define      CMD_TYPE                           "type"                 /*�������ͣ�������⡢QC�����á����ݹ���*/
#define      CMD_VAL                            "value"                /*�������*/

/*��Ϣֵ*/
#define      CMD_MSG_SUCCESS                    "success" 

/*��������ֵ*/
#define      CMD_TYPE_CONNECT                   "connect"          
#define      CMD_TYPE_SELF_TEST                 "self_test"          
#define      CMD_TYPE_SAMPLE_TEST               "sample_test"
#define      CMD_TYPE_QC                        "qc"
#define      CMD_TYPE_SET                       "set"
#define      CMD_TYPE_DATA_MNG                  "data_manage"
#define      CMD_TYPE_DATA_REPORT               "data_report"       /* �����ϱ� ������ID����⿨��Ϣ�����������*/

/* connect */
#define      CMD_CNT_VAL_DEV                    "device"
#define      CMD_CNT_VAL_USR                    "usrname"
#define      CMD_CNT_VAL_PWD                    "password"


/*Sample test �޲�������*/
#define      CMD_TEST_VAL_POWER                 "power"
#define      CMD_TEST_VAL_SCAN_CARD             "scan_id"


/*Sample test �޲�������(���Ͳ忨����)*/
#define      CMS_TEST_INSERT_CARD               "card_insert"

/*Sample test �в����������Ԥ����*/
#define			 CMD_TEST_VAL_PRE_TEST               "pre_test"       //�������ȣ��ƽ���Һ


/*Sample test �в��������ӡ��*/
#define      CMD_TEST_VAL_PRINT                 "print"                    /*��ӡ����*/ 
#define			 CMD_TEST_CARD_VALUE                "card_value"               /*��ӡ������*/
//#define      CMD_TEST_VAL_K                     "k"                        /* ��ӡ���� k*/
//#define      CMD_TEST_VAL_NA                    "na"                       /* ��ӡ���� na*/
//#define      CMD_TEST_VAL_CA                    "ca"                  
//#define      CMD_TEST_VAL_PH                    "ph"
//#define      CMD_TEST_VAL_PCO2                  "co2"
//#define      CMD_TEST_VAL_PO2                   "o2"

/*��Ƭ��Ϣ*/
#define     CMD_TEST_CARD_ONE                    "test_card_one"
#define     CMD_TEST_CARD_TWO                    "test_card_two"
#define     CMD_TEST_CARD_THREE                  "test_card_three"
#define     CMD_TEST_CARD_FOUR                   "test_card_four"
#define     CMD_TEST_CARD_FIVE                   "test_card_five"

/*�ϱ�����*/
#define     CMD_REPORT_VAL_INIT_STAT             "baseboard_init_stat"      /*�����ʼ��״̬ */
#define     CMD_REPORT_VAL_POTENTIO              "selftest_potentio"        /*��ѹ�Լ��� */
#define     CMD_REPORT_VAL_AMPERO                "selftest_ampero"          /*�����Լ���*/
#define     CMD_REPORT_VAL_IMPEDANCE             "selftest_impedance"       /*�迹�Լ��� */
#define     CMD_REPORT_VAL_PID                   "patient_id"               /*����ID */
#define     CMD_REPORT_VAL_TESTCARD_INFO         "test_card_info"           /*���Կ���Ϣ*/
#define     CMD_REPORT_VAL_TEST_ERR              "test_err"                 /*�����̷����Ĵ���*/
#define     CMD_REPORT_VAL_PH                    "ph"                       /* PHֵ*/
#define     CMD_REPORT_VAL_PCO2                  "pco2"                     /* co2 Ũ��*/
#define     CMD_REPORT_VAL_PO2                   "po2"                      /* o2 Ũ��*/
#define     CMD_REPORT_VAL_K                     "k"                        /* k ����Ũ��*/
#define     CMD_REPORT_VAL_NA                    "na"                       /* na ����Ũ��*/
#define     CMD_REPORT_VAL_CA                    "ca"                       /* ca ����Ũ��*/

/* set ������ */
#define      CMD_VAL_SUBTYPE_QC_REMIND           "qc_remind"                /* ����QC������ */
#define      CMD_VAL_TIME_GAP                    "time_gap"                 /* ÿ�ܡ��¡���QCһ��*/
#define      CMD_VAL_COUNT                       "count"                    /* �豸ʹ�����ɴ�QCһ��*/

/* data manage ������ */

/*Ϊ�� �ȴ�����*/


