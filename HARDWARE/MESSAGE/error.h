/***********************************************************************************
Copyright  Eaglenos Inc. 2019-2029.All right reserved.
File name:     err.h
Author:        wenyalei
Date:          2019-06-10
Description:   错误码,参考血液生化分析仪软件系统设计_v1.0_0610 异常处理章节
History:
***********************************************************************************/
#ifndef __ERROR_H
#define __ERROR_H

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
                                        // 可在这里扩展初自检错误代码

    /*登陆*/
    ERR_LOGIN_SUCCESS = 400,             // 登陆成功
    ERR_LOGIN_USR_EXIST,                 // 用户名存在
    ERR_LOGIN_USR_INEXIST,               // 用户名不存在
    ERR_LOGIN_PWD_ERR,                   // 密码错误
                                         // 可在这里扩展登陆错误代码

    /*样本检测*/
    ERR_TEST_SUCCESS = 500,
    ERR_TEST_HAVE_RESIDUE_SOLUTION,      // 检测卡有残留液体，无法检测
    ERR_TEST_MOTOR_PUSH,                 // 电机推动失败
    ERR_TEST_HEAT,                       // 加热异常
    ERR_TEST_INSERT_CARD_TOUT,           // 等待插入测试卡超时
    ERR_TEST_HAS_CARD,                   // 已经有卡
		
		/*插卡成功*/
		ERR_TEST_CARD_INSERT_SUCCESS,                   //卡插入成功
		
		/*测试准备完成*/
		ERR_TEST_PREPARE_SUCCESS,						//准备已经完成，测试前准备完成
		ERR_TEST_PREPARE_ERROR,							//测试前准备失败

		ERR_TEST_GET_VALUE,                  //读取检测卡信息失败
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
