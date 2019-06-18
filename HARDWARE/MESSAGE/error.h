/***********************************************************************************
Copyright  Eaglenos Inc. 2019-2029.All right reserved.
File name:     err.h
Author:        wenyalei
Date:          2019-06-10
Description:   ������,�ο�ѪҺ�������������ϵͳ���_v1.0_0610 �쳣�����½�
History:
***********************************************************************************/
#ifndef __ERROR_H
#define __ERROR_H

enum ERR_CODE
{
    /* ����ͨ�� */
    ERR_SERVER_DISPOSE_SUCCESS = 0,     // �������ɹ�����������
    ERR_RESOURCE_REMOVED,               // ��Դ����ת��
    ERR_SERVER_MISUNDERSTAND,           // �����������
    ERR_SERVER_REFUSE,                  // �������ܾ�����
    ERR_CONNECT_TIMEOUT,                // ���ӳ�ʱ
    ERR_SERVER_INTERNAL,                // �������ڲ�����    
    ERR_SERVER_TIMEOUT,                 // ��������ʱ
                                        // ����������չͨ�ô������
    /*��ʼ��*/
    ERR_INIT_SUCCESS = 100,             // ��ʼ���ɹ�
    ERR_INIT_MCU,                       // MCU ��ʼ��ʧ��
    ERR_INIT_SRAM,                      // SRAM ��ʼ��ʧ��
    ERR_INIT_NAND,                      // NAND ��ʼ��ʧ��
    ERR_INIT_LCD,                       // LCD ��ʼ��ʧ��
    ERR_INIT_NET,                       // NET ��ʼ��ʧ��
    ERR_INIT_UART,                      // UART ��ʼ��ʧ��
    ERR_INIT_NOACK,                     // ���İ��·������޻ظ�
    ERR_INIT_DATA,                      // DATA ��ʼ��ʧ��
                                        // ����������չ��ʼ���������

    /* ����ͨ�� */
    ERR_CONNECT_SUCCESS =  200,         //����ͨѶ�ɹ�

    /* �Լ� */
    ERR_SELFTEST_SUCCESS = 300,         // �Լ�ɹ�    
    ERR_SELFTEST_POTENTIO,              // ��ѹ�Լ�ʧ��    
    ERR_SELFTEST_AMPERO,                // �����Լ�ʧ��    
    ERR_SELFTEST_IMPEDANCE,             // �����Լ�ʧ��
                                        // ����������չ���Լ�������

    /*��½*/
    ERR_LOGIN_SUCCESS = 400,             // ��½�ɹ�
    ERR_LOGIN_USR_EXIST,                 // �û�������
    ERR_LOGIN_USR_INEXIST,               // �û���������
    ERR_LOGIN_PWD_ERR,                   // �������
                                         // ����������չ��½�������

    /*�������*/
    ERR_TEST_SUCCESS = 500,
    ERR_TEST_HAVE_RESIDUE_SOLUTION,      // ��⿨�в���Һ�壬�޷����
    ERR_TEST_MOTOR_PUSH,                 // ����ƶ�ʧ��
    ERR_TEST_HEAT,                       // �����쳣
    ERR_TEST_INSERT_CARD_TOUT,           // �ȴ�������Կ���ʱ
    ERR_TEST_HAS_CARD,                   // �Ѿ��п�
		
		/*�忨�ɹ�*/
		ERR_TEST_CARD_INSERT_SUCCESS,                   //������ɹ�
		
		/*����׼�����*/
		ERR_TEST_PREPARE_SUCCESS,						//׼���Ѿ���ɣ�����ǰ׼�����
		ERR_TEST_PREPARE_ERROR,							//����ǰ׼��ʧ��

		ERR_TEST_GET_VALUE,                  //��ȡ��⿨��Ϣʧ��
                                         // ����������չ��Ʒ���������
    /*�ʿ�*/
    ERR_QC_SUCCESS = 600,             
                                         // ����������չ��Ʒ���������

    /*����*/
    ERR_SET_SUCCESS = 700,

                                         // ����������չ��Ʒ���������

    /*���ݹ���*/
    ERR_DATA_MANAGE_SUCCESS = 800,                           
                                         // ����������չ��Ʒ���������

    /* ����*/
    ERR_NONE = 900,                      // ����
    ERR_PARA_INVLD,                      // �����Ƿ�
    ERR_MALLOC_NULL,                     // ����ʧ��
    ERR_CMD_NOT_SUPPORT,                 // ���֧��    
                                          
};

#endif
