/***********************************************************************************
Copyright  Eaglenos Inc. 2019-2029.All right reserved.
File name:     err.h
Author:        wenyalei
Date:          2019-06-10
Description:   错误码,参考血液生化分析仪软件系统设计_v1.0_0610 异常处理章节
History:
***********************************************************************************/
#ifndef __ERR_H
#define __ERR_H

enum ERR_CODE
{
    /* 网络通信 */
    ERR_SERVER_DISPOSE_SUCCESS = 0,     // 服务器成功处理了请求
    ERR_RESOURCE_REMOVED,               // 资源永久转移
    ERR_SERVER_MISUNDERSTAND,           // 服务器不理解
    ERR_SERVER_REFUSE,                  // 服务器拒绝请求
    ERR_CONNECT_TIMEOUT,                // 连接超时
    ERR_SERVER_INTERNAL,                // 服务器内部错误    
    ERR_SERVER_TIMEOUT,                 // 服务器超时
                                        // 可在这里扩展通用错误代码
    /*初始化*/
    ERR_INIT_SUCCESS = 100,             // 初始化成功
    ERR_INIT_MCU,                       // MCU 初始化失败
    ERR_INIT_SRAM,                      // SRAM 初始化失败
    ERR_INIT_NAND,                      // NAND 初始化失败
    ERR_INIT_LCD,                       // LCD 初始化失败
    ERR_INIT_NET,                       // NET 初始化失败
    ERR_INIT_UART,                      // UART 初始化失败
    ERR_INIT_NOACK,                     // 核心板下发命令无回复
    ERR_INIT_DATA,                      // DATA 初始化失败
                                        // 可在这里扩展初始化错误代码

    /* 串口通信 */
    ERR_CONNECT_SUCCESS =  200,         //串口通讯成功

    /* 自检 */
    ERR_SELFTEST_SUCCESS = 300,         // 自检成功    
    ERR_SELFTEST_POTENTIO,              // 电压自检失败    
    ERR_SELFTEST_AMPERO,                // 电流自检失败    
    ERR_SELFTEST_IMPEDANCE,             // 电阻自检失败
	ERR_SELFTEST_ERROR,                 // 自检失败
                                        // 可在这里扩展初自检错误代码

    /*登陆*/
    ERR_LOGIN_SUCCESS = 400,             // 登陆成功
    ERR_LOGIN_USR_EXIST,                 // 用户名存在
    ERR_LOGIN_USR_INEXIST,               // 用户名不存在
    ERR_LOGIN_PWD_ERR,                   // 密码错误
                                         // 可在这里扩展登陆错误代码

    /*样本检测 500-520代表正常状态*/
    ERR_TEST_SUCCESS = 500,
    ERR_TEST_SCAN_SUCCESS,               // 扫描成功
	  ERR_TEST_HEAT_SUCCESS,               // 加热到到指定温度
	  ERR_TEST_CARD_INSERT_SUCCESS,        // 插卡成功
	  ERR_TEST_PREPARE_SUCCESS,			 // 准备已经完成，测试前准备完成
    ERR_TEST_COLLECT_SUCCESS,			 // 收集数据成功
		ERR_TEST_START_HEAT_SUCCESS,    // 开始加热
		ERR_TEST_STOP_HEAT_SUCCESS,		 //停止加热
										 // 样本检测流程的成功状态在这里添加，注意不要超过520
    /*样本检测 521-599代表异常状态*/
	ERR_TEST_GET_VALUE = 521,            // 读取检测卡信息失败
	ERR_TEST_HAVE_RESIDUE_SOLUTION,      // 检测卡有残留液体，无法检测
    ERR_TEST_MOTOR_PUSH,                 // 电机推动失败
    ERR_TEST_HEAT,                       // 加热异常
	ERR_TEST_PREPARE_ERROR,				 //测试前准备失败
    ERR_TEST_INSERT_CARD_TOUT,           // 等待插入测试卡超时
    ERR_TEST_SAMPLE_COLLECT_ERR,         // 样品检测AD采集数据失败
    ERR_TEST_HAS_CARD,                   // 已经有卡，请取卡
    ERR_TEST_CARD_INFO,                  // 扫描测试卡信息错误，如扫了病人条形码
    ERR_TEST_CARD_OUT_OF_DATE,           // 测试卡过期
    ERR_TEST_PID_INFO,                   // 扫描病人条码错误
                                         // 可在这里扩展样品检测错误代码
    /*质控*/
    ERR_QC_SUCCESS = 600,             
                                         // 可在这里扩展样品检测错误代码

    /*设置*/
    ERR_SET_SUCCESS = 700,

                                         // 可在这里扩展样品检测错误代码

    /*数据管理*/
    ERR_DATA_MANAGE_SUCCESS = 800,                           
                                         // 可在这里扩展样品检测错误代码

    /* 其他*/
    ERR_NONE = 900,                      // 正常
    ERR_PARA_INVLD,                      // 参数非法
    ERR_MALLOC_NULL,                     // 分配失败
    ERR_CMD_NOT_SUPPORT,                 // 命令不支持    
                                          
};

#endif
/********************************** End of file ***********************************/

