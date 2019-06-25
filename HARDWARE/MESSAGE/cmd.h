/***********************************************************************************
Copyright  Eaglenos Inc. 2019-2029.All right reserved.
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
#ifndef __CMD_H
#define __CMD_H

#define      CMD_CODE                           "code"                 //״̬�루�����룩,�����Ե�ֵ��err.h
#define      CMD_MSG                            "msg"                  //��Ϣ˵��
#define      CMD_TYPE                           "type"                 //�������ͣ�������⡢QC�����á����ݹ���
#define      CMD_VAL                            "value"                //�������

/*��Ϣֵ*/
#define      CMD_MSG_SUCCESS                    "success" 

/*��������ֵ*/
#define      CMD_TYPE_CONNECT                   "connect"          
#define      CMD_TYPE_SELF_TEST                 "self_test"          
#define      CMD_TYPE_SAMPLE_TEST               "sample_test"
#define      CMD_TYPE_QC                        "qc"
#define      CMD_TYPE_SET                       "set"
#define      CMD_TYPE_DATA_MNG                  "data_manage"
#define      CMD_TYPE_DATA_REPORT               "data_report"           //�����ϱ� ������ID����⿨��Ϣ�����������

/* connect */
#define      CMD_CNT_VAL_DEV                    "device"
#define      CMD_CNT_VAL_USR                    "usrname"
#define      CMD_CNT_VAL_PWD                    "password"

/*Sample test �޲�������*/
#define      CMD_TEST_VAL_POWER                 "power"                  //���ػ�
#define      CMD_TEST_VAL_SCAN                  "scan_id"                //ɨ��
#define      CMS_TEST_INSERT_CARD               "card_insert"            //�忨

/*Sample test �в����������Ԥ����*/
#define		   CMD_TEST_VAL_PRE_TEST                "pre_test"               //�������ȡ��Ʊ�Һ������Ϊ�¶�
#define      CMD_TEST_VAL_COLLECT               "sample_collect"         //��Ʒ�ɼ�������Ϊ������
#define      CMD_TEST_VAL_PRINT                 "print"                  //��ӡ�����������
#define      CMD_TEST_VAL_ON_OFF_HEAT           "on_off_heat"            //���ؼ�������


#define      CMD_TEST_VAL_K                     "k"                      //��ӡ����������

/*��Ƭ������Ϣ*/
#define     CMD_TEST_CARD_A                     "test_card_a"
#define     CMD_TEST_CARD_B                     "test_card_b"
#define     CMD_TEST_CARD_C                     "test_card_c"

/*�ϱ�����*/
#define     CMD_REPORT_VAL_INIT_STAT             "baseboard_init_stat"    /*�����ʼ��״̬ */
#define     CMD_REPORT_VAL_POTENTIO              "selftest_potentio"      /*��ѹ�Լ��� */
#define     CMD_REPORT_VAL_AMPERO                "selftest_ampero"        /*�����Լ���*/
#define     CMD_REPORT_VAL_IMPEDANCE             "selftest_impedance"     /*�迹�Լ��� */
#define     CMD_REPORT_VAL_PID                   "patient_id"             /*����ID */
#define     CMD_REPORT_VAL_TESTCARD_INFO         "scan_info"         /*���Կ���Ϣ*/
#define     CMD_REPORT_VAL_TEST_ERR              "test_err"               /*�����̷����Ĵ���*/
#define     CMD_REPORT_VAL_PH                    "ph"                     /* PHֵ*/
#define     CMD_REPORT_VAL_PCO2                  "pco2"                   /* co2 Ũ��*/
#define     CMD_REPORT_VAL_PO2                   "po2"                    /* o2 Ũ��*/
#define     CMD_REPORT_VAL_K                     "k"                      /* k ����Ũ��*/
#define     CMD_REPORT_VAL_NA                    "na"                     /* na ����Ũ��*/
#define     CMD_REPORT_VAL_CA                    "ca"                     /* ca ����Ũ��*/
#define     CMD_REPORT_VAL_IMG                   "img"                    /* mg ����Ũ��*/
#define     CMD_REPORT_VAL_CL                    "cl"                     /* cl ����Ũ��*/

/* set ������ */
#define     CMD_VAL_SUBTYPE_QC_REMIND            "qc_remind"              /* ����QC������ */
#define     CMD_VAL_TIME_GAP                     "time_gap"               /* ÿ�ܡ��¡���QCһ��*/
#define     CMD_VAL_COUNT                        "count"                  /* �豸ʹ�����ɴ�QCһ��*/

/* data manage ������ */

/*δ�� �ȴ�����*/

#endif
/********************************** End of file ***********************************/

