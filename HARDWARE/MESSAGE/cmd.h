/***********************************************************************************
Copyright  agilebiotec Co.,Ltd. 2019-2029.All right reserved.
File name:     cmd.h
Author:        wenyalei
Date:          2019-06-10
Description:   核心板和基板通过串口交互，采用cjson 命令格式，本文件定义相关命令宏，供
               核心板和基板使用
History:
***********************************************************************************/
/*
    命令格式如下
{
    “code”:  200,
    “msg”:”success”,
    “type”:”cmd”,
    “value”:
    [
        {
            “data1”:”value1”,
            “data2”:”value2”,
        },
        {
            “data3”:”value3”,
            “data4”:”value4”,
        },
    ...
    ]
}

*/
#define      CMD_CODE                           "code"                 /*状态码（错误码）,该属性的值见err.h*/
#define      CMD_MSG                            "msg"                  /*消息说明*/
#define      CMD_TYPE                           "type"                 /*命令类型：样本检测、QC、设置、数据管理*/
#define      CMD_VAL                            "value"                /*命令参数*/

/*消息值*/
#define      CMD_MSG_SUCCESS                    "success" 

/*命令类型值*/
#define      CMD_TYPE_CONNECT                   "connect"          
#define      CMD_TYPE_SELF_TEST                 "self_test"          
#define      CMD_TYPE_SAMPLE_TEST               "sample_test"
#define      CMD_TYPE_QC                        "qc"
#define      CMD_TYPE_SET                       "set"
#define      CMD_TYPE_DATA_MNG                  "data_manage"
#define      CMD_TYPE_DATA_REPORT               "data_report"       /* 数据上报 ：病人ID、检测卡信息、采样结果等*/

/* connect */
#define      CMD_CNT_VAL_DEV                    "device"
#define      CMD_CNT_VAL_USR                    "usrname"
#define      CMD_CNT_VAL_PWD                    "password"


/*Sample test 无参子命令*/
#define      CMD_TEST_VAL_POWER                 "power"
#define      CMD_TEST_VAL_SCAN_CARD             "scan_id"


/*Sample test 无参子命令(发送插卡命令)*/
#define      CMS_TEST_INSERT_CARD               "card_insert"

/*Sample test 有参子命令（测试预处理）*/
#define			 CMD_TEST_VAL_PRE_TEST               "pre_test"       //包含加热，推进标液


/*Sample test 有参子命令（打印）*/
#define      CMD_TEST_VAL_PRINT                 "print"                    /*打印命令*/ 
#define			 CMD_TEST_CARD_VALUE                "card_value"               /*打印卡类型*/
//#define      CMD_TEST_VAL_K                     "k"                        /* 打印参数 k*/
//#define      CMD_TEST_VAL_NA                    "na"                       /* 打印参数 na*/
//#define      CMD_TEST_VAL_CA                    "ca"                  
//#define      CMD_TEST_VAL_PH                    "ph"
//#define      CMD_TEST_VAL_PCO2                  "co2"
//#define      CMD_TEST_VAL_PO2                   "o2"

/*卡片信息*/
#define     CMD_TEST_CARD_ONE                    "test_card_one"
#define     CMD_TEST_CARD_TWO                    "test_card_two"
#define     CMD_TEST_CARD_THREE                  "test_card_three"
#define     CMD_TEST_CARD_FOUR                   "test_card_four"
#define     CMD_TEST_CARD_FIVE                   "test_card_five"

/*上报数据*/
#define     CMD_REPORT_VAL_INIT_STAT             "baseboard_init_stat"      /*基板初始化状态 */
#define     CMD_REPORT_VAL_POTENTIO              "selftest_potentio"        /*电压自检结果 */
#define     CMD_REPORT_VAL_AMPERO                "selftest_ampero"          /*电流自检结果*/
#define     CMD_REPORT_VAL_IMPEDANCE             "selftest_impedance"       /*阻抗自检结果 */
#define     CMD_REPORT_VAL_PID                   "patient_id"               /*病人ID */
#define     CMD_REPORT_VAL_TESTCARD_INFO         "test_card_info"           /*测试卡信息*/
#define     CMD_REPORT_VAL_TEST_ERR              "test_err"                 /*检测过程发生的错误*/
#define     CMD_REPORT_VAL_PH                    "ph"                       /* PH值*/
#define     CMD_REPORT_VAL_PCO2                  "pco2"                     /* co2 浓度*/
#define     CMD_REPORT_VAL_PO2                   "po2"                      /* o2 浓度*/
#define     CMD_REPORT_VAL_K                     "k"                        /* k 离子浓度*/
#define     CMD_REPORT_VAL_NA                    "na"                       /* na 离子浓度*/
#define     CMD_REPORT_VAL_CA                    "ca"                       /* ca 离子浓度*/

/* set 子命令 */
#define      CMD_VAL_SUBTYPE_QC_REMIND           "qc_remind"                /* 提醒QC子命令 */
#define      CMD_VAL_TIME_GAP                    "time_gap"                 /* 每周、月、年QC一次*/
#define      CMD_VAL_COUNT                       "count"                    /* 设备使用若干次QC一次*/

/* data manage 子命令 */

/*为完 等待更新*/


